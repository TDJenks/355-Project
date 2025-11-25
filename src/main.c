//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------
// School: University of Victoria, Canada.
// Course: ECE 355 "Microprocessor-Based Systems".
// This is template code for Part 2 of Introductory Lab.
//
// See "system/include/cmsis/stm32f051x8.h" for register/bit definitions.
// See "system/src/cmsis/vectors_stm32f051x8.c" for handler declarations.
// ----------------------------------------------------------------------------

#include <stdio.h>
#include "diag/Trace.h"
#include <string.h>
#include <math.h>
#include "cmsis/cmsis_device.h"


// ----------------------------------------------------------------------------
//
// STM32F0 empty sample (trace via $(trace)).
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the $(trace) output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


/* Definitions of registers and their bits are
   given in system/include/cmsis/stm32f051x8.h */


/* Clock prescaler for TIM2 timer: no prescaling */
#define myTIM2_PRESCALER ((uint16_t)0x0000)
/* Maximum possible setting for overflow */
#define myTIM2_PERIOD ((uint32_t)0xFFFFFFFF)

void myADC_Init(void);
void myDAC_Init(void);
void myGPIOA_Init(void);
void myGPIOB_Init(void);
void mySPI_Init(void);
void myTIM2_Init(void);
void myEXTI_Init(void);
uint16_t adc_read(void);
void dac_write(uint16_t);


void oled_Write_Cmd(unsigned char);
void oled_Write_Data(unsigned char);

void oled_config(void);

void refresh_OLED(float,float);

static float adc_to_voltage(uint16_t adc_val);
static float adc_to_ohms(uint16_t adc_val);

SPI_HandleTypeDef SPI_Handle;


//
// LED Display initialization commands
//
const uint8_t oled_init_cmds[] = {
    0xAE,
    0x20, 0x00,
    0x40,
    0xA1,
    0xC8,
    0xA8, 0x3F,
    0xD3, 0x00,
    0xDA, 0x12,
    0xD5, 0x80,
    0xD9, 0xF1,
    0xDB, 0x30,
    0x81, 0x7F,
    0xA4,
    0xA6,
    0x8D, 0x14,
    0xAF
};

