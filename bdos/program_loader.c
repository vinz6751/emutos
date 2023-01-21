// #define ENABLE_KDEBUG

#include "mem.h"
#include "program_loader.h"
#include "string.h"
#include "sysconf.h"
#include "kprint.h"


extern const PROGRAM_LOADER prg_program_loader;
#if CONF_WITH_NON_RELOCATABLE_SUPPORT
extern const PROGRAM_LOADER pgz_program_loader;
#endif


static const PROGRAM_LOADER * const program_loaders[] = {
    &prg_program_loader
#if CONF_WITH_NON_RELOCATABLE_SUPPORT
    , &pgz_program_loader
#endif
};


void find_program_loader(LOAD_STATE* lstate) {
    int i;
    int r;

    lstate->loader = NULL;

    for (i = 0; i < sizeof(program_loaders)/sizeof(PROGRAM_LOADER*); i++) {
        PROGRAM_LOADER* loader = program_loaders[i];
        r = loader->can_load(lstate);
        if (r == 1) {
            lstate->loader = loader;
            return;
        }
        else if (r == 0)
            /* Reset file to start position for next loader */
            r = xlseek(r, lstate->fh, 0/*SEEK_CUR*/);
        
        if (r  < 0) {
            KDEBUG(("find_program_loader: error while reading %d\n", r));
            // GEMDOS error while reading or seeking, so abort.
            break;
        }
    }
}
