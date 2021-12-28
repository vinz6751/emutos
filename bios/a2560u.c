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

/* #define ENABLE_KDEBUG */

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "emutos.h"
#include "portab.h"
#include "vectors.h"
#include "tosvars.h"
#include "bios.h"
#include "processor.h"
#include "biosext.h"            /* for cache control routines */
#include "gemerror.h"
#include "ikbd.h"               /* for call_mousevec() */
#include "screen.h"
#include "delay.h"
#include "asm.h"
#include "string.h"
#include "disk.h"
#include "biosmem.h"
#include "bootparams.h"
#include "doprintf.h"
#include "machine.h"
#include "has.h"
#include "../bdos/bdosstub.h"
#include "screen.h"
#include "stdint.h"
#include "foenix.h"
#include "uart16550.h" /* Serial port */
#include "sn76489.h"   /* Programmable Sound Generator */
#include "wm8776.h"    /* Audio codec */
#include "bq4802ly.h"  /* Real time clock */
#include "ps2.h"
#include "ps2_keyboard.h"
#include "vicky2.h"    /* VICKY II graphics controller */
#include "a2560u.h"

/* Local variables ***********************************************************/
static uint32_t cpu_freq; /* CPU frequency */
static uint32_t vbl_freq; /* VBL frequency */


/* Prototypes ****************************************************************/

void a2560u_init_lut0(void);
static void timer_init(void);

/* Interrupt */
static void irq_init(void);
void irq_add_handler(int id, void *handler);
void a2560u_irq_bq4802ly(void);
void a2560u_irq_vicky(void);
void a2560u_irq_ps2kbd(void);
void a2560u_irq_ps2mouse(void);

/* Implementation ************************************************************/

void a2560u_init(void)
{
    *((unsigned long * volatile)0xB40008) = 0x0000000; /* Black border */

    cpu_freq = CPU_FREQ; /* TODO read that from GAVIN's Machine ID */

    irq_init();
    uart16550_init(UART0); /* So we can debug to serial port early */
    timer_init();    
    wm8776_init();
    sn76489_mute_all();

    /* Clear screen and home */
    a2560u_debug(("\033Ea2560u_init()"));
}

void a2560u_screen_init(void)
{
    vicky2_init();
    vbl_freq = 60; /* TODO read that from VICKY's video mode */

    /* Setup VICKY interrupts handler (VBL, HBL etc.) */
    vblsem = 0;
    a2560u_irq_set_handler(INT_SOF_A, int_vbl);
}

void a2560u_setphys(const uint8_t *address)
{
    vicky2_set_bitmap0_address(address-VRAM_Bank0);
}

void a2560u_set_border_color(uint32_t color)
{
    vicky2_set_border_color(color);
}

void a2560u_get_current_mode_info(UWORD *planes, UWORD *hz_rez, UWORD *vt_rez)
{
    FOENIX_VIDEO_MODE mode;
    vicky2_get_video_mode(&mode);
    *planes = 8;
    *hz_rez = mode.w;
    *vt_rez = mode.h;
}


/* Serial port support*/

uint32_t a2560u_bcostat1(void)
{
    return uart16550_can_put(UART0);
}

void a2560u_bconout1(uint8_t byte)
{
    return uart16550_put(UART0,&byte, 1);
}


/* For being able to translate settings of the ST's MFP68901 we need this */
static const uint16_t mfp_timer_prediv[] = { 0,4,10,16,50,64,100,200 };
#define MFP68901_FREQ 2457600 /* The ST's 68901 MFP is clocked at 2.4576MHZ */

void a2560u_xbtimer(uint16_t timer, uint16_t control, uint16_t data, void *vector)
{
    uint32_t frequency;
    uint32_t timer_clock;

    if (timer > 3)
        return;

    /* Read the intent of the caller */
    if (timer == 2) 
        control >>= 4; /* TCDCR has control bit for Timer C and D but D has bits 4,5,6 */
    if (control & 0x8)
    {
        /* We only support "delay mode. Other modes have bit 3 set */
        KDEBUG(("Not supported MFP timer %d mode %04x\n", 'A' + timer, control));
        return;
    }    
    /* Quantity of 2.4576MHz ticks before the timer fires */
    frequency = mfp_timer_prediv[frequency];
    if (data != 0)
        frequency *= data;
    
    /* Convert that according to our timer's clock */
    timer_clock = timer == 3 ? vbl_freq : cpu_freq;
    frequency = (frequency * timer_clock) / MFP68901_FREQ;

    a2560u_set_timer(timer, frequency, false, vector);
}

