/*
 * EmuCON2: a command processor for EmuTOS
 *
 * Copyright (C) 2013-2021 The EmuTOS development team
 *
 * Authors:
 *  VB    Vincent Barrilliot
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
/*
 * This allows to read environment variables in a very minimal way.
 * The environment is provided by the parent process.
 */

#include "cmd.h"
#include "string.h"

extern char *environment; /* from cmdasm.S */


PRIVATE int find_separator_position(char *var);


char *cmdenv_getenv(char *varname) {
	char *c;
	int   namelen;
	int   separator;
	char *n;

	/* GCC compiles this to something shorter than strlen */
	n = varname;
	while (*n++);
	namelen = n - varname -1;

	if ((c = environment) == NULL || namelen == 0)
		return NULL;

	while (*c) {

		/* separator is the position of "=" between name and value */
		separator = find_separator_position(c);
		if (separator && separator == namelen && !strncasecmp(varname,c,namelen))
			return &(c[separator+1]);

		/* Find next variable */
		while (*c++);

		/* Two consecutives NULL mean end of environment string */
	}

	return NULL;
}


PRIVATE inline int find_separator_position(char *var) {
	char *start = var;

	while (*var) {
		if (*var++ == '=')
			return var - start - 1;
	}

	/* We didn't find any "=" sign. That's a quirk. We don't handle that
	 * so we return 0 to signal an error. */
	return 0;
}
