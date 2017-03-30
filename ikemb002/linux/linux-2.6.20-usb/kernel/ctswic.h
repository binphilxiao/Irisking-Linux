/*******************************************************************
 * ctswic.h
 *
 * Copyright (C) 2007 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 ********************************************************************/

/*
* $Source: /u/bothell/cttarget/rtos/linux/common/src/ct_files/ctswic.h,v $
* $Revision: 1.7 $
* $Author: rionescu $
* $Date: 2005/05/23 16:11:06 $
*/
#ifndef CT_SWIC_H_
#define CT_SWIC_H_

/* define this if you disabled in ctdriver.c */
#ifdef CONFIG_ARM
#define CT_SWIC_BUFFER_NOCACHE
#endif

typedef struct Q_Tag {
  u32 tag ;
  u32 a_time ;
} Q_Tag ;

#define HOOK_BUFFER_SIZE  2048  /* must be a power of two */

struct Hook_Buffer {
  int head ;
  volatile int tail ;
  Q_Tag buffer[HOOK_BUFFER_SIZE] ;
} ;

#endif
