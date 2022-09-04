/*
 * a2560u - Foenix Retro Systems A2560U specific functions
 *
 * Copyright (C) 2013-2022 The EmuTOS development team
 *
 * Authors:
 *  VB   Vincent Barrilliot
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

#ifndef A2560U_H
#define A2560U_H

#ifdef MACHINE_A2560U

#include <stdint.h>
#include <stdbool.h>
#include "../foenix/vicky2.h"
#include "../foenix/a2560u.h"

/* C entry point for initialisation */
void a2560u_bios_init(void);

void a2560u_bios_screen_init(void);
void a2560u_bios_mark_cell_dirty(const uint8_t *cell_address);
void a2560u_bios_mark_screen_dirty(void);
void a2560u_bios_get_current_mode_info(uint16_t *planes, uint16_t *hz_rez, uint16_t *vt_rez);
void a2560u_bios_setrez(int16_t rez, int16_t mode);
int16_t a2560u_bios_vmontype(void);
uint8_t *a2560u_bios_physbase(void);
int32_t a2560u_bios_vgetsize(int16_t mode);
uint16_t a2560u_bios_vsetmode(int16_t mode);
uint32_t a2560u_bios_calc_vram_size(void);
void a2560u_bios_vsetrgb(int16_t index,int16_t count,const uint32_t *rgb);
void a2560u_bios_vgetrgb(int16_t index,int16_t count,uint32_t *rgb);

void a2560u_bios_kbd_init(void);

void a2560u_bios_calibrate_delay(uint32_t calibration_time);
void a2560u_bios_text_init(void);

/* Serial port */
uint32_t a2560u_bios_bcostat1(void);
void a2560u_bios_bconout1(uint8_t byte);
void a2560u_bios_rs232_init(void);

/* Timing stuff */
#define HZ200_TIMER_NUMBER 2
void a2560u_bios_xbtimer(uint16_t timer, uint16_t control, uint16_t data, void *vector);

#endif /* MACHINE_A2560U */

#endif
