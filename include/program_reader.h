#ifndef PROGRAM_READER_H
#define PROGRAM_READER_H

#include <stdbool.h>

#include "portab.h"

/* This is the interface that provides functions that can be used during loading
 * of a program. It exists so we can "read" from disk (as normal) but also from 
 * ROM to access builting programs. */
typedef struct {
    bool (*exists)(char *name);
    long (*open)(char *name, int mode);
    long (*read)(int handle, long length, void* buffer);
    long (*close)(int handle);
    long (*seek)(long offset, int handle, int whence);
    long data; // For free use
} PROGRAM_READER;

extern const PROGRAM_READER file_program_reader;
extern const PROGRAM_READER rom_program_reader;

#endif