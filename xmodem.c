/*
 * Copyright 2001-2010 Georges Menie (www.menie.org)
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the University of California, Berkeley nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE REGENTS AND CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* this code needs standard functions memcpy() and memset()
 and input/output functions _inbyte() and _outbyte().

 the prototypes of the input/output functions are:
 int _inbyte(unsigned short timeout); // msec timeout
 void _outbyte(int c);

 */
#include <Project_Source/ATP_SW/Include/mistIncludes.h>

#include <Project_Source/Utilities/Xmodem/crc16.h>
#include "ti/devices/msp432e4/driverlib/driverlib.h"
#include "ti/devices/msp432e4/inc/msp.h"
#include <stdint.h>
#include <string.h>
#include "ustdlib.h"
#include "uartstdio.h"

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define CAN  0x18
#define CTRLZ 0x1A

#define DLY_1S 1000
#define MAXRETRANS 25


UINT8 gu8BootData[21000];

static int check(int crc, const unsigned char *buf, int sz)
{
    if (crc)
    {
        unsigned short crc = crc16_ccitt(buf, sz);
        unsigned short tcrc = (buf[sz] << 8) + buf[sz + 1];
        if (crc == tcrc)
            return 1;
    }
    else
    {
        int i;
        unsigned char cks = 0;
        for (i = 0; i < sz; ++i)
        {
            cks += buf[i];
        }
        if (cks == buf[sz])
            return 1;
    }

    return 0;
}

static void flushinput(void)
{
    while (_inbyte(((DLY_1S) * 3) >> 1) >= 0)
        ;
}

#if 1
int xmodemReceive(unsigned char *dest, int destsz)
{

    UARTprintf("UARTReceive throough Xmodem\n");
    unsigned char xbuff[1030]; /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */
    unsigned char *p;
    int bufsz, crc = 0;
    unsigned char trychar = 'C';
    unsigned char packetno = 1;
    unsigned char c;
    int i, len = 0;
    int retry;
    int retrans = MAXRETRANS;
    int u8RetVal;
    //  u32SDRAMAddr = 0;

    //  unsigned int u32SDRAMAddr = 0;

    unsigned int u32Index = 0;

    unsigned int count = 0;

    for(;;)
    {

        for( retry = 1; retry; ++retry)
        {
            if (trychar)
            {

                ROM_UARTCharPutNonBlocking(UART0_BASE, trychar);
            }
            //            if ((c = _inbyte((DLY_1S)<<10)) >= 0)
            if ((c = ROM_UARTCharGetNonBlocking(UART0_BASE)) >= 0)
            {
                //                while (ROM_UARTCharsAvail(UART0_BASE))
                //                  {
                //
                //                      ROM_UARTCharGetNonBlocking(UART0_BASE);
                //                  }
                switch (c)
                {
                case SOH:
                {

                    //UARTprintf("receiving the data\n");
                    bufsz = 128;
                    goto start_recv;
                }
                case STX:
                {
                    bufsz = 1024;
                    goto start_recv;
                }
                case EOT:
                {
                    //                    flushinput();
                    //                    _outbyte(ACK);
                    ROM_UARTCharPut(UART0_BASE, ACK);

                    return len; /* normal end */
                }
                case CAN:
                {
                    if ((c = ROM_UARTCharGet(UART0_BASE)) == CAN)
                    {
                        //                        flushinput();
                        //                        _outbyte(ACK);
                        ROM_UARTCharPut(UART0_BASE, ACK);
                        return -1; /* canceled by remote */
                    }
                    break;
                }
                default:
                    break;
                }

            }
        }
        if (trychar == 'C') { trychar = NAK; continue; }
        flushinput();
        ROM_UARTCharPut(UART0_BASE, CAN);
        ROM_UARTCharPut(UART0_BASE, CAN);
        ROM_UARTCharPut(UART0_BASE, CAN);
        //        _outbyte(CAN);
        //        _outbyte(CAN);
        //        _outbyte(CAN);
        return -2; /* sync error */

        start_recv:
        if (trychar == 'C') crc = 1;
        trychar = 0;
        p = xbuff;
        *p++ = c;
        int k=0;
        for (i = 0;  i < (bufsz+(crc?1:0)+3); ++i)
        {
            if ((c =ROM_UARTCharGet(UART0_BASE)) < 0)
            {
                goto reject;
            }
            *p++ = c;
            // gu8BootData[k]=c;
            // k++;
        }
        //        memcpy (&gu8BootData, &xbuff[2], 4096);




        /* // memcpy (&dest[len], &xbuff[3], count);
        u32SDRAMAddr = 0x60000000;
         *******SDRAM Data Copy using X-mode **********************
        memcpy(&SDRAMWriteDATA[0],&xbuff[3],count);

        for(u32Index = 0; u32Index < count/4; u32Index++)
        {

            sdram32bitAddrPointer[u32SDRAMAddr] = SDRAMWriteDATA[u32Index++];

            u32SDRAMAddr++;
        }
         */
        if (xbuff[1] == (unsigned char)(~xbuff[2]) &&
                (xbuff[1] == packetno || xbuff[1] == (unsigned char)packetno-1) &&
                check(crc, &xbuff[3], bufsz)) {
            if (xbuff[1] == packetno)   {
                count = destsz - len;
                if (count > bufsz)
                {
                    count = bufsz;
                }

                if (count > 0) {


                    //memcpy (&gu8BootData[len], &xbuff[3], count);

                    memcpy (&dest[len], &xbuff[3], count);
                    /********SDRAM Data Copy using X-mode ***********************/
                    //                memcpy(&SDRAMWriteDATA[0],&xbuff[3],count);
                    //
                    /* for(u32Index = 0; u32Index < count/4; u32Index++)
                   {

                       sdram32bitAddrPointer[u32SDRAMAddr] = SDRAMWriteDATA[u32Index++];

                       u32SDRAMAddr++;
                   }*/


                    // u8RetVal = SDRAM_Write(0x60000000,&xbuff[3],count);

                    len += count;
                }
                ++packetno;
                retrans = MAXRETRANS+1;
            }
            if (--retrans <= 0) {
                flushinput();
                ROM_UARTCharPut(UART0_BASE, CAN);
                ROM_UARTCharPut(UART0_BASE, CAN);
                ROM_UARTCharPut(UART0_BASE, CAN);
                //        _outbyte(CAN);
                //        _outbyte(CAN);
                //        _outbyte(CAN);
                return -3; /* too many retry error */
            }
            //            _outbyte(ACK);
            ROM_UARTCharPut(UART0_BASE, ACK);
            continue;
        }
        reject:
        flushinput();
        ROM_UARTCharPut(UART0_BASE, NAK);
        //        _outbyte(NAK);
    }
}

