#include "cmd_shell.h"
#include "uart_text_io.h"
#include "cmd_interp.h"
#include "../main.h"


/*Code module from AVR command shell project.
See appropriate header file for detais.*/

#define nFUNC_OUT_DATA 22
#define nFUNC_OUT_PARAM 19

volatile char fCmdShell;
volatile char fMainProg;
volatile char fExplusion;
StateOiler state;
char fSaveParams = false;
char fEnter = true;
char fOutMsgMainPrg = false;
int expl_time = 0;
uint8_t* arg_ptr_tmp[1];

//System messages
uint8_t msg_cmd_err[] PROGMEM = {"Input error - unknown command.\r\n"};
uint8_t msg_con[] PROGMEM = {"> "};
uint8_t msg_exit[] PROGMEM = {"Shell Off. Working main programm."};
uint8_t msg_start[] PROGMEM = { "KosmOiler v3.0 by Kosmonavt\r\n" };
uint8_t msg_param[] PROGMEM = {"p"};


uint8_t msg_newline[] PROGMEM = {"\r\n"};

uint8_t cmdline[SHELL_INPUT_LEN];
//uint8_t index = 0;

void cmd_process_setup()
{
	TIO_Init();
	fCmdShell = 0; // ���� �����
	fMainProg = 0;

	TIO_PrintPgmMsg(msg_start);
	TIO_PrintPgmMsg(msg_con);
	cmd_exec(msg_param);
}

void cmd_process_loop()
{
    	fOutMsgMainPrg = false;

    	if (fOutData) {
    		((void (*)(uint8_t**,uint8_t))pgm_read_word(&(sys_func[nFUNC_OUT_DATA-1])))(0,0);
    		((void (*)(uint8_t**,uint8_t))pgm_read_word(&(sys_func[nFUNC_OUT_PARAM-1])))(0,0);
    		fOutData = false;
    	}
    	if (TIO_TextInput(cmdline))
    	{
    		//TIO_TextInput(cmdline);

    		//TIO_PrintPgmMsg(msg_newline);

    		if (!(cmd_exec(cmdline)))
    			TIO_PrintPgmMsg(msg_cmd_err);
    		else
    			fCmdShell = true; // ����� ���������� ���� �� ���� ������� (���� ���������)

    		//TIO_PrintPgmMsg(msg_newline);
    		cmdline[0] = 0;
    		//TIO_PrintPgmMsg(msg_con);
    	}
}
