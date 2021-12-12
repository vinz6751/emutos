/*
 * a2560u - Foenix Retro Systems A2560U specific functions
 *
 * Copyright (C) 2013-2021 The EmuTOS development team
 *
 * Authors:
 *  VB   Vincent Barrilliot
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef A2560U_H
#define A2560U_H

#include <stdint.h>
#include <stdbool.h>
#include "vicky2.h"
#include "foenix.h"

#define VIDEO_RAM_SIZE VRAM0_SIZE

void a2560u_init(void); /* C entry point for initialisation */
void a2560u_screen_init(void);
void a2560u_get_current_mode_info(uint16_t *planes, uint16_t *hz_rez, uint16_t *vt_rez);
void a2560u_setphys(const uint8_t *address);
void a2560u_set_border_color(uint32_t color);

/* Serial port */
uint32_t a2560u_bcostat1(void);
void a2560u_bconout1(uint8_t byte);

/* Timers */
struct a2560u_timer_t {
    uint32_t control;  /* Control register */
    uint32_t value;    /* Value register   */
    uint32_t compare;  /* Compare register */
    uint32_t deprog;   /* AND the control register with this to clear timer settings */
    uint32_t prog;     /* OR the control register with this to program the timer in countdown mode */
    uint32_t start;    /* OR the control register with this to start the timer in countdown mode */
    uint16_t irq_mask; /* AND this to the irq_pending_group to acknowledge the interrupt */
    uint16_t vector;   /* Exception vector number (not address !) */
    uint32_t dummy;    /* Useless but having the structure 32-byte larges makes it quicker to generate an offset with lsl #5 */
};

void a2560u_xbtimer(uint16_t timer, uint16_t control, uint16_t data, void *vector);
void a2560u_set_timer(uint16_t timer, uint32_t frequency, bool repeat, void *handler);
void a2560u_timer_enable(uint16_t timer, bool enable);

#endif