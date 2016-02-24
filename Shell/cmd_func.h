#ifndef _CMD_FUNC_H_
#define _CMD_FUNC_H_

/*
This module contains user-defined command handling functions
and command/function tables. Customize source to your needs.

Written by YS from radiokot.ru and we.easyelectronics.ru

You may freely use and redistribute this code any way you want,
but only at your own risk. Author gives no warranty and takes no 
responsibility on anything that could happen while using this code.
*/

#include <stdint.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "cmd_util.h"
#include "uart_text_io.h"

//Total functions number
#define FUNC_NUM        22

#ifdef __cplusplus
extern "C" {
#endif

//Function and command tables
extern void (*sys_func[])(uint8_t* p_arg[],uint8_t num_args);
extern uint8_t* sys_func_names[];

#ifdef __cplusplus
}
#endif

#endif
