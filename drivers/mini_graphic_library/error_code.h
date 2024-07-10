// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *
 */
/************************************************************************
*  @file error_code.h
*
*  @brief For cross-platforms error code definitions.
**************************************************************************/

#ifndef  __ERROR_CODE_H
#define  __ERROR_CODE_H

/**************************************************************************
* Includes
***************************************************************************/

#include "errno.h"

#ifndef __ASSEMBLER__
//#if defined(LINUX) || defined(__linux) || defined(__CYGWIN__)
//#       if defined(__KERNEL__) && !defined(__CYGWIN__)
//#        include <stdarg.h>
//#        include <linux/config.h>
//#        include <linux/version.h>
//#        include <linux/module.h>
//#        if defined(MODVERSIONS)
//#        include <linux/modversions.h>
//#        endif /*MODVERSIONS*/
//#        include <linux/kernel.h>
//#        include <linux/string.h>
//#        include <linux/errno.h>
//#        include <linux/sched.h>
//#        include <semaphore.h>     
//#        include <linux/types.h>
//#        include <linux/kernel.h>
//#       else  /* !__KERNEL__*/
//#        include <stdarg.h>
//#        include <errno.h>
//#        include <assert.h>
//#        include <stdio.h>
//#        include <ctype.h>
//#        include <sched.h>      
//#        include <stddef.h>
//#       endif  /* __KERNEL__ */                                                                             
//#    else /*LINUX*/
//#    endif  /* WIN32 */
//
///* Nothing to include */
#if defined(__cplusplus)
extern "C" {
#endif /*__cplusplus*/


/**************************************************************************
* ERROR CODE DEFINITIONS
***************************************************************************/

#ifndef   RET_OK
#define   RET_OK           0
#endif /*RET_OK*/

#ifndef   RET_FAILED
#define   RET_FAILED      -1
#endif /*RET_FAILED*/

#ifdef ERR_INVAL
#undef ERR_INVAL              
#endif 

#ifdef ERR_TIMEOUT            
#undef ERR_TIMEOUT            
#endif 

#if defined(LINUX) || defined(__UBOOT__)

    #define   ERR_PERM	      EPERM		 	  /* Operation not permitted */
    #define   ERR_ENOENT	    ENOENT		  /* No such file or directory */
    #define   ERR_EINTR       EINTR		 	  /* Interrupted system call */
    #define   ERR_IO        	EIO		      /* I/O error */
    #define	  ERR_NXIO        ENXIO		 	  /* No such device or address */
    #define	  ERR_AGAIN		    EAGAIN  	  /* Try again */
    #define   ERR_OOMEM     	ENOMEM		  /* Out of memory */
    #define	  ERR_ACCES       EACCES		  /* Permission denied */
    #define	  ERR_FAULT       EFAULT		  /* Bad address */
    #define   ERR_BUSY      	EBUSY			  /* Device or resource busy */
    #define	  ERR_EXIST       EEXIST  	  /* File exists */
    #define	  ERR_NODEV		    ENODEV   	  /* No such device */
    #define	  ERR_NOTDIR      ENOTDIR		  /* Not a directory */
    #define	  ERR_ISDIR       EISDIR		  /* Is a directory */
    #define   ERR_INVAL     	EINVAL		  /* Invalid argument */
    #define	  ERR_FBIG        EFBIG	  	  /* File too large */
    #define	  ERR_NOSPC       ENOSPC  	  /* No space left on device */
    #define	  ERR_RONLY       EROFS			  /* Read-only file system */
    #define	  ERR_PIPE        EPIPE			  /* Broken pipe */
    #define   ERR_NOSYS     	ENOSYS			/* Function not implemented */
    #define	  ERR_NOTEMPTY    ENOTEMPTY		/* Directory not empty */
    #define	  ERR_WOULDBLOCK  EWOULDBLOCK	/* Operation would block */
    #define	  ERR_BADSLT      EBADSLT			/* Invalid slot */
    #define	  ERR_NODATA      ENODATA			/* No data available */
    #define   ERR_TIMEOUT   	ETIME			  /* Timer expired */
    #define	  ERR_PROTO       EPROTO		  /* Protocol error */
    #define	  ERR_BADFD       EBADFD		  /* File descriptor in bad state */
    #define	  ERR_USERS       EUSERS		  /* Too many users */
    #define	  ERR_NOBUFS      ENOBUFS		  /* No buffer space available */
    #define   ERR_UNKNOWN     501         /* Unknown error */
    #define   ERR_USER        2048        /* User defined error code*/
#else /*!LINUX*/  /*!__UBOOT__*/
    #define   ERR_PERM	        -256		 	/* Operation not permitted */
    #define   ERR_ENOENT	      -257		  /* No such file or directory */
    #define   ERR_EINTR        	-258      /* Interrupted system call */
    #define   ERR_IO        	  -259      /* I/O error */
    #define	  ERR_NXIO         	-260      /* No such device or address */
    #define	  ERR_AGAIN		    	-261      /* Try again */
    #define   ERR_OOMEM     		-262      /* Out of memory */
    #define	  ERR_ACCES       	-263      /* Permission denied */
    #define	  ERR_FAULT       	-264      /* Bad address */
    #define   ERR_BUSY      		-265      /* Device or resource busy */
    #define	  ERR_EXIST       	-266      /* File exists */
    #define	  ERR_NODEV		     	-267      /* No such device */
    #define	  ERR_NOTDIR      	-268      /* Not a directory */
    #define	  ERR_ISDIR       	-269      /* Is a directory */
    #define   ERR_INVAL     		-270      /* Invalid argument */
    #define	  ERR_FBIG        	-271      /* File too large */
    #define	  ERR_NOSPC       	-272      /* No space left on device */
    #define	  ERR_RONLY       	-273      /* Read-only file system */
    #define	  ERR_PIPE        	-274      /* Broken pipe */
    #define   ERR_NOSYS     		-275      /* Function not implemented */
    #define	  ERR_NOTEMPTY    	-276	    /* Directory not empty */
    #define	  ERR_WOULDBLOCK  	-277      /* Operation would block */
    #define	  ERR_BADSLT      	-278	    /* Invalid slot */
    #define	  ERR_NODATA      	-279	    /* No data available */
    #define   ERR_TIMEOUT   		-280      /* Timer expired */
    #define	  ERR_PROTO       	-281      /* Protocol error */
    #define	  ERR_BADFD       	-282      /* File descriptor in bad state */
    #define	  ERR_USERS       	-283      /* Too many users */
    #define	  ERR_NOBUFS      	-284      /* No buffer space available */
    #define   ERR_UNKNOWN       -501      /* Unknown error */
    #define   ERR_USER          -2048     /* User defined error code*/
#endif /*LINUX*/ 


#endif /*__ASSEMBLER__*/
#ifdef __cplusplus 
} 
#endif /*__cplusplus*/

#endif /*__ERROR_CODE_H*/
