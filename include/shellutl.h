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


WORD shellutl_get_drive(const char *path);
BOOL shellutl_is_drive_valid(char drive, char *drvstr);

void  shellutl_get_current_drive_and_dir(char *path);
char *shellutl_get_filename_part(char *path);
char *shellutl_get_path_components(char *paths, const char *filename_to_append, char *opath);
char *shellutl_filename_start(const char *path);
void  shellutl_build_root_path(char *path,WORD drive);
WORD shellutl_get_current_drive_root(char *opath);

void shellutl_getenv(char *environment, char **ppath, const char *psrch);
WORD wildcmp(const char *pwld, const char *ptst);
LONG readfile(char *filename, LONG count, char *buf);

WORD set_default_path(char *path);

/* to add desksupp.c file_exists and many other things from there */

#endif
