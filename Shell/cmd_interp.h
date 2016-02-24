#ifndef _CMD_INTERP_H_
#define _CMD_INTERP_H_

/*
Command interpreter module.

Written by YS from radiokot.ru and we.easyelectronics.ru

You may freely use and redistribute this code any way you want,
but only at your own risk. Author gives no warranty and takes no 
responsibility on anything that could happen while using this code.
*/

#include <stdint.h>
#include "cmd_func.h"
#include "cmd_util.h"
#include "cmd_shell.h"

#define MAX_CMD_LEN         SHELL_INPUT_LEN
#define MAX_ARGS_NUM        SHELL_ARGS_MAX

#ifdef __cplusplus
extern "C" {
#endif

//Command interpreting function.
//Takes command line and executes appropriate handler.
uint8_t cmd_exec(uint8_t* cmdline);

#ifdef __cplusplus
}
#endif

#endif
