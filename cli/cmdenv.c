/*
 * EmuCON2: a command processor for EmuTOS
 *
 * Copyright (C) 2013-2020 The EmuTOS development team
 *
 * Authors:
 *  VB    Vincent Barrilliot
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
/*
 * This allows to read environment variables in a very minimal way.
 * For the standalone version, the environment is the one given by the
 * parent process.
 * For the version builtin EmuTOS, the environment is taken from the
 * "the_env" TOS variable at 0x4be, so an AUTO program can set it.
 */

#include "cmd.h"
#include "string.h"

#define the_env	(char**)0x4be /* TOS variable  */
extern char *environment;       /* from cmdasm.S */

/* To pass variable name to getenv */
PRIVATE char *varname;

PRIVATE char *getenv(void);
PRIVATE int find_separator_position(char *var);

char *cmdenv_getenv(char *name) {
	varname = name;
#ifdef STANDALONE_CONSOLE
	return getenv();
#else
	return (char*)Supexec(getenv); /* Reading "the_env" requires supervisor mode */
#endif
}

PRIVATE char *getenv(void) {
#ifdef STANDALONE_CONSOLE
	char *c = environment;
#else
	char *c = *the_env;
#endif
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
