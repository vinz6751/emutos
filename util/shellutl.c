/*
 * shellutl.c - shell functions
 *
 * Copyright (C) 2002-2022 The EmuTOS development team
 *
 * Authors:
 *  LVL     Laurent Vogel
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

/*
 * Note: compiling EmuTOS with option -lc does not work. Because of
 * this, replacements for common string routines are provided here.
 */

#include "emutos.h"
#include "gemdos.h"
#include "shellutl.h"
#include "sysconf.h"

#include <string.h>

void sh_curdrvdir(char *ppath)
{
	WORD drive;

	/* remember current directory */
	drive = dos_gdrv();
	*ppath++ = drive + 'A';
	*ppath++ = ':';
	*ppath = '\0';
	dos_gdir(drive+1, ppath);
	if (*ppath == '\0')
    {
		*ppath++ = '\\';
		*ppath = '\0';
    }
}


/*
 *  Return a pointer to the start of the filename in a path
 *  (assumed to be the last component of the path)
 */
char *sh_name(char *ppath)
{
	char *pname = ppath;

	/*
	 * note: filename_start() assumes that there is a filename separator
	 * within the path, so we handle a path like X:AAAAAAAA.BBB before
	 * calling the general function
	 */
	if (ppath[0] && (ppath[1] == ':'))
		pname += 2;

	return filename_start(pname);
}


/*
 *  Search for a particular string in the DOS environment and return a
 *  value in the pointer pointed to by the first argument.  If the string
 *  is found, the value is a pointer to the first character after the
 *  string; otherwise it is a NULL pointer.
 */
void sh_envrn(char *environment, char **ppath, const char *psrch)
{
    char *p;
    WORD len;

    len = strlen(psrch);
    *ppath = NULL;

    /*
     * scan environment string until double nul
     */
    for (p = environment; *p; )
    {
        if (strncmp(p, psrch, len) == 0)
        {
            *ppath = p + len;
            break;
        }
        while(*p++) /* skip to end of current env variable */
            ;
    }
}


/*
 *  Search next style routine to pick up each path in the PATH= portion
 *  of the DOS environment.  It returns a pointer to the start of the
 *  following path until there are no more paths to find.
 */
char *sh_path(char *src, char *dest, char *pname)
{
    char last = 0;
    char *p;

    if (!src)           /* precautionary */
        return NULL;

    /* check for end of PATH= env var */
    if (!*src)
        return NULL;

    /* copy over path */
    for (p = src; *p; )
    {
        if ((*p == ';') || (*p == ','))
            break;
        last = *p;
        *dest++ = *p++;
    }

    /* see if extra slash is needed */
    if ((last != '\\') && (last != ':'))
        *dest++ = '\\';

    /* append file name */
    strcpy(dest, pname);

    /* point past terminating separator or nul */
    return p + 1;
}


/*
 * return pointer to start of last segment of path
 * (assumed to be the filename)
 */
char *filename_start(char *path)
{
    char *start = path;

    while (*path)
        if (*path++ == '\\')
            start = path;

    return start;
}


/*
 *  Routine to see if the test filename matches a standard TOS
 *  wildcard string.  For example:
 *      pattern = "*.BAT"
 *      filename = "MYFILE.BAT"
 */
WORD wildcmp(const char *pattern,const char *filename)
{
WORD i;

    /*
     * we process the name on pass1 and the extension on pass2
     */
    for (i = 0; i < 2; i++)
    {
        for ( ; *filename && (*filename != '.'); filename++)
        {
            if (*pattern == '*')
                continue;
            if ((*pattern == '?') || (*pattern == *filename))
            {
                pattern++;
                continue;
            }
            return FALSE;
        }

        /* soak up any remaining wildcard pattern characters */
        while((*pattern == '*') || (*pattern== '?'))
            pattern++;

        /* filename is nul or '.', pattern is a real character or '.' */
        if (*pattern == '.')
            pattern++;
        if (*filename == '.')
            filename++;
    }

    return (*pattern == *filename);
}


/*
 *  get drive number from specified path (if possible) or default
 */
WORD get_drive(char *path)
{
    char c;

    if (path[1] == ':')     /* drive letter is present */
    {
        c = toupper(path[0]);
        if ((c >= 'A') && (c <= 'Z'))
            return c - 'A';
    }

    return dos_gdrv();
}


/*
 *  Set default desktop path (root of current drive)
 */
void sh_curdir(char *ppath)
{
    *ppath++ = dos_gdrv() + 'A';
    *ppath++ = ':';
    *ppath++ = '\\';
    *ppath = '\0';
}

/*
 *  Build root path for specified drive
 */
void build_root_path(char *path,WORD drive)
{
    char *p = path;

    *p++ = drive;
    *p++= ':';
    *p++ = '\\';
    *p = '\0';
}


/*
 *  Routine to read in the start of a file where the filename
 *  is an absolute one (X:\..)
 *
 *  returns: >=0  number of bytes read
 *           < 0  error code from dos_open()/dos_read()
 */
LONG readfile(char *filename, LONG count, char *buf)
{
    char tmpstr[MAXPATHLEN]; /* VB: this was MAX_LEN in geminit.c */

    strcpy(tmpstr, filename);
    tmpstr[0] += dos_gdrv();            /* set the drive letter */

    return dos_load_file(tmpstr, count, buf);
}


/*
 *  Routine to change the default drive and directory
 */
WORD set_default_path(char *path)
{
    dos_sdrv(path[0]-'A');

    return (WORD)dos_chdir(path);
}


/*
 *  Check if specified drive letter is valid
 *
 *  if it is, return TRUE
 *  else issue form_alert and return FALSE
 *  drvstr is a buffer, should be at least 2 characters
 */
BOOL shellutl_is_drive_valid(char drive, char *drvstr)
{
    int drv = drive - 'A';

    drvstr[0] = drive;
    drvstr[1] = '\0';

    if ((drv >= 0) && (drv < BLKDEVNUM))
        if (dos_sdrv(dos_gdrv()) & (1L << drv))
            return TRUE;

    return FALSE;
}