/* This holds all readily usable details of timers per timer number,
 * so we don't have to compute anything when setting a timer.
 * It's done so timers can be reprogrammed quickly without having to do
 * computation, so to get the best timing possible. */
/* Timers */
const struct a2560u_timer_t {
    uint32_t control;  /* Control register */
    uint32_t value;    /* Value register   */
    uint32_t compare;  /* Compare register */
    uint32_t deprog;   /* AND the control register with this to clear timer settings */
    uint32_t prog;     /* OR the control register with this to program the timer in countdown mode */
    uint32_t start;    /* OR the control register with this to start the timer in countdown mode */
    uint16_t irq_mask; /* OR this to the irq_pending_group to acknowledge the interrupt */
    uint16_t vector;   /* Exception vector number (not address !) */
    uint32_t dummy;    /* Useless but having the structure 32-byte larges makes it quicker to generate an offset with lsl #5 */
} a2560u_timers[] = 
{
    #define COUNT_DOWN 0xA4L
    /* 0xA4 is TIMER_CTRL_IRQ|TIMER_CTRL_LOAD||TIMER_CTRL_RELOAD (count down)
     * 0x9A is TIMER_CTRL_IRQ|TIMER_CTRL_CLEAR|TIMER_CTRL_RECLEAR (count up)
    /* TODO 1L as "start" is really TIMER_CTRL_ENABLE but I get a "left shift count >= width of type" warning I can't get rid of */
    { TIMER_CTRL0, TIMER0_VALUE, TIMER0_COMPARE, 0xffffff00, COUNT_DOWN << 0,  1L << 0,  0x0100, INT_TIMER0_VECN },
    { TIMER_CTRL0, TIMER1_VALUE, TIMER1_COMPARE, 0xffff00ff, COUNT_DOWN << 8,  1L << 8,  0x0200, INT_TIMER1_VECN },
    { TIMER_CTRL0, TIMER2_VALUE, TIMER2_COMPARE, 0xff00ffff, COUNT_DOWN << 16, 1L << 16, 0x0400, INT_TIMER2_VECN },
    { TIMER_CTRL1, TIMER3_VALUE, TIMER3_COMPARE, 0xffffff00, COUNT_DOWN << 0,  1L << 0,  0x0800, INT_TIMER3_VECN }
};


/*
 * Initialise the timers.
 */
static void timer_init(void) 
{
    int i;

    /* Disable all timers */
    for (i = 0; i < sizeof(a2560u_timers)/sizeof(struct a2560u_timer_t); i++)
        a2560u_timer_enable(i, false);
}

/*
 * Program a timer but don't start it. This causes the timer to stop.
 * timer: timer number
 * frequency: desired frequency
 * repeat: whether the timer should reload after firing interrupt
 * handler: routine to execute when timer fires
 */
void a2560u_set_timer(uint16_t timer, uint32_t frequency, bool repeat, void *handler)
{
    struct a2560u_timer_t *t;
    uint16_t sr;    

    if (timer > 3)
        return;

    KDEBUG(("\033ESet timer %d, freq:%ldHz, repeat:%s, handler:%p\n",timer,frequency,repeat?"ON":"OFF",handler));
    
    /* Identify timer control register to use */
    t = (struct a2560u_timer_t *)&a2560u_timers[timer];

    /* We don't want interrupts while we reprogramming */
    sr = set_sr(0x2700);

    /* Stop the timer while we configure */    
    a2560u_timer_enable(timer, false);

    /* Stop and reprogram and the timer, but don't start. */
    /* TODO: In case of control register 0, we write the config of 3 timers at once because
     * the timers control register has to be addressed as long word.
     * Can that have nasty effects on any other running timers ? */
    R32(t->control) = (R32(t->control) & t->deprog) | t->prog;

    /* Set timer period */
    {
        uint32_t value = cpu_freq / frequency;
        uint32_t compare = 0L;        
        R32(t->value) = value;
        R32(t->compare) = compare;        
    }

    /* Set handler */
    setexc(t->vector, (uint32_t)handler);

    /* Before starting the timer, ignore any previous pending interrupt from it */    
    if (R16(IRQ_PENDING_GRP1) & t->irq_mask)
        R16(IRQ_PENDING_GRP1) = t->irq_mask; /* Yes it's an assignment, no a OR. That's how interrupts are acknowledged */

    /* Unmask interrupts for that timer */
    R16(IRQ_MASK_GRP1) &= ~t->irq_mask;

    set_sr(sr);
#if 0
    KDEBUG(("AFTER SETTING TIMER %d\n", timer));
    KDEBUG(("CPU          sr=%04x\n", get_sr()));
    KDEBUG(("vector       0x%02x=%p\n",t->vector,(void*)setexc(t->vector, -1L)));
    KDEBUG(("value        %p=%08lx\n",(void*)t->value,R32(t->value)));
    KDEBUG(("irq_pending  %p=%04x\n", (void*)IRQ_PENDING_GRP1,R16(IRQ_PENDING_GRP1)));
    KDEBUG(("irq_mask     %p=%04x\n", (void*)IRQ_MASK_GRP1,R16(IRQ_MASK_GRP1)));    
    KDEBUG(("control      %p=%08lx\n",(void*)t->control,R32(t->control)));    
#endif    
}

