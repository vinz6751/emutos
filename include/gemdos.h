/*
 * EmuTOS interface to GEMDOS
 *
 * Copyright (C) 2002-2019 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef GEMDOS_H
#define GEMDOS_H

#include "bdosbind.h"

#if 0
/*
 * system calls
 */
extern LONG jmp_gemdos(WORD, ...);
extern LONG jmp_bios(WORD, ...);
extern LONG jmp_xbios(WORD, ...);

#define jmp_gemdos_v(a)         jmp_gemdos((WORD)(a))
#define jmp_gemdos_w(a,b)       jmp_gemdos((WORD)(a),(WORD)(b))
#define jmp_gemdos_l(a,b)       jmp_gemdos((WORD)(a),(LONG)(b))
#define jmp_gemdos_p(a,b)       jmp_gemdos((WORD)(a),(void*)(b))
#define jmp_gemdos_ww(a,b,c)    jmp_gemdos((WORD)(a),(WORD)(b),(WORD)(c))
#define jmp_gemdos_pw(a,b,c)    jmp_gemdos((WORD)(a),(void *)(b),(WORD)(c))
#define jmp_gemdos_wlp(a,b,c,d) jmp_gemdos((WORD)(a),(WORD)(b),(LONG)(c),(void *)(d))
#define jmp_gemdos_wpp(a,b,c,d) jmp_gemdos((WORD)(a),(WORD)(b),(void *)(c),(void *)(d))
#define jmp_gemdos_pww(a,b,c,d) jmp_gemdos((WORD)(a),(void *)(b),(WORD)(c),(WORD)(d))
#define jmp_gemdos_wppp(a,b,c,d,e)  jmp_gemdos((WORD)(a),(WORD)(b),(void *)(c),(void *)(d),(void *)(e))

#define jmp_bios_w(a,b)         jmp_bios((WORD)(a),(WORD)(b))
#define jmp_bios_ww(a,b,c)      jmp_bios((WORD)(a),(WORD)(b),(WORD)(c))
#define jmp_xbios_v(a)          jmp_xbios((WORD)(a))
#define jmp_xbios_l(a,b)        jmp_xbios((WORD)(a),(LONG)(b))
#define jmp_xbios_llww(a,b,c,d,e)   jmp_xbios((WORD)a,(LONG)b,(LONG)c,(WORD)d,(WORD)e)
#define jmp_xbios_ww(a,b,c)     jmp_xbios((WORD)(a),(WORD)(b),(WORD)(c))

#define Dsetdrv(a)          jmp_gemdos_w(0x0e,a)
#define Dgetdrv()           jmp_gemdos_v(0x19)
#define Fgetdta()           jmp_gemdos_v(0x2f)
#define Sversion()          jmp_gemdos_v(0x30)
#define Dfree(a,b)          jmp_gemdos_pw(0x36,a,b)
#define Dcreate(a)          jmp_gemdos_p(0x39,a)
#define Ddelete(a)          jmp_gemdos_p(0x3a,a)
#define Dsetpath(a)         jmp_gemdos_p(0x3b,a)
#define Fcreate(a,b)        jmp_gemdos_pw(0x3c,a,b)
#define Fopen(a,b)          jmp_gemdos_pw(0x3d,a,b)
#define Fclose(a)           jmp_gemdos_w(0x3e,a)
#define Fread(a,b,c)        jmp_gemdos_wlp(0x3f,a,b,c)
#define Fwrite(a,b,c)       jmp_gemdos_wlp(0x40,a,b,c)
#define Fdelete(a)          jmp_gemdos_p(0x41,a)
#define Fattrib(a,b,c)      jmp_gemdos_pww(0x43,a,b,c)
#define Fdup(a)             jmp_gemdos_w(0x45,a)
#define Fforce(a,b)         jmp_gemdos_ww(0x46,a,b)
#define Dgetpath(a,b)       jmp_gemdos_pw(0x47,a,b)
#define Malloc(a)           jmp_gemdos_l(0x48,a)
#define Mfree(a)            jmp_gemdos_p(0x49,a)
#define Pexec(a,b,c,d)      jmp_gemdos_wppp(0x4b,a,b,c,d)
#define Fsfirst(a,b)        jmp_gemdos_pw(0x4e,a,b)
#define Fsnext()            jmp_gemdos_v(0x4f)
#define Frename(a,b,c)      jmp_gemdos_wpp(0x56,a,b,c)

#define Bconstat(a)         jmp_bios_w(0x01,a)
#define Bconin(a)           jmp_bios_w(0x02,a)
#define Bconout(a,b)        jmp_bios_ww(0x03,a,b)

#define Getrez()            jmp_xbios_v(0x04)
#define Setscreen(a,b,c,d)  jmp_xbios_llww(0x05,a,b,c,d)
#define Setcolor(a,b)       jmp_xbios_ww(0x07,a,b)
#define Cursconf(a,b)       jmp_xbios_ww(0x15,a,b)
#define Kbrate(a,b)         jmp_xbios_ww(0x23,a,b)
#define Supexec(a)          jmp_xbios_l(0x26,a)

#endif


WORD pgmld(WORD handle, char *pname, LONG **ldaddr);

LONG dos_rawcin(void);
void dos_conws(char *string);
WORD dos_conis(void);
void dos_sdta(void *ldta);
void *dos_gdta(void);
WORD dos_sfirst(char *pspec, WORD attr);
WORD dos_snext(void);
LONG dos_open(char *pname, WORD access);
WORD dos_close(WORD handle);
LONG dos_read(WORD handle, LONG cnt, void *pbuffer);
LONG dos_write(WORD handle, LONG cnt, void *pbuffer);
LONG dos_lseek(WORD handle, WORD smode, LONG sofst);
LONG dos_exec(WORD mode, const char *pcspec, const char *pcmdln, const char *segenv); /* see: gemstart.S */
LONG dos_chdir(char *pdrvpath);
WORD dos_gdir(WORD drive, char *pdrvpath);
LONG dos_sdrv(WORD newdrv);
LONG dos_create(char *name, WORD attr);
WORD dos_mkdir(char *path);
WORD dos_chmod(char *name, WORD wrt, WORD mod);
WORD dos_setdt(UWORD h, UWORD time, UWORD date);
WORD dos_label(char drive, char *plabel);
LONG dos_delete(char *name);
void dos_space(WORD drv, LONG *ptotal, LONG *pavail);
WORD dos_rename(char *p1, char *p2);
WORD dos_rmdir(char *path);
LONG dos_load_file(char *filename, LONG count, char *buf);

void *dos_alloc_stram(LONG nbytes);
void *dos_alloc_anyram(LONG nbytes);
LONG dos_avail_stram(void);
LONG dos_avail_altram(void);
LONG dos_avail_anyram(void);
WORD dos_free(void *maddr);
WORD dos_shrink(void *maddr, LONG length);

#endif
