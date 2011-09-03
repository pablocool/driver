/*
 * micom_asc.c
 *
 * (c) 2011 konfetti
 * partly copied from user space implementation from M.Majoor
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/termbits.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,17)
#include <linux/stm/pio.h>
#else
#include <linux/stpio.h>
#endif
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/poll.h>

#include "micom.h"
#include "micom_asc.h"

//-------------------------------------

unsigned int InterruptLine   = 121;
unsigned int ASCXBaseAddress = ASC2BaseAddress;

//-------------------------------------

void serial_init (void)
{
    // Configure the asc input/output settings
    *(unsigned int*)(ASCXBaseAddress + ASC_INT_EN)   = 0x00000000; // TODO: Why do we set here the INT_EN again ???
    *(unsigned int*)(ASCXBaseAddress + ASC_CTRL)     = 0x00000589; //1589
    *(unsigned int*)(ASCXBaseAddress + ASC_TIMEOUT)  = 0x00000014; //10 
    *(unsigned int*)(ASCXBaseAddress + ASC_BAUDRATE) = 0x0000028a; //c9
    *(unsigned int*)(ASCXBaseAddress + ASC_TX_RST)   = 0;
    *(unsigned int*)(ASCXBaseAddress + ASC_RX_RST)   = 0;
}

int serial_putc (char Data)
{
    char                  *ASCn_TX_BUFF = (char*)(ASCXBaseAddress + ASC_TX_BUFF);
    unsigned int          *ASCn_INT_STA = (unsigned int*)(ASCXBaseAddress + ASC_INT_STA);
    unsigned long         Counter = 200000;

    while (((*ASCn_INT_STA & ASC_INT_STA_THE) == 0) && --Counter)
         mdelay(1);

    if (Counter == 0)
    {
        dprintk(1, "Error writing char\n");
    }

    *ASCn_TX_BUFF = Data;
    return 1;
}
