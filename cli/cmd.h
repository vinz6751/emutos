/*
 * EmuCON2 header
 *
 * Copyright (C) 2013-2020 The EmuTOS development team
 *
 * Authors:
 *  RFB    Roger Burrows
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */
#ifndef STANDALONE_CONSOLE
 #include "config.h"
 #include "sysconf.h"
 #include "nls.h"
#else
 /* config.h */
 #define CONF_ATARI_HARDWARE    1
 #define CONF_WITH_TT_SHIFTER   1
 #define MAXPATHLEN      256
 #define BLKDEVNUM       26
 /* nls.h */
 #define _(a) a
 #define N_(a) a
 #define gettext(a) a
#endif

#include "portab.h"
#include "gemdos.h"
#include "xbiosbind.h"
#include "biosbind.h"

extern char *environment; /* environment string, from cmdAsm.s */


/*
 * program parameters
 */
#define CMDLINELEN      128     /* allow for length char etc */
#define MAXCMDLINE      125     /* the most amount of real data allowed */

#define IOBUFSIZE       16384L  /* buffer size */

#define MAX_LINE_SIZE   200L    /* must be greater than the largest screen width */
#define HISTORY_SIZE    10      /* number of lines of history */
#define MAX_ARGS        30      /* maximum number of args we can parse */

#define LOCAL           static  /* comment out for testing */
#define PRIVATE         static  /* comment out for testing */

/*
 * date/time display format stuff
 */
#define _IDT_COOKIE     0x5f494454      /* '_IDT' */
#define _IDT_MDY        0               /* date format: month-day-year */
#define _IDT_DMY        1               /*              day-month-year */
#define _IDT_YMD        2               /*              year-month-day */
#define _IDT_YDM        3               /*              year-day-month */
#define _IDT_12H        0               /* time format: 12-hour */
#define _IDT_24H        1               /*              24-hour */

#define DEFAULT_DT_SEPARATOR    '/'
#define DEFAULT_DT_FORMAT   ((_IDT_12H<<12) + (_IDT_YMD<<8) + DEFAULT_DT_SEPARATOR)

/*
 * video stuff
 */
#define _VDO_COOKIE     0x5f56444fL     /* '_VDO' */
#define _VDO_ST         0x00000000L     /* ST */
#define _VDO_TT         0x00020000L     /* TT */
#define _VDO_VIDEL      0x00030000L     /* Falcon videl */
#define ST_LOW          0               /* from Getrez() */
#define ST_MEDIUM       1
#define ST_HIGH         2
#define TT_MEDIUM       4
#define TT_HIGH         6
#define TT_LOW          7

/*
 *  typedefs
 */

/* Type of function run by execute() */
typedef LONG FUNC(WORD argc,char **argv);

/*
 *  return codes from get_next_arg()
 */
#define ARG_NORMAL      1
#define NO_MORE_ARGS    0
#define QUOTING_ERROR   -1

/*
 *  manifest constants
 */
#define EFILNF          -33
#define EPTHNF          -34
#define ENHNDL          -35
#define EACCDN          -36
#define ENSMEM          -39
#define EDRIVE          -46
#define ENMFIL          -49
                                /* additional emucon-only error codes */
#define USER_BREAK      -100        /* user interrupted long output */
#define NOT_DIRECTORY   -101        /* path points to a file */
#define DISK_FULL       -102
#define CMDLINE_LENGTH  -103
#define DIR_NOT_EMPTY   -104        /* translated from EACCDN for folders */
#define CANT_DELETE     -105        /* translated from EACCDN for files */
#define CHANGE_RES      -125        /* returned by mode command */
#define INVALID_PARAM   -126        /* for builtin commands */
#define WRONG_NUM_ARGS  -127        /* for builtin commands */

#define ESC             0x1b
#define DBLQUOTE        0x22

#define CTL_C           ('C'-0x40)
#define CTL_Q           ('Q'-0x40)
#define CTL_S           ('S'-0x40)

#define blank_line()    escape('l')
#define clear_screen()  escape('E')
#define cursor_left()   escape('D')
#define cursor_right()  escape('C')
#define enable_cursor() escape('e')
#define conin()         Bconin(2)
#define constat()       Bconstat(2)
#define conout(c)       Bconout(2,(unsigned char)(c))

#define LOOKUP_EXIT     (FUNC *)-1L     /* special return values from lookup_builtin() */
#define LOOKUP_ARGS     (FUNC *)-2L

/*
 *  global variables
 */
extern LONG idt_value;
extern UWORD screen_cols, screen_rows;
extern WORD current_res, requested_res;
extern WORD linewrap;
extern WORD nflops_copy;
extern DTA *dta;
extern LONG redir_handle;
extern char user_path[MAXPATHLEN];     /* from PATH command */

/*
 *  function prototypes
 */
/* cmdmain.c */
int valid_res(WORD res);

/* cmdedit.c */
WORD init_cmdedit(void);
void init_screen(void);
void insert_char(char *line,WORD pos,WORD len,char c);
WORD read_line(char *line);
void save_history(const char *line);

/* cmdexec.c */
LONG exec_program(WORD argc,char **argv,char *redir_name);

/* cmdint.c */
LONG (*lookup_builtin(WORD argc,char **argv))(WORD,char **);

/* cmdparse.c */
WORD parse_line(char *line,char **argv,char *redir_name);

/* cmdutil.c */
WORD decode_date_time(char *s,UWORD date,UWORD time);
void errmsg(LONG rc);
void escape(char c);
WORD getcookie(LONG cookie,LONG *pvalue);
WORD getword(char *buf);
WORD get_path_component(const char **pp,char *dest);
WORD has_wildcard(const char *name);
void message(const char *msg);
void messagenl(const char *msg);
const char *program_extension(const DTA *dta);
WORD strequal(const char *s1,const char *s2);
char *strlower(char *str);
char *strupper(char *str);

/* cmdasm.S */
ULONG getwh(void);
WORD getht(void);

/* cmdenv.c */
char *cmdenv_getenv(char *varname);