/*
 * Enable or disable a timer.
 * timer: timer number
 * enable: 0 to disable, anything to enable
 */
void a2560u_timer_enable(uint16_t timer, bool enable)
{
    struct a2560u_timer_t *t = (struct a2560u_timer_t *)&a2560u_timers[timer];

KDEBUG(("Before %s > control %p=0x%08lx\n",enable?"Enable":"Disable", (void*)t->control,R32(t->control)));
    if (enable)
        R32(t->control) |= t->start;
    else
        R32(t->control) &= ~t->start;

KDEBUG(("After %s  > control %p=0x%08lx\n",enable?"Enable":"Disable", (void*)t->control,R32(t->control)));
    if (R32(t->value) != R32(t->value))
        KDEBUG(("Timer is running: 0x%08lx 0x%08lx 0x%08lx...\n",R32(t->value), R32(t->value), R32(t->value)));
    else
        KDEBUG(("Timer is not running\n"));
}


/*
 * Output the string on the serial port.
 * Use this if the KDEBUG is not available yet.
 */
void a2560u_debug(const char* __restrict__ s, ...)
{
#ifdef ENABLE_KDEBUG
    char msg[80];    
    va_list ap;
    va_start(ap, s);
    sprintf(msg,s,ap);
    doprintf((void(*)(int))a2560u_bconout1, s, ap);
    va_end(ap);
    uart16550_put(UART0, (uint8_t*)"\r\n", 2);
#endif
}

/* Interrupts management */
static void irq_init(void)
{
    int i, j;

    volatile uint16_t *pending = (uint16_t*)IRQ_PENDING_GRP0;
    volatile uint16_t *polarity = (uint16_t*)IRQ_POL_GRP0;
    volatile uint16_t *edge = (uint16_t*)IRQ_EDGE_GRP0;
    volatile uint16_t *mask = (uint16_t*)IRQ_MASK_GRP0;

    for (i = 0; i < IRQ_GROUPS; i++)
    {
        mask[i] = edge[i] = 0xffff;
        pending[i] = 0xffff; /* Acknowledge any pending interrupt */
        polarity[i] = 0;        

        for (j=0; j<16; j++)
            a2560_irq_vectors[i][j] = just_rts;
    }

    for (i=0x40; i<0x60; i++)
        setexc(i,(uint32_t)just_rte);

    setexc(INT_VICKYII, (uint32_t)a2560u_irq_vicky);
}

/* Interrupt handlers for each of the IRQ groups */
void *a2560_irq_vectors[IRQ_GROUPS][16];
/* Utility functions, don't use directly */
static inline uint16_t irq_group(uint16_t irq_id) { return irq_id >> 4; }
static inline uint16_t irq_number(uint16_t irq_id) { return irq_id & 0xf; }
static inline uint16_t irq_mask(uint16_t irq_id) { return 1 << irq_number(irq_id); }
static inline uint16_t *irq_mask_reg(uint16_t irq_id) { return &((uint16_t*)IRQ_MASK_GRP0)[irq_group(irq_id)]; }
static inline uint16_t *irq_pending_reg(uint16_t irq_id) { return &((uint16_t*)IRQ_PENDING_GRP0)[irq_group(irq_id)]; }
#define irq_handler(irqid) (a2560_irq_vectors[irq_group(irqid)][irq_number(irqid)])


/* Enable an interruption. First byte is group, second byte is bit */
void a2560U_irq_enable(uint16_t irq_id)
{
    a2560u_irq_acknowledge(irq_id);
    R16(irq_mask_reg(irq_id)) &= ~irq_mask(irq_id);
    KDEBUG(("a2560U_irq_enable(%u) -> Mask %p=%04x\n", irq_id, irq_mask_reg(irq_id), R16(irq_mask_reg(irq_id))));
}