#endif


#if 1
int xmodemTransmit(unsigned char *src, int srcsz)
{
    //  unsigned char xbuff[1030]; /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */
    unsigned char xbuff[40192]; /* 1024 for XModem 1k + 3 head chars + 2 crc + nul */
    int bufsz, crc = -1;
    unsigned char packetno = 1;
    int i, c, len = 0;
    int retry;

    for (;;)
    {
        for (retry = 0; retry < 16; ++retry)
        {
            // if ((c = _inbyte((DLY_1S) << 1)) >= 0)
           // if((c = ROM_UARTCharGet(UART0_BASE)) >= 0)
            if((c = ROM_UARTCharGet(UART2_BASE)) >= 0)
            {
                switch (c)
                {
                case 'C':
                    crc = 1;
                    //  UARTprintf("X\n");

                    goto start_trans;
                case NAK:
                    crc = 0;

                    goto start_trans;
                case CAN:
                    //if ((c = _inbyte(DLY_1S)) == CAN)
                   // if ((c = ROM_UARTCharGet(UART0_BASE)) >= CAN)
                    if ((c = ROM_UARTCharGet(UART2_BASE)) >= CAN)
                    {
                       // _outbyte(ACK);
                        ROM_UARTCharPut(UART2_BASE, ACK);
                        flushinput();
                        return -1; /* canceled by remote */
                    }
                    break;
                default:
                    break;
                }
            }
        }
        ROM_UARTCharPut(UART2_BASE, CAN);// _outbyte(CAN);
        ROM_UARTCharPut(UART2_BASE, CAN); // _outbyte(CAN);
        ROM_UARTCharPut(UART2_BASE, CAN);//_outbyte(CAN);
        flushinput();
        return -2; /* no sync */

        for (;;)
        {
            start_trans: xbuff[0] = SOH;
            bufsz = 128;
            xbuff[1] = packetno;
            xbuff[2] = ~packetno;
            c = srcsz - len;
            if (c > bufsz)
                c = bufsz;
            if (c >= 0)
            {
                memset(&xbuff[3], 0, bufsz);
                if (c == 0)
                {
                    xbuff[3] = CTRLZ;
                }
                else
                {
                    memcpy(&xbuff[3], &src[len], c);
                    if (c < bufsz)
                        xbuff[3 + c] = CTRLZ;
                }
                if (crc)
                {
                    unsigned short ccrc = crc16_ccitt(&xbuff[3], bufsz);
                    xbuff[bufsz + 3] = (ccrc >> 8) & 0xFF;
                    xbuff[bufsz + 4] = ccrc & 0xFF;
                }
                else
                {
                    unsigned char ccks = 0;
                    for (i = 3; i < bufsz + 3; ++i)
                    {
                        ccks += xbuff[i];
                    }
                    xbuff[bufsz + 3] = ccks;
                }
                for (retry = 0; retry < MAXRETRANS; ++retry)
                {
                    for (i = 0; i < bufsz + 4 + (crc ? 1 : 0); ++i)
                    {
                        ROM_UARTCharPut(UART2_BASE, xbuff[i]);// _outbyte(xbuff[i]);
                        //UARTprintf(" D:%d",i);
                    }
                    if ((c = ROM_UARTCharGet(UART2_BASE)) >= 0)
                    {
                        switch (c)
                        {
                        case ACK:
                            //  UARTprintf("ACK \n",i);
                            ++packetno;
                            len += bufsz;
                            goto start_trans;
                        case CAN:
                            if ((c = ROM_UARTCharGet(UART2_BASE)) == CAN)
                            {
                                ROM_UARTCharPut(UART2_BASE, ACK);//  _outbyte(ACK);
                               // flushinput();
                                return -1; /* canceled by remote */
                            }
                            break;
                        case NAK:
                        default:
                            break;
                        }
                    }
                }
                ROM_UARTCharPut(UART2_BASE, CAN);//_outbyte(CAN);
                ROM_UARTCharPut(UART2_BASE, CAN);//_outbyte(CAN);
                ROM_UARTCharPut(UART2_BASE, CAN);//_outbyte(CAN);
              //  flushinput();
                return -4; /* xmit error */
            }
            else
            {
                for (retry = 0; retry < 10; ++retry)
                {
                    ROM_UARTCharPut(UART2_BASE, EOT); //_outbyte(EOT);

                   // if ((c = _inbyte((DLY_1S) << 1)) == ACK)
                        if ((c = ROM_UARTCharGet(UART2_BASE)) == ACK)
                        break;
                }

                //flushinput();

                return (c == ACK) ? len : -5;
            }
        }
    }
}

