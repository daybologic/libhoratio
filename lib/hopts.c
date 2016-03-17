/*
Horatio's Memory Manager
Copyright (c) 2000-2014, David Duncan Ross Palmer (M6KVM), Daybo Logic
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    * Neither the name of the Daybo Logic nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

/*! \file hopts.c
 * \brief Horatio options handling.
 *
 * This module attempts to replace options which were formerly provided
 * by the configure script.
 *
 * This should discourage many static builds.
 *
 * When I created the library in 2000, I made writing to a log file
 * dependent on a compile time configuration option.  However, priorities
 * have shifted in the years after this, and packaging systems are more
 * mature.  Different projects on the same system may or may not want
 * logging... so it's probably better that they specify at run time,
 * and we disable the code only under rare circumstances where the calls
 * themselves aren't available, eg. embedded systems,
 *
 * Also, recompiling the library for a release version of your library
 * isn't always convenient.
 */

#define HORATIO_SOURCE

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /*HAVE_CONFIG_H*/

#include <stddef.h>
#include <stdio.h>

#ifdef HORATIO_HDRSTOP
# pragma hdrstop
#endif /*HORATIO_HDRSTOP*/

#include "hbuild.h" /* General build parameters */
#include "horatio.h" /* Public library header */
#include "hintdata.h" /* Internal library data */

PS_HORATIO_OPTIONS horatio_options(
	PS_HORATIO_OPTIONS pOptions
) {
	if (pOptions && _options.len == pOptions->len) {

		_options.enableLog = pOptions->enableLog;
		_options.errorHandle = pOptions->errorHandle;
	}

	return &_options;
}

/*! \brief horatio_int_options_init.
 *
 *  Initializes an options structure.
 *  If no pointer to an options structure is supplied, the internal
 *  options themselves are initialized.
 *
 *  Returns:
 *  Pointer to modified options structure
 *
 *  Parameters:
 *  pOptions: Pointer to S_HORATIO_OPTIONS structure, or NULL
 */
PS_HORATIO_OPTIONS horatio_options_init(PS_HORATIO_OPTIONS pOptions) {

	if (!pOptions) {
		pOptions = &horatio_int__options;
	}

	pOptions->len = sizeof(S_HORATIO_OPTIONS);
	pOptions->enableLog = 0;
	pOptions->errorHandle = stderr;

	return pOptions;
}
