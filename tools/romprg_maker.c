/* This program helps produce a ROM_PRG, which is a program that can be
 * embedded in the OS image.
 * Author: Vincent Barrilliot.
 * 
 * This is not yet integrated into the Makefile etc. To use it, you have to
 * compile it using your gcc, then update if necessary the emutos.ld and
 * program_reader.c to register in both places.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

/* This definition must match that in program_reader.h */
typedef struct _rom_prg {
    char     filename[14]; // ABCDEFGH.EXT, null terminated
    uint32_t length;
    uint8_t  contents[];
} ROM_PRG;


int main(int argc, char *argv[]) {
    int ret;

    if (argc != 2) {
        puts("Usage: romprg_maker <filename>\n");
        return 0;
    }

    uint8_t *src_buf = NULL;
    FILE *src = fopen(argv[1],"rb");
    if (src == NULL) {
        fprintf(stderr, "Problem opening file %f\n", argv[1]);
        ret = -1;
        goto cleanup;
    }

    fseek(src, 0L, 2);
    uint32_t src_size = ftell(src);
    fseek(src, 0L, 0);
    // Read the file into a buffer
    src_buf = (uint8_t*)malloc(src_size);
    fread(src_buf, sizeof(uint8_t), src_size, src);

    FILE *dst = fopen("rom.prg", "wb");
    if (dst == NULL) {
        fprintf(stderr, "Problem writting destination file\n");
        ret = -1;
        goto cleanup;
    }
 
    // Write ROM name
    char rom_name[14];
    for (int i=0; i<sizeof(rom_name); i++)
        rom_name[i] = 0;
    strcpy(rom_name, "rom.prg");
    fwrite(rom_name, sizeof(char), sizeof(rom_name), dst);

    // Write ROM name in 68k endianness
    printf("Program size: %ld\n", src_size);
    uint32_t size = htonl(src_size);
    fwrite(&size, sizeof(uint32_t), 1, dst);
    
    // Write file content
    fwrite(src_buf, sizeof(uint8_t), src_size, dst);
    ret = 0;

cleanup:
    if (src)
        fclose(src);
    if (dst)
        fclose(src);
    if (src_buf)
        free(src_buf);
    return ret;
}