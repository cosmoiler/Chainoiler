#ifndef _UART_TIO_H_
#define _UART_TIO_H_

/*
Text I/O module. Moves data through UART @ 9600BPS/8N1.

Written by YS from radiokot.ru and we.easyelectronics.ru

You may freely use and redistribute this code any way you want,
but only at your own risk. Author gives no warranty and takes no 
responsibility on anything that could happen while using this code.
*/
#include <stdio.h>
#include <avr/io.h>
#include <stdint.h>
#include <avr/pgmspace.h>

#include "cmd_shell.h"

//Maximum input text length
#define MAX_TXT_LEN			SHELL_INPUT_LEN

//Control characers
#define CHR_BS				0x08
#define CHR_ENTER			0x0D
typedef uint8_t boolean;
#define true 0x1
#define false 0x0

//UART initialization
void TIO_Init(void);
//Char I/O
int TIO_CharInput(void);
void TIO_CharOutput(uint8_t ch);
//Text I/O. Input supports backspace.
void TIO_TextOutput(char *outtxt);
boolean TIO_TextInput(uint8_t *intxt);
//Sends string from FLASH.
void TIO_PrintPgmMsg(uint8_t* pgm_msg);

int uart_putc(  char c, FILE *file );
int uart_getc( FILE* file );

#endif