/* Disable an interruption. First byte is group, second byte is bit */
void a2560U_irq_disable(uint16_t irq_id)
{
    R16(irq_mask_reg(irq_id)) |= irq_mask(irq_id);
    KDEBUG(("a2560U_irq_disable(%u) -> Mask %p=%04x\n", irq_id, irq_mask_reg(irq_id), R16(irq_mask_reg(irq_id))));
}


void a2560u_irq_acknowledge(uint16_t irq_id)
{
    R16(irq_pending_reg(irq_id)) |= irq_mask(irq_id);
}


/* Set an interrupt handler for IRQ managed through GAVIN interrupt registers */
void *a2560u_irq_set_handler(uint16_t irq_id, void *handler)
{    
//    a2560_irq_vectors[0][0] = handler;
//    return 0L;
    void *old_handler = irq_handler(irq_id);
    irq_handler(irq_id) = (void*)handler;

    // char msg[50];
    // sprintf(msg, "a2560_irq_vectors=%p\r\n", (void*)a2560_irq_vectors);
    // a2560u_debug(msg);
    // sprintf(msg,"handler(%d) irq_handler(irq_id)=%p value=%p\r\n",irq_id, &irq_handler(irq_id), irq_handler(irq_id));
    // a2560u_debug(msg);

    KDEBUG(("Handler %04ux: %p\n", irq_id, handler));
    return old_handler;
}

/* Real Time Clock  **********************************************************/

void a2560u_clock_init(void)
{
    setexc(INT_BQ4802LY_VECN, (uint32_t)a2560u_irq_bq4802ly);
    bq4802ly_init();
}


uint32_t a2560u_getdt(void)
{
    uint8_t day, month, hour, minute, second;
    uint16_t year;    

    bq4802ly_get_datetime(&day, &month, &year, &hour, &minute, &second);
    
    return MAKE_ULONG((year << 9) + (month << 5) + day, (hour << 11) + (minute << 5) + second);
}


void a2560u_setdt(uint32_t datetime)
{
    const uint16_t date = HIWORD(datetime);
    const uint16_t time = LOWORD(datetime);

    bq4802ly_set_datetime(
        (date & 0b0000000000011111),       /* day */
        (date & 0b0000000111100000) >> 5,  /* month */
        (date & 0b1111111000000000) >> 9,  /* year */                
        (time & 0b1111100000000000) >> 11, /* hour */
        (time & 0b0000011111100000) >> 5,  /* minute */
        (time & 0b0000000000011111));      /* second */
}


/* PS/2 setup  ***************************************************************/

static void *balloc_proxy(size_t howmuch)
{
    return balloc_stram(howmuch, false);
}

static const struct ps2_driver_t *drivers[] =
{
    &ps2_keyboard_driver  
};


void a2560u_kbd_init(void)
{
    /* Disable IEQ while we're configuring */
    a2560U_irq_disable(INT_KBD_PS2);
    a2560U_irq_disable(INT_MOUSE);

    /* Explain our setup to the PS/2 subsystem */
    ps2_config.counter      = (uint32_t*)&frclock; /* FIXME we're using the VBL counter */
    ps2_config.counter_freq = 60;
    ps2_config.port_data    = (uint8_t*)PS2_DATA;
    ps2_config.port_status  = (uint8_t*)PS2_CMD;
    ps2_config.port_cmd     = (uint8_t*)PS2_CMD;
    ps2_config.n_drivers    = 1;
    ps2_config.drivers      = (struct ps2_driver_t **)drivers;
    ps2_config.malloc       = balloc_proxy;
    ps2_config.on_key_down  = kbd_int;
    ps2_config.on_key_up    = kbd_int;

    ps2_init();

    /* Register GAVIN interrupt handlers */
    setexc(INT_PS2KBD_VECN, (uint32_t)a2560u_irq_ps2kbd);
    setexc(INT_PS2MOUSE_VECN, (uint32_t)a2560u_irq_ps2mouse);

    /* Acknowledge any pending interrupt */
    a2560u_irq_acknowledge(INT_KBD_PS2);
    a2560u_irq_acknowledge(INT_MOUSE);

    /* Go ! */
    a2560U_irq_enable(INT_KBD_PS2);
    a2560U_irq_enable(INT_MOUSE);        
}


void debug1(void* p);
void debug1(void* p)
{   
#if 0
    char msg[30];
    sprintf(msg,"handler=%p\r\n",(void*)frclock);
    a2560u_debug(msg);    
#endif
}