#endif


int _inbyte(unsigned int timeout) // msec timeout
{

    return (ROM_UARTCharGet(UART0_BASE));
    //    return (UARTCharGetTimeout(UART0_BASE, timeout));
}
void _outbyte(int c)
{
    ROM_UARTCharPut(UART0_BASE, c);

}

#if 0
#ifdef TEST_XMODEM_RECEIVE
int main(void)
{
    int st;

    printf ("Send data using the xmodem protocol from your terminal emulator now...\n");
    /* the following should be changed for your environment:
       0x30000 is the download address,
       65536 is the maximum size to be written at this address
     */
    st = xmodemReceive((char *)0x30000, 65536);
    if (st < 0) {
        printf ("Xmodem receive error: status: %d\n", st);
    }
    else  {
        printf ("Xmodem successfully received %d bytes\n", st);
    }

    return 0;
}
#endif
#ifdef TEST_XMODEM_SEND
int main(void)
{
    int st;

    printf ("Prepare your terminal emulator to receive data now...\n");
    /* the following should be changed for your environment:
       0x30000 is the download address,
       12000 is the maximum size to be send from this address
     */
    st = xmodemTransmit((char *)0x60000000, 1024);
    if (st < 0) {
        printf ("Xmodem transmit error: status: %d\n", st);
    }
    else  {
        printf ("Xmodem successfully transmitted %d bytes\n", st);
    }

    return 0;
}
#endif
#endif





