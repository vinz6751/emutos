/*
 * keyb_ru.h - Russian keyboard layout definition
 *
 * Copyright (C) 2010-2024 The EmuTOS development team
 *
 * Authors:
 *  Dima Sobolev
 *
 * This file is distributed under the GPL, version 2 or at your
 * option any later version.  See doc/license.txt for details.
 */

static const UBYTE keytbl_ru_norm[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0',  '-',  '=', 0x08, 0x09,
     'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
     'o',  'p',  '[',  ']', 0x0d,    0,  'a',  's',
     'd',  'f',  'g',  'h',  'j',  'k',  'l',  ';',
    '\'',  '`',    0,  '#',  'z',  'x',  'c',  'v',
     'b',  'n',  'm',  ',',  '.',  '/',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0, 0x7f,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
    '\\',    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
};

static const UBYTE keytbl_ru_shft[] = {
       0, 0x1b,  '!',  '@',  '#',  '$',  '%',  '^',
     '&',  '*',  '(',  ')',  '_',  '+', 0x08, 0x09,
     'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P',  '{',  '}', 0x0d,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L',  ':',
    '\"', 0xa1,    0,  '~',  'Z',  'X',  'C',  'V',
     'B',  'N',  'M',  '<',  '>',  '?',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,  '7',
     '8',    0,  '-',  '4',    0,  '6',  '+',    0,
     '2',    0,  '0', 0x7f,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
     '|',    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
};

static const UBYTE keytbl_ru_caps[] = {
       0, 0x1b,  '1',  '2',  '3',  '4',  '5',  '6',
     '7',  '8',  '9',  '0',  '-',  '=', 0x08, 0x09,
     'Q',  'W',  'E',  'R',  'T',  'Y',  'U',  'I',
     'O',  'P',  '[',  ']', 0x0d,    0,  'A',  'S',
     'D',  'F',  'G',  'H',  'J',  'K',  'L',  ';',
    '\'',  '`',    0,  '#',  'Z',  'X',  'C',  'V',
     'B',  'N',  'M',  ',',  '.',  '/',    0,    0,
       0,  ' ',    0,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
       0,    0,  '-',    0,    0,    0,  '+',    0,
       0,    0,    0, 0x7f,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
    '\\',    0,    0,  '(',  ')',  '/',  '*',  '7',
     '8',  '9',  '4',  '5',  '6',  '1',  '2',  '3',
     '0',  '.', 0x0d,    0,    0,    0,    0,    0,
       0,    0,    0,    0,    0,    0,    0,    0,
};

static const UBYTE keytbl_ru_altnorm[] = {
    0x00, 0x1b, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x30, 0x2d, 0x3d, 0x08, 0x09,
    0xe9, 0xf6, 0xf3, 0xea, 0xe5, 0xed, 0xe3, 0xf8,
    0xf9, 0xe7, 0xf5, 0xfa, 0x0d, 0x00, 0xf4, 0xfb,
    0xe2, 0xe0, 0xef, 0xf0, 0xee, 0xeb, 0xe4, 0xe6,
    0xfd, 0x60, 0x00, 0x90, 0xff, 0xf7, 0xf1, 0xec,
    0xe8, 0xf2, 0xfc, 0xe1, 0xfe, 0x2e, 0x00, 0x00,
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x2d, 0x00, 0x00, 0x00, 0x2b, 0x00,
    0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3c, 0x00, 0x00, 0x28, 0x29, 0x2f, 0x2a, 0x37,
    0x38, 0x39, 0x34, 0x35, 0x36, 0x31, 0x32, 0x33,
    0x30, 0x2e, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const UBYTE keytbl_ru_altshft[] = {
    0x00, 0x1b, 0x21, 0x22, 0xb5, 0x3b, 0x25, 0x3a,
    0x3f, 0x2a, 0x28, 0x29, 0x5f, 0x2b, 0xbc, 0x09,
    0xc9, 0xd6, 0xd3, 0xca, 0xc5, 0xcd, 0xc3, 0xd8,
    0xd9, 0xc7, 0xd5, 0xda, 0x0d, 0x00, 0xd4, 0xdb,
    0xc2, 0xc0, 0xcf, 0xd0, 0xce, 0xcb, 0xc4, 0xc6,
    0xdd, 0xa1, 0x00, 0xb0, 0xdf, 0xd7, 0xd1, 0xcc,
    0xc8, 0xd2, 0xdc, 0xc1, 0xde, 0x2c, 0x00, 0x00,
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x2d, 0x00, 0x00, 0x00, 0x2b, 0x00,
    0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3e, 0x00, 0x00, 0x28, 0x29, 0x2f, 0x2a, 0x37,
    0x38, 0x39, 0x34, 0x35, 0x36, 0x31, 0x32, 0x33,
    0x30, 0x2e, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const UBYTE keytbl_ru_altcaps[] = {
    0x00, 0x1b, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x30, 0x2d, 0x3d, 0x08, 0x09,
    0xc9, 0xd6, 0xd3, 0xca, 0xc5, 0xcd, 0xc3, 0xd8,
    0xd9, 0xc7, 0xd5, 0xda, 0x0d, 0x00, 0xd4, 0xdb,
    0xc2, 0xc0, 0xcf, 0xd0, 0xce, 0xcb, 0xc4, 0xc6,
    0xdd, 0x60, 0x00, 0xb0, 0xdf, 0xd7, 0xd1, 0xcc,
    0xc8, 0xd2, 0xdc, 0xc1, 0xde, 0x2e, 0x00, 0x00,
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x2d, 0x00, 0x00, 0x00, 0x2b, 0x00,
    0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x3c, 0x00, 0x00, 0x28, 0x29, 0x2f, 0x2a, 0x37,
    0x38, 0x39, 0x34, 0x35, 0x36, 0x31, 0x32, 0x33,
    0x30, 0x2e, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static const struct keytbl keytbl_ru = {
    keytbl_ru_norm,
    keytbl_ru_shft,
    keytbl_ru_caps,
    keytbl_ru_altnorm,
    keytbl_ru_altshft,
    keytbl_ru_altcaps,
    NULL,
    DUAL_KEYBOARD
};
