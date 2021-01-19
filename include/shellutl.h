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

#ifndef _SHELLUTL_H
#define _SHELLUTL_H

void sh_curdrvdir(char *ppath);
void build_root_path(char *path,WORD drive);
char *sh_name(char *ppath);
char *sh_path(char *src, char *dest, char *pname);
void sh_envrn(char *environment, char **ppath, const char *psrch);
char *filename_start(char *path);
WORD wildcmp(const char *pwld, const char *ptst);
WORD get_drive(char *path);
void sh_curdir(char *ppath);
LONG readfile(char *filename, LONG count, char *buf);

WORD set_default_path(char *path);
BOOL shellutl_is_drive_valid(char drive, char *drvstr);
/* to add desksupp.c file_exists and many other things from there */

#endif
