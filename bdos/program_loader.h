#ifndef PROGRAM_LOADER_H
#define PROGRAM_LOADER_H

#include "portab.h"
#include "fs.h"
#include "pghdr.h"
#include "program_reader.h"

typedef struct load_state_t {
    struct program_loader_t *loader;

    /* This provides methods for reading the program from disk/rom */
    PROGRAM_READER *prg_reader;
    int fh; /* handle of the program file (only applies to file_program_reader)*/

    /* Owner to set for any memory block allocated to the child process */
    PD *owner;

    /* Filled in by get_program_info */
    ULONG flags;            /* flags as per PGMHDR01.h01_flags */
    ULONG relocatable_size; /* size of relocatable block to allocate (may be 0) */
    
    /* Filled by load_program_into_memory */
    void *prg_entry_point;  /* entry point of the program (address of first instruction to execute) */
    
    void *data;             /* free for use by the loader, to store info about a file being loaded */
} LOAD_STATE;


typedef struct program_loader_t {
    /* Tells whether this loader can manage that executable format.
     * Returns < 0 if error, 1 if success, 0 if file cannot be handled by the loader. */
    WORD (*can_load)(const LOAD_STATE *lstate);

    /* The program loader must read or make up a program header */
    WORD (*get_program_info)(LOAD_STATE *lstate);

    /* Load the program into memory and tweak the PD accordingly. 
     * It's allowed to allocate memory */
    LONG (*load_program_into_memory)(PD *pdptr, LOAD_STATE *lstate);
} PROGRAM_LOADER;


void find_program_loader(LOAD_STATE *lstate);

#endif