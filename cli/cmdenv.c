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

extern char *environment;       /* from cmdasm.S */


PRIVATE int find_separator_position(char *var);


char *cmdenv_getenv(char *varname) {
	char *c = environment;
	BOOL  notdone;
	int   namelen;
	int   separator;
	char *n;

	/* GCC compiles this to something shorter than strlen */
	n = varname;
	while (*n++);
  	namelen = n - varname;
  
  	if (--namelen == 0)
  		return NULL;

	notdone = c && *c;
	while (notdone) {

		/* separator is the position of "=" between name and value */
		separator = find_separator_position(c);
		if (--separator != namelen)
			continue;

		if (strncasecmp(varname,c,namelen) == 0)
			return &(c[separator+1]);	
	
		/* Find next variable */
		while (*c++);
		
		/* Two consecutives NULL mean end of environment */
		if (!*c)
			notdone = FALSE;
	}

	return NULL;
}


PRIVATE inline int find_separator_position(char *var) {
	char *start = var;

	while (*var && *var++ != '=');

	return var - start;
}
