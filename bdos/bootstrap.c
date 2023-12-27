/*
 * bootstrap.c - Startup the GEMDOS
 *
 * Copyright (C) 2001 Lineo, Inc.
 *               2002-2021 The EmuTOS development team
 *
 * Authors:
 *  EWF  Eric W. Fleischman
 *  JSL  Jason S. Loveman
 *  SCC  Steven C. Cavender
 *  LTG  Louis T. Garavaglia
 *  KTB  Karl T. Braun (kral)
 *  ACH  Anthony C. Hay (DR UK)
 *  MAD  Martin Doering
 *  THH  Thomas Huth
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "emutos.h"
#include "fs.h"
#include "mem.h"
#include "console.h"
#include "time.h"
#include "string.h"
#include "bdosstub.h"
#include "biosbind.h"   /* for Kbshift() */
#include "biosext.h"
#include "bdosbind.h"   /* it's ok to use the BDOS from the bootstraper */
#include "../bios/blkdev.h"
#include "../bios/initinfo.h"
#if WITH_CLI
#include "../cli/clistub.h"
#endif

#define ENABLE_KDEBUG

#define ENABLE_RESET_RESIDENT 0 /* enable to run "reset-resident" code (see below) */

/* Environment stuff */
#define ENV_SIZE    12              /* sufficient for standard PATH=^X:\^^ (^=nul byte) */
#define DEF_PATH    "A:\\"          /* default value for path */
static char default_env[ENV_SIZE];  /* default environment area */

/* Initial process setup */
static const char double_nul[2] __attribute__ ((aligned (2))) = { 0, 0 }; /* env string for initial process */
static PD initial_basepage;     /* basepage of initial process (statically allocated since never freed) */

static void install(void);
static void startup(void);
static void init_default_environment(void);
static void init_memory(void);
static void autoexec(void);
static void apply_boot_settings(ULONG shiftbits);

void bdos_install_traps(void); /* in bdosmain.c */

/*
 * Setup the GEMDOS then execute the os (_exec_os).
 */
void bdos_bootstrap(void)
{
    install();
    startup();
}

/*
 * Install the BDOS (a.k.a GEMDOS)
 */
static void install(void)
{
    bdos_install_traps();

    /* Setup memory and management of it */
    bufl_init();
    init_memory();

    /* Set up initial process. Required by Malloc() */
    run = &initial_basepage;
    run->p_flags = PF_STANDARD;
    run->p_env = CONST_CAST(char *,double_nul);
    KDEBUG(("BDOS: address of basepage = %p\n", run));

    time_init();

    stdhdl_init();  /* set up system initial standard handles */

    KDEBUG(("BDOS: install successful ...\n"));
}

/*
 * Register memory detected by the BIOS to the BDOS pool
 */
static void init_memory(void)
{
    /* Init memory */
    osmem_init();
    umem_init();

/* We can have this conditional check to save a few bytes as effectively 
 * we only take action on blocks of type ALT */
#if CONF_WITH_ALT_RAM 
    {
        struct memory_block_t *mem;
    
        for (mem = bget_memory_info(); mem != NULL; mem = mem->next)
        {
            xmaddalt((UBYTE*)mem->start, mem->size);
        }
    }
#endif
}

/*
 * Collect user boot settings and act accordingly by starting processes.
 */
static void startup(void)
{
    BOOL  show_initinfo;         /* TRUE if welcome screen must be displayed */
    ULONG shiftbits;
    BOOL  first_boot;

    KDEBUG(("drvbits = %08lx\n",drvbits));

#if CONF_ATARI_HARDWARE
    /* Steem needs this to initialize its GEMDOS hard disk emulation.
     * This may change drvbits. See Steem sources:
     * File steem/code/emulator.cpp, function intercept_bios(). */
    Drvmap();
#endif

    /* If it's not the first boot, we use the existing bootdev.
     * this allows a boot device that was selected via the welcome
     * screen to persist across warm boots. */
    first_boot = is_first_boot();
    if (first_boot)
        bootdev = is_drive_available(DEFAULT_BOOTDEV) ? DEFAULT_BOOTDEV : FLOPPY_BOOTDEV;

    /* Get boot preferences */
#if INITINFO_DURATION == 0
    show_initinfo = FALSE;
#elif ALWAYS_SHOW_INITINFO
    show_initinfo = TRUE;
#else
    show_initinfo = first_boot;
#endif
    if (show_initinfo)
        bootdev = initinfo(&shiftbits); /* Show the welcome screen */
    else
        shiftbits = Kbshift(-1);

    /* Update bootdev/bootflags according to what's possible and user choices */
    apply_boot_settings(shiftbits);

    KDEBUG(("bootdev = %d\n", bootdev));
    KDEBUG(("bootflags = 0x%02x\n", bootflags));

    /* If the user decided to skip AUTO programs, we don't attempt to execute the bootsector */
    if (bootflags & ~BOOTFLAG_SKIP_AUTO_ACC)
    {
#ifdef DISABLE_HD_BOOT
        if (bootdev < NUMFLOPPIES) /* don't attempt to boot from hard disk */
#endif
            /* Execute the bootsector code (if present) */
            (*hdv_boot)();
    }

    Dsetdrv(bootdev);           /* Set boot drive as current */

    init_default_environment(); /* Build default environment string */

    /* Normal EmuTOS would run run_reset_resident() here, but I think that's a BIOS feature
     * so in better to have that done in the BIOS startup so it's moved there. */

#if WITH_CLI
    if (bootflags & BOOTFLAG_EARLY_CLI)
    {
        KDEBUG(("Launching CLI"));
        /* Run an early console, passing the default environment */
        PD *pd = (PD *) Pexec(PE_BASEPAGEFLAGS, (char *)PF_STANDARD, "", default_env);
        pd->p_tbase = (UBYTE *) coma_start;
        pd->p_tlen = pd->p_dlen = pd->p_blen = 0;
        Pexec(PE_GOTHENFREE, "", (char *)pd, default_env);
    }
#endif

    /* Run programs from the AUTO folder */
    autoexec();

    /* Give control to next process to run */
    if (cmdload != 0)
    {
        /* Run COMMAND.PRG with an empty environment (like Atari TOS) */
        Pexec(PE_LOADGO, "COMMAND.PRG", "", NULL);
    } 
    else if (exec_os) {
        KDEBUG(("Launching OS from %p", exec_os));
        /* Start the default (ROM) shell with the default environment (like Atari TOS) */
        PD *pd;
        pd = (PD *) Pexec(PE_BASEPAGEFLAGS, (char *)PF_STANDARD, "", default_env);
        pd->p_tbase = (UBYTE *) exec_os;
        pd->p_tlen = pd->p_dlen = pd->p_blen = 0;
        Pexec(PE_GO, "", (char *)pd, default_env);
    }
}