//
// Character specifications for LED Display (1 row = 8 bytes = 1 ASCII character)
// Example: to display '4', retrieve 8 data bytes stored in Characters[52][X] row
//          (where X = 0, 1, ..., 7) and send them one by one to LED Display.
// Row number = character ASCII code (e.g., ASCII code of '4' is 0x34 = 52)
//
unsigned char Characters[][8] = {
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // SPACE
    {0b00000000, 0b00000000, 0b01011111, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // !
    {0b00000000, 0b00000111, 0b00000000, 0b00000111, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // "
    {0b00010100, 0b01111111, 0b00010100, 0b01111111, 0b00010100,0b00000000, 0b00000000, 0b00000000},  // #
    {0b00100100, 0b00101010, 0b01111111, 0b00101010, 0b00010010,0b00000000, 0b00000000, 0b00000000},  // $
    {0b00100011, 0b00010011, 0b00001000, 0b01100100, 0b01100010,0b00000000, 0b00000000, 0b00000000},  // %
    {0b00110110, 0b01001001, 0b01010101, 0b00100010, 0b01010000,0b00000000, 0b00000000, 0b00000000},  // &
    {0b00000000, 0b00000101, 0b00000011, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // '
    {0b00000000, 0b00011100, 0b00100010, 0b01000001, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // (
    {0b00000000, 0b01000001, 0b00100010, 0b00011100, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // )
    {0b00010100, 0b00001000, 0b00111110, 0b00001000, 0b00010100,0b00000000, 0b00000000, 0b00000000},  // *
    {0b00001000, 0b00001000, 0b00111110, 0b00001000, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // +
    {0b00000000, 0b01010000, 0b00110000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // ,
    {0b00001000, 0b00001000, 0b00001000, 0b00001000, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // -
    {0b00000000, 0b01100000, 0b01100000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // .
    {0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00000010,0b00000000, 0b00000000, 0b00000000},  // /
    {0b00111110, 0b01010001, 0b01001001, 0b01000101, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // 0
    {0b00000000, 0b01000010, 0b01111111, 0b01000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // 1
    {0b01000010, 0b01100001, 0b01010001, 0b01001001, 0b01000110,0b00000000, 0b00000000, 0b00000000},  // 2
    {0b00100001, 0b01000001, 0b01000101, 0b01001011, 0b00110001,0b00000000, 0b00000000, 0b00000000},  // 3
    {0b00011000, 0b00010100, 0b00010010, 0b01111111, 0b00010000,0b00000000, 0b00000000, 0b00000000},  // 4
    {0b00100111, 0b01000101, 0b01000101, 0b01000101, 0b00111001,0b00000000, 0b00000000, 0b00000000},  // 5
    {0b00111100, 0b01001010, 0b01001001, 0b01001001, 0b00110000,0b00000000, 0b00000000, 0b00000000},  // 6
    {0b00000011, 0b00000001, 0b01110001, 0b00001001, 0b00000111,0b00000000, 0b00000000, 0b00000000},  // 7
    {0b00110110, 0b01001001, 0b01001001, 0b01001001, 0b00110110,0b00000000, 0b00000000, 0b00000000},  // 8
    {0b00000110, 0b01001001, 0b01001001, 0b00101001, 0b00011110,0b00000000, 0b00000000, 0b00000000},  // 9
    {0b00000000, 0b00110110, 0b00110110, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // :
    {0b00000000, 0b01010110, 0b00110110, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // ;
    {0b00001000, 0b00010100, 0b00100010, 0b01000001, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // <
    {0b00010100, 0b00010100, 0b00010100, 0b00010100, 0b00010100,0b00000000, 0b00000000, 0b00000000},  // =
    {0b00000000, 0b01000001, 0b00100010, 0b00010100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // >
    {0b00000010, 0b00000001, 0b01010001, 0b00001001, 0b00000110,0b00000000, 0b00000000, 0b00000000},  // ?
    {0b00110010, 0b01001001, 0b01111001, 0b01000001, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // @
    {0b01111110, 0b00010001, 0b00010001, 0b00010001, 0b01111110,0b00000000, 0b00000000, 0b00000000},  // A
    {0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b00110110,0b00000000, 0b00000000, 0b00000000},  // B
    {0b00111110, 0b01000001, 0b01000001, 0b01000001, 0b00100010,0b00000000, 0b00000000, 0b00000000},  // C
    {0b01111111, 0b01000001, 0b01000001, 0b00100010, 0b00011100,0b00000000, 0b00000000, 0b00000000},  // D
    {0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b01000001,0b00000000, 0b00000000, 0b00000000},  // E
    {0b01111111, 0b00001001, 0b00001001, 0b00001001, 0b00000001,0b00000000, 0b00000000, 0b00000000},  // F
    {0b00111110, 0b01000001, 0b01001001, 0b01001001, 0b01111010,0b00000000, 0b00000000, 0b00000000},  // G
    {0b01111111, 0b00001000, 0b00001000, 0b00001000, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // H
    {0b01000000, 0b01000001, 0b01111111, 0b01000001, 0b01000000,0b00000000, 0b00000000, 0b00000000},  // I
    {0b00100000, 0b01000000, 0b01000001, 0b00111111, 0b00000001,0b00000000, 0b00000000, 0b00000000},  // J
    {0b01111111, 0b00001000, 0b00010100, 0b00100010, 0b01000001,0b00000000, 0b00000000, 0b00000000},  // K
    {0b01111111, 0b01000000, 0b01000000, 0b01000000, 0b01000000,0b00000000, 0b00000000, 0b00000000},  // L
    {0b01111111, 0b00000010, 0b00001100, 0b00000010, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // M
    {0b01111111, 0b00000100, 0b00001000, 0b00010000, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // N
    {0b00111110, 0b01000001, 0b01000001, 0b01000001, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // O
    {0b01111111, 0b00001001, 0b00001001, 0b00001001, 0b00000110,0b00000000, 0b00000000, 0b00000000},  // P
    {0b00111110, 0b01000001, 0b01010001, 0b00100001, 0b01011110,0b00000000, 0b00000000, 0b00000000},  // Q
    {0b01111111, 0b00001001, 0b00011001, 0b00101001, 0b01000110,0b00000000, 0b00000000, 0b00000000},  // R
    {0b01000110, 0b01001001, 0b01001001, 0b01001001, 0b00110001,0b00000000, 0b00000000, 0b00000000},  // S
    {0b00000001, 0b00000001, 0b01111111, 0b00000001, 0b00000001,0b00000000, 0b00000000, 0b00000000},  // T
    {0b00111111, 0b01000000, 0b01000000, 0b01000000, 0b00111111,0b00000000, 0b00000000, 0b00000000},  // U
    {0b00011111, 0b00100000, 0b01000000, 0b00100000, 0b00011111,0b00000000, 0b00000000, 0b00000000},  // V
    {0b00111111, 0b01000000, 0b00111000, 0b01000000, 0b00111111,0b00000000, 0b00000000, 0b00000000},  // W
    {0b01100011, 0b00010100, 0b00001000, 0b00010100, 0b01100011,0b00000000, 0b00000000, 0b00000000},  // X
    {0b00000111, 0b00001000, 0b01110000, 0b00001000, 0b00000111,0b00000000, 0b00000000, 0b00000000},  // Y
    {0b01100001, 0b01010001, 0b01001001, 0b01000101, 0b01000011,0b00000000, 0b00000000, 0b00000000},  // Z
    {0b01111111, 0b01000001, 0b00000000, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // [
    {0b00010101, 0b00010110, 0b01111100, 0b00010110, 0b00010101,0b00000000, 0b00000000, 0b00000000},  // back slash
    {0b00000000, 0b00000000, 0b00000000, 0b01000001, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // ]
    {0b00000100, 0b00000010, 0b00000001, 0b00000010, 0b00000100,0b00000000, 0b00000000, 0b00000000},  // ^
    {0b01000000, 0b01000000, 0b01000000, 0b01000000, 0b01000000,0b00000000, 0b00000000, 0b00000000},  // _
    {0b00000000, 0b00000001, 0b00000010, 0b00000100, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // `
    {0b00100000, 0b01010100, 0b01010100, 0b01010100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // a
    {0b01111111, 0b01001000, 0b01000100, 0b01000100, 0b00111000,0b00000000, 0b00000000, 0b00000000},  // b
    {0b00111000, 0b01000100, 0b01000100, 0b01000100, 0b00100000,0b00000000, 0b00000000, 0b00000000},  // c
    {0b00111000, 0b01000100, 0b01000100, 0b01001000, 0b01111111,0b00000000, 0b00000000, 0b00000000},  // d
    {0b00111000, 0b01010100, 0b01010100, 0b01010100, 0b00011000,0b00000000, 0b00000000, 0b00000000},  // e
    {0b00001000, 0b01111110, 0b00001001, 0b00000001, 0b00000010,0b00000000, 0b00000000, 0b00000000},  // f
    {0b00001100, 0b01010010, 0b01010010, 0b01010010, 0b00111110,0b00000000, 0b00000000, 0b00000000},  // g
    {0b01111111, 0b00001000, 0b00000100, 0b00000100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // h
    {0b00000000, 0b01000100, 0b01111101, 0b01000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // i
    {0b00100000, 0b01000000, 0b01000100, 0b00111101, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // j
    {0b01111111, 0b00010000, 0b00101000, 0b01000100, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // k
    {0b00000000, 0b01000001, 0b01111111, 0b01000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // l
    {0b01111100, 0b00000100, 0b00011000, 0b00000100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // m
    {0b01111100, 0b00001000, 0b00000100, 0b00000100, 0b01111000,0b00000000, 0b00000000, 0b00000000},  // n
    {0b00111000, 0b01000100, 0b01000100, 0b01000100, 0b00111000,0b00000000, 0b00000000, 0b00000000},  // o
    {0b01111100, 0b00010100, 0b00010100, 0b00010100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // p
    {0b00001000, 0b00010100, 0b00010100, 0b00011000, 0b01111100,0b00000000, 0b00000000, 0b00000000},  // q
    {0b01111100, 0b00001000, 0b00000100, 0b00000100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // r
    {0b01001000, 0b01010100, 0b01010100, 0b01010100, 0b00100000,0b00000000, 0b00000000, 0b00000000},  // s
    {0b00000100, 0b00111111, 0b01000100, 0b01000000, 0b00100000,0b00000000, 0b00000000, 0b00000000},  // t
    {0b00111100, 0b01000000, 0b01000000, 0b00100000, 0b01111100,0b00000000, 0b00000000, 0b00000000},  // u
    {0b00011100, 0b00100000, 0b01000000, 0b00100000, 0b00011100,0b00000000, 0b00000000, 0b00000000},  // v
    {0b00111100, 0b01000000, 0b00111000, 0b01000000, 0b00111100,0b00000000, 0b00000000, 0b00000000},  // w
    {0b01000100, 0b00101000, 0b00010000, 0b00101000, 0b01000100,0b00000000, 0b00000000, 0b00000000},  // x
    {0b00001100, 0b01010000, 0b01010000, 0b01010000, 0b00111100,0b00000000, 0b00000000, 0b00000000},  // y
    {0b01000100, 0b01100100, 0b01010100, 0b01001100, 0b01000100,0b00000000, 0b00000000, 0b00000000},  // z
    {0b00000000, 0b00001000, 0b00110110, 0b01000001, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // {
    {0b00000000, 0b00000000, 0b01111111, 0b00000000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // |
    {0b00000000, 0b01000001, 0b00110110, 0b00001000, 0b00000000,0b00000000, 0b00000000, 0b00000000},  // }
    {0b00001000, 0b00001000, 0b00101010, 0b00011100, 0b00001000,0b00000000, 0b00000000, 0b00000000},  // ~
    {0b00001000, 0b00011100, 0b00101010, 0b00001000, 0b00001000,0b00000000, 0b00000000, 0b00000000}   // <-
};




// Declare/initialize your global variables here...
// NOTE: You'll need at least one global variable
// (say, timerTriggered = 0 or 1) to indicate
// whether TIM2 has started counting or not.


/*** Call this function to boost the STM32F0xx clock to 48 MHz ***/

void SystemClock48MHz( void )
{
//
// Disable the PLL
//
    RCC->CR &= ~(RCC_CR_PLLON);
//
// Wait for the PLL to unlock
//
    while (( RCC->CR & RCC_CR_PLLRDY ) != 0 );
//
// Configure the PLL for 48-MHz system clock
//
    RCC->CFGR = 0x00280000;
//
// Enable the PLL
//
    RCC->CR |= RCC_CR_PLLON;
//
// Wait for the PLL to lock
//
    while (( RCC->CR & RCC_CR_PLLRDY ) != RCC_CR_PLLRDY );
//
// Switch the processor to the PLL clock source
//
    RCC->CFGR = ( RCC->CFGR & (~RCC_CFGR_SW)) | RCC_CFGR_SW_PLL;
//
// Update the system with the new clock frequency
//
    SystemCoreClockUpdate();

}

/*****************************************************************/

int first_edge = 1;
unsigned int Freq = 0;
unsigned int Res = 0;
uint16_t last_pot = 0;

volatile uint8_t active_source = 0;  // 0 = PB2, 1 = PB3

volatile uint8_t first_edge_chan[2] = {1, 1};
volatile uint32_t captured_timer_chan[2] = {0, 0};
volatile float measured_freq_chan[2] = {0.0f, 0.0f};

#define R_REF 10000.0f   // Change if your reference resistor is different


int main(int argc, char* argv[])
{

	SystemClock48MHz();

	trace_printf("This is Part 2 of Introductory Lab...\n");
	trace_printf("System clock: %u Hz\n", SystemCoreClock);

    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;  /* Enable SYSCFG clock */

    myGPIOA_Init();		/* Initialize I/O port PA */
    myGPIOB_Init();		/* Initialize I/O port PB */
    mySPI_Init();		/* Initialize SPI */
    myTIM2_Init();		/* Initialize timer TIM2 */
    myEXTI_Init();		/* Initialize EXTI */
    myADC_Init();		/* Initialize ADC */
    myDAC_Init();		/* Initialize DAC */

    oled_config();		/* configure OLED */

    uint32_t counter = 0;

	while (1)
	{
		uint16_t value = adc_read();
		dac_write(value);

		float resistance = adc_to_ohms(value);

		counter++;

		if(counter >= 25)
		{
			counter = 0;

			float freq = measured_freq_chan[active_source]; // depends on current active interrupt line

			refresh_OLED(freq, resistance);

			trace_printf("\n---- Measurement ----\n");

			trace_printf("Frequency: %.2f Hz\n", freq);

			 if (isfinite(resistance))
				trace_printf("Resistance: %.2f Ohms\n", resistance);
			else
				trace_printf("Resistance: OPEN CIRCUIT\n"); //board specific
		}
		for (volatile int i = 0; i < 20000; i++);
	}

	return 0;

}

static float adc_to_voltage(uint16_t adc_val)
{
    return (3.3f * adc_val) / 4095.0f;
}

static float adc_to_ohms(uint16_t adc_val)
{
    float v = adc_to_voltage(adc_val);

    if (v >= 3.299f) return INFINITY;

    return R_REF * (v / (3.3f - v));
}

void myADC_Init()
{
	/* Enable clock for ADC */
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN;

	/* Calibration */
	if (ADC1->CR & ADC_CR_ADEN)
		ADC1->CR |= ADC_CR_ADDIS;
	ADC1->CR |= ADC_CR_ADCAL;
	while (ADC1->CR & ADC_CR_ADCAL);

	/* Continuous conversion mode */
	ADC1->CFGR1 |= ADC_CFGR1_CONT;

	/* Channel 1 */
	ADC1->CHSELR = ADC_CHSELR_CHSEL1;

	/* Sampling time */
	ADC1->SMPR |= ADC_SMPR_SMP_2;


	/* Enable ADC */
	ADC1->CR |= ADC_CR_ADEN;
	while(!(ADC1->ISR & ADC_ISR_ADRDY));

	/* Start continuous conversion */
	ADC1->CR |=ADC_CR_ADSTART;

}

uint16_t adc_read(void)
{
		return ADC1->DR;
}

void myDAC_Init() {
	/* Enable clock for DAC */
	RCC->APB1ENR |= RCC_APB1ENR_DACEN;

	/* Enable DAC channel 1, buffer on */
	DAC->CR &= ~DAC_CR_BOFF1;
	DAC->CR |= DAC_CR_EN1;


}

void dac_write(uint16_t value)
{
	/* Write a DAC value (0-4095) */
	DAC->DHR12R1 = value;
}

void myGPIOA_Init()
{
	/* Enable clock for GPIOA peripheral */
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	/* Configure PA0 as input */
	GPIOA->MODER &= ~(GPIO_MODER_MODER0);

	/* Configure PA4 as analog */
	GPIOA->MODER |= 0x300;

	/* Configure PA1 as analog */
	GPIOA->MODER |= 0xC;

	/* Set PA0 to pull-down*/
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0;
}

void myGPIOB_Init()
{
	/* Enable clock for GPIOB peripheral */
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

	/* Configure PB2 as input */
	GPIOB->MODER &= ~(GPIO_MODER_MODER2);

	/* Configure PB3 as input */
	GPIOB->MODER &= ~(GPIO_MODER_MODER3);

	/* Ensure no pull-up/pull-down for PB2 */
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR2);

	/* Ensure no pull-up/pull-down for PB3 */
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR3);

	GPIOB->MODER &= ~(GPIO_MODER_MODER8 | GPIO_MODER_MODER9 | GPIO_MODER_MODER11);  // clear bits
	GPIOB->MODER |=  (GPIO_MODER_MODER8_0 | GPIO_MODER_MODER9_0 | GPIO_MODER_MODER11_0); // set as OUTPUT

	// Push-pull outputs
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_8 | GPIO_OTYPER_OT_9 | GPIO_OTYPER_OT_11);

	// Medium speed
	GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR8_0 | GPIO_OSPEEDER_OSPEEDR9_0 | GPIO_OSPEEDER_OSPEEDR11_0);

	// No pull-up/pull-down
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR8 | GPIO_PUPDR_PUPDR9 | GPIO_PUPDR_PUPDR11);

	// Set CS high, DC low, RES high
	GPIOB->BSRR = GPIO_BSRR_BS_8;  // CS = 1
	GPIOB->BSRR = GPIO_BSRR_BR_9;  // DC = 0
	GPIOB->BSRR = GPIO_BSRR_BS_11; // RES = 1

    // --- PB13 = SCK, PB15 = MOSI (SPI2, AF0) ---
    // Set mode = Alternate Function
    GPIOB->MODER &= ~(GPIO_MODER_MODER13 | GPIO_MODER_MODER15);
    GPIOB->MODER |= (GPIO_MODER_MODER13_1 | GPIO_MODER_MODER15_1);  // AF mode

    // High speed
    GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR13 | GPIO_OSPEEDER_OSPEEDR15);

    // Push-pull
    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT_13 | GPIO_OTYPER_OT_15);

    // No pull-up/pull-down
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR13 | GPIO_PUPDR_PUPDR15);

    // Select AF0 for SPI2 on PB13/PB15
    GPIOB->AFR[1] &= ~(0xF << ((13 - 8) * 4));
    GPIOB->AFR[1] &= ~(0xF << ((15 - 8) * 4));
    // AF0 is 0x0, so no need to OR anything
}

void mySPI_Init()
{
	/* Enable clock for SPI peripheral */
	RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

    SPI_Handle.Instance = SPI2;

    SPI_Handle.Init.Direction = SPI_DIRECTION_1LINE;
    SPI_Handle.Init.Mode = SPI_MODE_MASTER;
    SPI_Handle.Init.DataSize = SPI_DATASIZE_8BIT;
    SPI_Handle.Init.CLKPolarity = SPI_POLARITY_LOW;
    SPI_Handle.Init.CLKPhase = SPI_PHASE_1EDGE;
    SPI_Handle.Init.NSS = SPI_NSS_SOFT;
    SPI_Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
    SPI_Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SPI_Handle.Init.TIMode = SPI_TIMODE_DISABLE;
    SPI_Handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    SPI_Handle.Init.CRCPolynomial = 7;

    HAL_SPI_Init(&SPI_Handle);
}


void myTIM2_Init()
{
	/* Enable clock for TIM2 peripheral */
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	/* Configure TIM2: buffer auto-reload, count up, stop on overflow,
	 * enable update events, interrupt on overflow only */
	TIM2->CR1 |= TIM_CR1_ARPE; // buffer auto reload enabled
	TIM2->CR1 &= ~(TIM_CR1_DIR); // count up
	TIM2->CR1 |= TIM_CR1_OPM; // stop on next update event
	TIM2->CR1 &= ~(TIM_CR1_UDIS); // enable update events
	TIM2->CR1 |= TIM_CR1_URS; // update interrupt on overflow only

	/* Set clock prescaler value */
	TIM2->PSC = myTIM2_PRESCALER;
	/* Set auto-reloaded delay */
	TIM2->ARR = myTIM2_PERIOD;

	/* Update timer registers */
	TIM2->EGR |= TIM_EGR_UG;


	/* Assign TIM2 interrupt priority = 0 in NVIC */
	NVIC_SetPriority(TIM2_IRQn, 0);

	/* Enable TIM2 interrupts in NVIC */
	NVIC_EnableIRQ(TIM2_IRQn);

	/* Enable update interrupt generation */
	TIM2->DIER |= TIM_DIER_UIE;

}


void myEXTI_Init()
{
	/* Map EXTI line to PA0 PB2 and PB3*/
	SYSCFG->EXTICR[0] = (SYSCFG_EXTICR1_EXTI0_PA
						| SYSCFG_EXTICR1_EXTI2_PB
						| SYSCFG_EXTICR1_EXTI3_PB);


	/* EXTI0 line interrupts: set rising-edge trigger */
	EXTI->RTSR |= EXTI_RTSR_TR0;

	/* EXTI2 line interrupts: set rising-edge trigger */
	EXTI->RTSR |= EXTI_RTSR_TR2;

	/* EXTI3 line interrupts: set rising-edge trigger */
	EXTI->RTSR |= EXTI_RTSR_TR3;

	/* Unmask interrupts from EXTI0 line */
	EXTI->IMR |= EXTI_IMR_MR0;

	/* Unmask interrupts from EXTI2 line */
	EXTI->IMR |= EXTI_IMR_MR2;

	/* Mask interrupts from EXTI3 line */
	EXTI->IMR &= ~(EXTI_IMR_MR3);

	/* Assign EXTI0 interrupt priority = 0 in NVIC */
	NVIC_SetPriority(EXTI0_1_IRQn, 0);

	/* Assign EXTI2 & EXTI3 interrupt priority = 1 in NVIC */
	NVIC_SetPriority(EXTI2_3_IRQn, 1);

	/* Enable EXTI0 interrupts in NVIC */
	NVIC_EnableIRQ(EXTI0_1_IRQn);

	/* Enable EXTI2 & EXTI3 interrupts in NVIC */
	NVIC_EnableIRQ(EXTI2_3_IRQn);

}


/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void TIM2_IRQHandler()
{
	/* Check if update interrupt flag is indeed set */
	if ((TIM2->SR & TIM_SR_UIF) != 0)
	{
		trace_printf("\n*** Overflow! ***\n");

		/* Clear update interrupt flag */
		TIM2->SR &= ~(TIM_SR_UIF);

		/* Restart stopped timer */
		TIM2->CR1 |= TIM_CR1_CEN;
	}
}

void EXTI0_1_IRQHandler()
{
	/* Check if EXTI0 interrupt pending flag is set */

	if ((EXTI->PR & EXTI_PR_PR0) != 0)
		{
			/* Flip masking interrupts for EXTI2 and EXTI3 */
			EXTI->IMR ^= (EXTI_IMR_MR2 | EXTI_IMR_MR3);

			// 0 = PB2, 1 = PB3
			if(active_source == 0) { active_source = 1; }
			else { active_source = 0; }

			/* Clear flag */
			EXTI->PR = EXTI_PR_PR0;
		}
}

/* This handler is declared in system/src/cmsis/vectors_stm32f051x8.c */
void EXTI2_3_IRQHandler()
{

	if ((EXTI->PR & EXTI_PR_PR2) != 0 )
	{
		if (first_edge_chan[0])
		{
			TIM2->CNT = 0;
			TIM2->CR1 |= TIM_CR1_CEN;
			first_edge_chan[0] = 0;
		}
		else
		{
			TIM2->CR1 &= ~TIM_CR1_CEN;
			uint32_t ticks = TIM2->CNT;
			if (ticks == 0) ticks = 1;

			measured_freq_chan[0] = (float)SystemCoreClock / (float)ticks;
			first_edge_chan[0] = 1;
		}

		EXTI->PR = EXTI_PR_PR2;
	}

	// PB3 -> index 1
	if ((EXTI->PR & EXTI_PR_PR3) != 0 )
	{
		if (first_edge_chan[1])
		{
			TIM2->CNT = 0;
			TIM2->CR1 |= TIM_CR1_CEN;
			first_edge_chan[1] = 0;

		}
		else
		{
			TIM2->CR1 &= ~TIM_CR1_CEN;
			uint32_t ticks = TIM2->CNT;
			if (ticks == 0) ticks = 1;

			measured_freq_chan[1] = (float)SystemCoreClock / (float)ticks;
			first_edge_chan[1] = 1;
		}

		EXTI->PR = EXTI_PR_PR3;
	}

}

//
// LED Display Functions
//


void refresh_OLED(float freq, float ohms)
{
    unsigned char Buffer[17];
    uint8_t start_col = 2;

    // ---- PAGE 0: Resistance ----
    oled_Write_Cmd(0xB0 | 0);   // Page 0
    oled_Write_Cmd(start_col & 0x0F);
    oled_Write_Cmd(0x10);

    snprintf(Buffer, sizeof(Buffer), "R:%6.1f ", ohms);

    for (int i = 0; Buffer[i] != 0; i++)
    {
        unsigned char c = Buffer[i];
        for (int col = 0; col < 8; col++)
            oled_Write_Data(Characters[c][col]);
    }

    // ---- PAGE 2: Frequency ----
    oled_Write_Cmd(0xB0 | 2);   // Page 2 (skip one row for spacing)
    oled_Write_Cmd(start_col & 0x0F);
    oled_Write_Cmd(0x10);

    snprintf(Buffer, sizeof(Buffer), "F:%6.1fHz", freq);

    for (int i = 0; Buffer[i] != 0; i++)
    {
        unsigned char c = Buffer[i];
        for (int col = 0; col < 8; col++)
            oled_Write_Data(Characters[c][col]);
    }
}


void oled_Write_Cmd(uint8_t data)
{
    GPIOB->BSRR = GPIO_BSRR_BR_9;  // DC = 0
    GPIOB->BSRR = GPIO_BSRR_BR_8;  // CS = 0
    HAL_SPI_Transmit(&SPI_Handle, &data, 1, HAL_MAX_DELAY);
    GPIOB->BSRR = GPIO_BSRR_BS_8;  // CS = 1
}

void oled_Write_Data(uint8_t data)
{
    GPIOB->BSRR = GPIO_BSRR_BS_9;  // DC = 1
    GPIOB->BSRR = GPIO_BSRR_BR_8;  // CS = 0
    HAL_SPI_Transmit(&SPI_Handle, &data, 1, HAL_MAX_DELAY);
    GPIOB->BSRR = GPIO_BSRR_BS_8;  // CS = 1
}


void oled_config( void )
{

// Don't forget to enable GPIOB clock in RCC
// Don't forget to configure PB13/PB15 as AF0
// Don't forget to enable SPI2 clock in RCC


//
// Initialize the SPI interface
//
    HAL_SPI_Init( &SPI_Handle );

//
// Enable the SPI
//
    __HAL_SPI_ENABLE( &SPI_Handle );


    /* Reset LED Display (RES# = PB11):
       - make pin PB11 = 0, wait for a few ms
       - make pin PB11 = 1, wait for a few ms
    */

    // Reset OLED using PB11
    GPIOB->BSRR = GPIO_BSRR_BR_11;  // RES = 0
    for (volatile int i = 0; i < 50000; i++);

    GPIOB->BSRR = GPIO_BSRR_BS_11;  // RES = 1
    for (volatile int i = 0; i < 50000; i++);


//
// Send initialization commands to LED Display
//
    for ( unsigned int i = 0; i < sizeof( oled_init_cmds ); i++ )
    {
        oled_Write_Cmd( oled_init_cmds[i] );
    }

    oled_Write_Cmd(0xAF);


    for (int page = 0; page < 8; page++)
    {
        oled_Write_Cmd(0xB0 | page);   // Set page
        oled_Write_Cmd(0x00);          // Lower column
        oled_Write_Cmd(0x10);          // Upper column

        for (int i = 0; i < 128; i++)
            oled_Write_Data(0x00);
    }

    /* Fill LED Display data memory (GDDRAM) with zeros:
       - for each PAGE = 0, 1, ..., 7
           set starting SEG = 0
           call oled_Write_Data( 0x00 ) 128 times
    */




}


#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
