/*
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS HEADER.
 *
 * Copyright 2008 Sun Microsystems, Inc. All rights reserved.
 *
 * THE BSD LICENSE
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer. 
 * Redistributions in binary form must reproduce the above copyright notice, 
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution. 
 *
 * Neither the name of the  nor the names of its contributors may be
 * used to endorse or promote products derived from this software without 
 * specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PUBLIC_NSACL_NSERRDEF_H
#define PUBLIC_NSACL_NSERRDEF_H

/*
 * Type:        NSEFrame_t
 *
 * Description:
 *
 *	This type describes the structure of an error frame.  An error
 *	frame contains the following items:
 *
 *	ef_retcode	- This is a copy of the traditional error code,
 *			  as might be returned as a function value to
 *			  indicate an error.  The purpose of the error
 *			  code is to provide the caller of a function
 *			  with sufficient information to determine how
 *			  to process the error.  That is, it does not
 *			  need to identify a specific error, but only
 *			  has to distinguish between classes of errors
 *			  as needed by the caller to respond differently.
 *			  Usually this should be a small number of values.
 *
 *	ef_errorid	- This is an integer identifier which uniquely
 *			  identifies errors in a module or library.
 *			  That is, there should be only one place in
 *			  the source code of the module or library which
 *			  generates a particular error id.  The error id
 *			  is used to select an error message in an error
 *			  message file.
 *
 *	ef_program	- This is a pointer to a string which identifies
 *			  the module or library context of ef_errorid.
 *			  The string is used to construct the name of
 *			  the message file in which an error message for
 *			  ef_errorid can be found.
 *
 *	ef_errc		- This is the number of values stored in ef_errc[]
 *			  for the current error id.
 *
 *	ef_errv		- This is an array of strings which are relevant
 *			  to a particular error id.  These strings can
 *			  be included in an error message retrieved from
 *			  a message file.  The strings in a message file
 *			  can contain "%s" sprintf() format codes.  The
 *			  ef_errv[] strings are passed to sprintf() along
 *			  with the error message string.
 */

#define NSERRMAXARG	8	/* size of ef_errv[] */

typedef struct NSEFrame_s NSEFrame_t;
struct NSEFrame_s {
    NSEFrame_t * ef_next;	/* next error frame on NSErr_t list */
    long ef_retcode;		/* error return code */
    long ef_errorid;		/* error unique identifier */
    char * ef_program;		/* context for ef_errorid */
    int ef_errc;		/* number of strings in ef_errv[] */
    char * ef_errv[NSERRMAXARG];/* arguments for formatting error message */
};

/*
 * Description (NSErr_t)
 *
 *	This type describes the structure of a header for a list of
 *	error frames.  The header contains a pointer to the first
 *	and last error frames on the list.  The first error frame
 *	is normally the one most recently generated, which usually
 *	represents the highest-level interpretation available for an
 *	error that is propogating upward in a call chain.  These
 *	structures are generally allocated as automatic or static
 *	variables.
 */

typedef struct NSErr_s NSErr_t;
struct NSErr_s {
    NSEFrame_t * err_first;			/* first error frame */
    NSEFrame_t * err_last;			/* last error frame */
    NSEFrame_t *(*err_falloc)(NSErr_t * errp);	/* error frame allocator */
    void (*err_ffree)(NSErr_t * errp,
		      NSEFrame_t * efp);	/* error frame deallocator */
};

/* Define an initializer for an NSErr_t */
#define NSERRINIT	{ 0, 0, 0, 0 }

#ifndef INTNSACL

#define nserrDispose (*__nsacl_table->f_nserrDispose)
#define nserrFAlloc (*__nsacl_table->f_nserrFAlloc)
#define nserrFFree (*__nsacl_table->f_nserrFFree)
#define nserrGenerate (*__nsacl_table->f_nserrGenerate)

#endif /* !INTNSACL */

#endif /* !PUBLIC_NSACL_NSERRDEF_H */
