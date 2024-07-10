// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *
 */

#ifndef __MAILBOX_CSR_H__
#define __MAILBOX_CSR_H__

#include <common.h>

#define E21_IDLE                0x00000000
#define E21_CHK_BUSY_VAL        0x00000001
#define E21_IRQ_BIT             0x00000001

/* Mailbox Registers start: */
//#define MAILBOX_BASE 0x00000000 
#define REG_WR_DATA_0           (u32)(0x0000)/*Write FIFO data register*/
#define REG_WR_DATA_1           (u32)(0x0004)/*Write FIFO data register.The highest bit indicates
                                               whether the data is written to FIFO*/
#define REG_RD_DATA_0           (u32)(0x0008)/*Read FIFO data register*/
#define REG_RD_DATA_1           (u32)(0x000C)/*Read FIFO data register*/
#define REG_FIFO_STATUS         (u32)(0x0010)
#define REG_MB_ERR              (u32)(0x0014)
#define REG_MB_INT              (u32)(0x0018)
#define REG_WR_LOCK             (u32)(0x001C)
/* Mailbox Registers end. */


typedef struct
{
    u8 full:1; //FIFO full status flag
    u8 empty:1; //FIFO empty status flag
    u8 reserved0:6;
    u8 reserved1:8;
    u8 reserved2:8;
    u8 reserved3:8;
}FIFO_STATUS_T;

typedef struct
{
    u8 mb_err:1;//To indicate whether the FIFO of the mailbox is full-write or empty-read
    u8 reserved0:7;
    u8 reserved1:8;
    u8 reserved2:8;
    u8 reserved3:8;
}MB_ERR_T;

typedef struct
{
    u8 mb_int0:1;
    u8 mb_int1:1;
    u8 mb_int2:1;
    u8 mb_int3:1;
    u8 mb_int4:1;
    u8 mb_int5:1;
    u8 mb_int6:1;
    u8 mb_int7:1;
    
    u8 mb_int8:1;
    u8 mb_int9:1;
    u8 mb_int10:1;
    u8 mb_int11:1;
    u8 reserved0:4;
    
    u8 reserved1:8;
    u8 reserved2:8;
}MB_INT_T;

typedef struct
{
    u8 wr_lock0:1;
    u8 wr_lock1:1;
    u8 wr_lock2:1;
    u8 wr_lock3:1;
    u8 wr_lock4:1;
    u8 wr_lock5:1;
    u8 wr_lock6:1;
    u8 wr_lock7:1;
    
    u8 wr_lock8:1;
    u8 wr_lock9:1;
    u8 wr_lock10:1;
    u8 wr_lock11:1;
    u8 reserved0:4;
    u8 reserved1:8;
    u8 reserved2:8;
}WR_LOCK_T;

#define BIT_FIFO_FULL     0
#define BIT_FIFO_EMPTY    1

#define BIT_MB_ERR   0

#define BIT_MB_INT0  0
#define BIT_MB_INT1  1
#define BIT_MB_INT2  2
#define BIT_MB_INT3  3
#define BIT_MB_INT4  4
#define BIT_MB_INT5  5
#define BIT_MB_INT6  6
#define BIT_MB_INT7  7
#define BIT_MB_INT8  8
#define BIT_MB_INT9  9
#define BIT_MB_INT10  10
#define BIT_MB_INT11  11


#define BIT_WR_LOCK0  0
#define BIT_WR_LOCK1  1
#define BIT_WR_LOCK2  2
#define BIT_WR_LOCK3  3
#define BIT_WR_LOCK4  4
#define BIT_WR_LOCK5  5
#define BIT_WR_LOCK6  6
#define BIT_WR_LOCK7  7
#define BIT_WR_LOCK8  8
#define BIT_WR_LOCK9  9
#define BIT_WR_LOCK10  10
#define BIT_WR_LOCK11  11

#define GET_BIT(var,n) (var&(1<<n))
#define SET_BIT(var,n) (var|=(1<<n))


#endif







