/*
 * ipsec_ocf_debug.h -- 
 *
 * Copyright (C) 2001 Alessandro Rubini and Jonathan Corbet
 * Copyright (C) 2001 O'Reilly & Associates
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.  The citation
 * should list that the code comes from the book "Linux Device
 * Drivers" by Alessandro Rubini and Jonathan Corbet, published
 * by O'Reilly & Associates.   No warranty is attached;
 * we cannot take responsibility for errors or fitness for use.
 *
 */

#ifndef _IPSEC_OCF_H_
#define _IPSEC_OCF_H_


/*
 * Macros to help debugging
 */

//#define IPSEC_OCF_DEBUG

#ifdef IPSEC_OCF_DEBUG
#    define PDEBUG(fmt, args...) printk( "ocf: " fmt, ## args)
#else
#  define PDEBUG(fmt, args...) /* not debugging: nothing */
#endif


#endif /* _IPSEC_OCF_H_ */
