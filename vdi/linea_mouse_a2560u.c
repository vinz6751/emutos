/*
 * linea_mouse_a2560u.c - A2560U Foenix-specific mouse support
 *
 * Copyright 1982 by Digital Research Inc.  All rights reserved.
 * Copyright 1999 by Caldera, Inc. and Authors:
 * Copyright 2002-2022 by The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#include "emutos.h"
#include "asm.h"

#ifdef MACHINE_A2560U

#include "linea.h"
#include "a2560u.h"


static void paint_mouse(WORD x, WORD y)
{
    a2560u_debug("paint_mouse @ %d,%d", x, y);
    R16(VICKY_MOUSE_X) = x;
    R16(VICKY_MOUSE_Y) = y;
    vicky2_show_mouse();
}


static void unpaint_mouse(void)
{
    //vicky2_hide_mouse();
}


static const unsigned char Color_Pointer_bin[] = {
	0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xfc, 0xf9, 0xfd, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xf3, 0xed, 0xf5, 0xbd, 0x86, 0xcb, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xf4, 0xee, 0xf6, 0xce, 0xb8, 0xd6, 0xba, 0x59, 0xd0, 0xd8, 0x6e, 0xe9,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x74, 0x4f, 0x7e, 0xb3, 0x9c, 0xbb, 0xaf, 0x63, 0xc2, 0xa6, 0x51, 0xb9,
	0xe5, 0x78, 0xf6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x6a, 0x42, 0x75, 0x89, 0x5e, 0x95, 0xa6, 0x4d, 0xbb, 0xa9, 0x41, 0xbf,
	0x8d, 0x39, 0x9c, 0xe5, 0x78, 0xf6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x56, 0x05, 0x63, 0x6e, 0x27, 0x7c, 0x84, 0x32, 0x95, 0xb0, 0x24, 0xc9,
	0xa8, 0x20, 0xc1, 0x84, 0x24, 0x95, 0xe5, 0x78, 0xf6, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x55, 0x05, 0x62, 0x57, 0x05, 0x65, 0x69, 0x21, 0x77, 0xa7, 0x25, 0xbf,
	0xa9, 0x20, 0xc2, 0xb0, 0x26, 0xc8, 0xa2, 0x36, 0xb4, 0xd9, 0x6f, 0xea,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x55, 0x05, 0x62, 0x55, 0x05, 0x62, 0x56, 0x05, 0x63, 0x7d, 0x17, 0x90,
	0xad, 0x15, 0xc8, 0xae, 0x1b, 0xc8, 0xad, 0x2d, 0xc4, 0x96, 0x24, 0xaa,
	0xe5, 0x78, 0xf6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x55, 0x05, 0x62, 0x55, 0x05, 0x62, 0x55, 0x05, 0x62, 0x58, 0x09, 0x66,
	0xac, 0x13, 0xc5, 0xae, 0x1e, 0xc7, 0xd8, 0x69, 0xeb, 0xe3, 0x77, 0xf4,
	0xe5, 0x78, 0xf6, 0xe5, 0x78, 0xf6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x55, 0x05, 0x62, 0x55, 0x05, 0x62, 0x00, 0x00, 0x00, 0x55, 0x05, 0x62,
	0x7f, 0x0d, 0x91, 0xbd, 0x3b, 0xd4, 0xde, 0x70, 0xf0, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x55, 0x05, 0x62, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x58, 0x05, 0x66, 0x7d, 0x20, 0x8e, 0xd5, 0x69, 0xe7, 0xe5, 0x78, 0xf6,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x35, 0x00, 0x3d, 0x78, 0x0f, 0x89, 0xad, 0x3e, 0xc2, 0xe5, 0x78, 0xf6,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x5b, 0x06, 0x69, 0x7f, 0x24, 0x8f, 0xdd, 0x73, 0xee,
	0xe5, 0x78, 0xf6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x30, 0x00, 0x37, 0x52, 0x0b, 0x5f, 0xe5, 0x78, 0xf6,
	0xe5, 0x78, 0xf6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x37, 0xe5, 0x78, 0xf6,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


/* This is here rather than in a2560u.c because MFORM would required to include aesdefs.h there.
 * and I'd like the a2560u.c to not have dependencies on EmuTOS so it can be used elsewhere. */
static void set_mouse_cursor(const MFORM *src)
{
    /* No hot-spot support in the Foenix :( */
#if 1
    short src_offset = 0;
    short dest_offset = 0;
    unsigned short low_components;
    unsigned short hi_components;  
    UWORD *MousePointer_Mem_A = (UWORD*)VICKY_MOUSE_MEM;
    int i;
    for (i = 0; i < 256; i++) {
        low_components = (Color_Pointer_bin[src_offset+1] << 8) + Color_Pointer_bin[src_offset];
        hi_components = Color_Pointer_bin[src_offset+2];
        MousePointer_Mem_A[dest_offset] = low_components;
        MousePointer_Mem_A[dest_offset+1] = hi_components;

        src_offset += 3;
        dest_offset += 2;
    }
#else
    int r,c; /* row, column */
    UWORD mask;
    UWORD data;
    UWORD *v = (UWORD*)VICKY_MOUSE_MEM;

    //a2560u_debug("set_mouse_cursor");
    for (r = 0; r < 16; r++)
    {
        mask = src->mf_mask[r];
        data = src->mf_data[r];

        for (c = 0; c < 16; c++)
        {
            if (mask & 0x8000)
            {
                // 2 words, AR then GB
                if (data & 0x8000)
                {
                    /* White */
                    *v++ = 0xffff;
                    *v++ = 0xffff;                   
                }
                else
                {
                    /* Black */
                    *v++= 0xff0;
                    *v++= 0x000;
                }
            }
            else
            {
                /* Transparent */
                *v++ = 0;
                *v++ = 0;
            }
            mask <<= 1;
            data <<= 1;
        }
    }

    v = (UWORD*)VICKY_MOUSE_MEM;
    for (r=0;r<1024;r++)
        *v++ = r;
#endif
    //a2560u_debug("set_mouse_cursor done");
}


static void vbl_draw(WORD x, WORD y)
{
    R16(VICKY_MOUSE_X) = x;
    R16(VICKY_MOUSE_Y) = y;
    //a2560u_debug("%d %d",x,y);
}


const LINEA_MOUSE_RENDERER mouse_display_driver = {
    .init = just_rts,
    .deinit = just_rts,
    vbl_draw,
    paint_mouse,
    unpaint_mouse,
    set_mouse_cursor,
    just_rts /* resolution_changed */
};

#endif
