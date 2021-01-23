/*
 * shellutl.c - shell functions
 *
 * Copyright (C) 2002-2021 The EmuTOS development team
 *
 * Authors:
 *  VB      Vincent Barrilliot
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
/* This is a "shell library" which collects various methods
 * related to shell operations, so these can be used by both the 
 * AES, the desktop and the CLI EmuCON
 */
#ifndef _SHELLUTL_H
#define _SHELLUTL_H


WORD shellutl_get_drive(const char *path);
BOOL shellutl_is_drive_valid(char drive);

LONG shellutl_get_current_path_for_drive(char *buf, WORD drive);

char *shellutl_get_filename_part(char *path);

char *shellutl_get_path_components(char *paths, const char *filename_to_append, char *opath);
WORD  shellutl_get_path_component(const char **pp,char *dest);

char *shellutl_filename_start(const char *path);
void  shellutl_build_root_path(char *path,WORD drive);
WORD  shellutl_get_current_drive_root(char *opath);

void  shellutl_getenv(char *environment, char **ppath, const char *psrch);
WORD  wildcmp(const char *pwld, const char *ptst);
LONG  readfile(char *filename, LONG count, char *buf);

WORD  set_default_path(char *path);

/* to add desksupp.c file_exists and many other things from there */

#endif
