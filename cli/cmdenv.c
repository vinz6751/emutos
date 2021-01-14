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
 * This manage the environment variables in a basic way.
 */

#include "cmd.h"
#include "string.h"

#define the_env	(char**)0x4be /* TOS variable  */
extern char *environment;       /* from cmdasm.S */

/* To pass info from the function called by Supexec */
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


/* CAVEAT: We don't expect the = to be missing. Can be pretty bad if you 
 * run the console at start of EMUTOS after AUTO programs set wrong env... */
PRIVATE inline int find_separator_position(char *var) {
	char *start = var;

	while (*var && *var++ != '=');

	return var - start;
}