/*
 * Build the default environment string: "PATH=^X:\^^" [where ^=nul]
 */
static void init_default_environment(void)
{
    char *p;

    strcpy(default_env,PATH_ENV);
    p = default_env + sizeof(PATH_ENV); /* point to first byte of path string */
    strcpy(p,DEF_PATH);
    *p += bootdev;                      /* fix up drive letter */
    p += sizeof(DEF_PATH);
    *p = '\0';                          /* terminate with double nul */
}

/*
 * boot_from_block_device - boot from device in 'bootdev'
 */
static void apply_boot_settings(ULONG shiftbits)
{
    if (shiftbits & MODE_ALT)
        bootflags |= BOOTFLAG_SKIP_HDD_BOOT;

    if (shiftbits & MODE_CTRL)
        bootflags |= BOOTFLAG_SKIP_AUTO_ACC;

    /* If the user decided to skip hard drive boot, we set the boot device to floppy A: */
    if (bootflags & BOOTFLAG_SKIP_HDD_BOOT)
        bootdev = FLOPPY_BOOTDEV;
}

/*
 * autoexec - run programs in auto folder
 *
 * Skip this if user holds the Control key down.
 *
 * Note that GEMDOS already created a default basepage so it is safe
 * to use GEMDOS calls here!
 */

static void run_auto_program(const char* filename)
{
    char path[30];

    strcpy(path, "\\AUTO\\");
    strcat(path, filename);

    KDEBUG(("Loading %s ...\n", path));
    Pexec(PE_LOADGO, path, "", NULL);
    KDEBUG(("[OK]\n"));
}

static void autoexec(void)
{
    DTA dta;
    WORD err;

    /* check if the user does not want to run AUTO programs */
    if (bootflags & BOOTFLAG_SKIP_AUTO_ACC)
        return;

#if DETECT_NATIVE_FEATURES
    natfeat_bootstrap(default_env);           /* try to boot the new OS kernel directly */
#endif

    if(!is_drive_available(bootdev))          /* check, if bootdev available */
        return;

    Fsetdta(&dta);
    err = Fsfirst("\\AUTO\\*.PRG", 7);
    while(err == 0)
    {
#ifdef TARGET_PRG
        if (!strncmp(dta.d_fname, "EMUTOS", 6))
            KDEBUG(("Skipping %s from AUTO folder\n", dta.d_fname));
        else
#endif
        {
            run_auto_program(dta.d_fname);

            /* Setdta. BetaDOS corrupted the AUTO load if the Setdta
             * not repeated here */
            Fsetdta(&dta);
        }

        err = Fsnext();
    }
}


#if DETECT_NATIVE_FEATURES

static void bootstrap(void)
{
    /* start the kernel provided by the emulator */
    PD *pd;
    LONG length;
    LONG r;
    char args[128];

    args[0] = '\0';
    nf_getbootstrap_args(args, sizeof(args));

    /* allocate space */
    pd = (PD *) Pexec(PE_BASEPAGEFLAGS, (char*)PF_STANDARD, args, default_env);

    /* get the TOS executable from the emulator */
    length = nf_bootstrap(pd->p_lowtpa + sizeof(PD), pd->p_hitpa - pd->p_lowtpa);

    /* free the allocated space if something is wrong */
    if (length <= 0)
        goto err;

    /* relocate the loaded executable */
    r = Pexec(PE_RELOCATE, (char *)length, (char *)pd, default_env);
    if (r != (LONG)pd)
        goto err;

    /* set the boot drive for the new OS to use */
    bootdev = nf_getbootdrive();

    /* execute the relocated process */
    Pexec(PE_GO, "", (char *)pd, default_env);

err:
    Mfree(pd->p_env); /* Mfree() the environment */
    Mfree(pd); /* Mfree() the process area */
}

#endif /* DETECT_NATIVE_FEATURES */