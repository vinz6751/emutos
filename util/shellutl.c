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
#include "bdosbind.h"

#include <string.h>


/*
 *  Get specified drive's current path (including drive letter) into buffer
 *
 *  Note: drive is specified as for Dgetpath(): for current drive, use 0
 *  otherwise use the drive number + 1
 *
 *  returns error code from Dgetpath()
 */
LONG shellutl_get_current_path_for_drive(char *buf, WORD drive)
{
	LONG rc;
	char *p = buf;

    *p++ = 'A' + (drive ? drive-1 : Dgetdrv());
    *p++ = ':';
    *p = '\0';

    rc = Dgetpath(p,drive);
    if (!*p) {          /* the root */
        *p++ = '\\';
        *p = '\0';
    }

    return rc;
}


/*
 *  Return a pointer to the start of the filename in a path
 *  (assumed to be the last component of the path)
 */
char *shellutl_get_filename_part(char *path)
{
	char *name = path;

	/*
	 * note: filename_start() assumes that there is a filename separator
	 * within the path, so we handle a path like X:AAAAAAAA.BBB before
	 * calling the general function
	 */
	if (path[0] && (path[1] == ':'))
		name += 2;

	return shellutl_filename_start(name);
}


/*
 *  "Search next"-style routine to pick up each path in the PATH= portion
 *  of the environment. These can be delimited by "," or ";".
 *  For each path, it returns a pointer to the start of the
 *  following path and appends the given filename, until there are no more
 *  paths to find.
 *  paths is list of paths, e.g C:BIN;D:\USR\BIN
 *  opath is the returned path C:\BIN\TEST.TXT, then D:\USR\BIN\TEST.TXT
 *  filename_to_append, in this example, is TEST.TXT
 */
char *shellutl_get_path_components(char *paths, const char *filename_to_append, char *opath)
{
    char last = 0;
    char *p;

    if (!paths)           /* precautionary */
        return NULL;

    /* check for end of PATH= env var */
    if (!*paths)
        return NULL;

    /* copy over path */
    for (p = paths; *p; )
    {
        if ((*p == ';') || (*p == ','))
            break;
        last = *p;
        *opath++ = *p++;
    }

    /* see if extra slash is needed */
    if ((last != '\\') && (last != ':'))
        *opath++ = '\\';

    /* append file name */
    strcpy(opath, filename_to_append);

    /* point past terminating separator or nul */
    return p + 1;
}


/*
 *  copies next path component from buffer &
 *  updates buffer pointer
 *
 *  returns:
 *      1   arg is normal
 *      0   no more args
 */
WORD shellutl_get_path_component(const char **pp,char *dest)
{
const char *p;
char *q = dest;

    /*
     *  look for start of next component
     */
    for (p = *pp; *p; p++)
        if (*p != ';')
            break;
    if (!*p) {          /* end of buffer */
        *pp = p;
        return 0;
    }

    while(*p) {
        if (*p == ';' || *p ==',')
            break;
        *q++ = *p++;
    }
    *q = '\0';

    *pp = p;
    return 1;
}


/*
 * return pointer to start of last segment of path
 * (assumed to be the filename)
 */
char *shellutl_filename_start(const char *path)
{
    char *start = (char*)path;

    while (*path)
        if (*path++ == '\\')
            start = (char*)path;

    return start;
}


/*
 *  get drive number from specified path (if possible) or default
 */
WORD shellutl_get_drive(const char *path)
{
    char c;

    if (path[1] == ':')     /* drive letter is present */
    {
        c = toupper(path[0]);
        if ((c >= 'A') && (c <= 'Z'))
            return c - 'A';
    }

    return Dgetdrv();
}


/*
 *  Get root folder of current drive
 */
WORD shellutl_get_current_drive_root(char *opath)
{
	WORD drive;

	drive = Dgetdrv();
	shellutl_build_root_path(opath,drive);

	return drive;
}

/*
 *  Build root path for specified drive
 */
void shellutl_build_root_path(char *opath, WORD drive)
{
    char *p = opath;

    *p++ = drive;
    *p++= ':';
    *p++ = '\\';
    *p = '\0';
}


/*
 *  Search for a particular string in the DOS environment and return a
 *  value in the pointer pointed to by the first argument.  If the string
 *  is found, the value is a pointer to the first character after the
 *  string; otherwise it is a NULL pointer.
 */
void shellutl_getenv(char *environment, char **ppath, const char *psrch)
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
    tmpstr[0] += Dgetdrv();            /* set the drive letter */

    return dos_load_file(tmpstr, count, buf);
}


/*
 *  Routine to change the default drive and directory
 */
WORD set_default_path(char *path)
{
    Dsetdrv(path[0]-'A');

    return (WORD)dos_chdir(path);
}


/*
 *  Check if specified drive letter is valid
 */
BOOL shellutl_is_drive_valid(char drive)
{
    int drv = (drive | 0x20) - 'a'; /* Force lower case */

    if ((drv >= 0) && (drv < BLKDEVNUM))
        if (Dsetdrv(Dgetdrv()) & (1L << drv))
            return TRUE;

    return FALSE;
}
