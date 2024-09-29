/*
 * keyb_ro.h - Romanian keyboard layout definition
 *
 * Copyright (C) 2020-2024 The EmuTOS development team
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

static const UBYTE keytbl_ro_norm[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0',  '-',  '=',  0x08, 0x09,
     'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
     'o',  'p', 0xe3, 0xee, 0x0d,    0,  'a',  's',
     'd',  'f',  'g',  'h',  'j',  'k',  'l', 0xba,
    0xfe,  '`',    0, 0xe2,  'z',  'x',  'c',  'v',
     'b',  'n',  'm',  ',',  '.',  '/',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0, 0x7f,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
};

static const UBYTE keytbl_ro_shft[] = {
       0, 0x1b,  '!',  '@',  '#',  '$',  '%',  '^',
     '&',  '*',  '(',  ')',  '_',  '+', 0x08, 0x09,
     'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P', 0xc3, 0xde, 0x0d,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L', 0xaa,
    0xde,  '~',    0, 0xc2,  'Z',  'X',  'C',  'V',
     'B',  'N',  'M',  ';',  ':',  '?',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,  '7',
     '8',    0,  '-',  '4',    0,  '6',  '+',    0,
     '2',    0,  '0', 0x7f,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
};

static const UBYTE keytbl_ro_caps[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0',  '-',  '=', 0x08, 0x09,
     'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P',  '[',  ']', 0x0d,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L',  ';',
    '\'',  '`',    0, '\\',  'Z',  'X',  'C',  'V',
     'B',  'N',  'M',  ',',  '.',  '/',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0, 0x7f,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
};


static const struct keytbl keytbl_ro = {
    keytbl_ro_norm,
    keytbl_ro_shft,
    keytbl_ro_caps,
    NULL,
    0
};
