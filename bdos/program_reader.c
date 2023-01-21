/* While loading a program for execution, the content of the program is loaded
 * using a so-called PROGRAM_READER. This provides filesystem-like operations
 * for reading the file. We have two implementations:
 * - one that reads from files from disk using normal GEMDOS functions
 *   (thats' the general case when executing a program)
 * - one for reading the file out of the ROM.
 * The file in the ROM must be included as binary and described by a
 * ROM_PRG structure, and registered in the rom_prgs array.
 * 
 * This feature was introduced so to leverage system which have a comfortably
 * large ROM/flash space, so we can ship apps. This is also used for running
 * programs at boot, in the case of a whole self-contained single app ROM.
 */

// #define ENABLE_KDEBUG

#include <stdbool.h>
#include "fs.h"
#include "program_reader.h"
#include "gemerror.h"
#include "kprint.h"
#include "string.h"

// File program reader --------------------------------------------------------

static bool file_exists(char *name) {
    return ixsfirst(name,0,0L) == E_OK;
}

const PROGRAM_READER file_program_reader = {
    file_exists,
    xopen,
    xread,
    xclose,
    xlseek
};


// ROM program reader ---------------------------------------------------------
#define ROM_NAME_SIZE 14

// Describes a program stored in the ROM or flash.
typedef struct {
    char    name[ROM_NAME_SIZE]; // ABCDEFGH.EXT, null terminated, even size
    long    length;
    UBYTE   contents[];
} ROM_PRG;

// Info about current ROM-based program being loaded
static long     romprg_offset; /* Current position in ROM file */
static ROM_PRG* romprg_cur;    /* Current ROM file we're dealing with */

// Start/end labels from the "INCBIN"'ed program.
extern const ROM_PRG binary_tools_rom_prg_start;

static const ROM_PRG * const rom_prgs[1] = {
    // Programs included here must have a matching entry in the linker file.
    &binary_tools_rom_prg_start
};

static ROM_PRG* romprg_find(char *name) {
    int i;
    for (i=0; i<sizeof(rom_prgs)/sizeof(ROM_PRG*); i++) {
        ROM_PRG* p = (ROM_PRG*)rom_prgs[i];
        if (strncasecmp(name, p->name, ROM_NAME_SIZE) == 0)
            return p;
    }
    return NULL;
}

static bool romprg_exists(char *name) {
    return romprg_find(name) != NULL;
}

static long romprg_open(char *name, int mode) {
    romprg_offset = 0L;
    romprg_cur = romprg_find(name);
    return 1L; /* Whatever, no importance as romprg_xxx stuff doesn't use handlers. */
}

static long romprg_read(int handle, long length, void* buffer) {
    // This is not completely safe, but since if it not for free user usage
    // we're not checking for length == 0 or "to" being already EOF.
    if (romprg_offset + length > romprg_cur->length)
        length = romprg_cur->length - romprg_offset;
    memcpy(buffer, &romprg_cur->contents[romprg_offset], length);
    romprg_offset += length;
    return  length;
}

static long romprg_close(int handle) {
    romprg_cur = NULL;
    return E_OK;
}

static long romprg_seek(long offset, int handle, int whence) {
    switch (whence) {
        case 0: /* SEEK_BEG*/
            romprg_offset = offset;
            break;
#if 0 /* Nothing uses that so let's save a few bytes of ROM space */
        case 1: /* SEEK_CUR */
            romprg_offset += offset;
            break;
#endif
        case 2: /* SEEK_END */
            romprg_offset = romprg_cur->length-1 + offset;
            break;
        default:
            return EINVFN;
    }
    if (romprg_offset < 0 || romprg_offset > romprg_cur->length)
        return ERANGE;
    return romprg_offset;
}

const PROGRAM_READER rom_program_reader = {
    romprg_exists,
    romprg_open,
    romprg_read,
    romprg_close,
    romprg_seek
};
