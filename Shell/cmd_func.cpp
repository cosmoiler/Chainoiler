#include <Arduino.h>
#include "cmd_func.h"
#include "cmd_shell.h"
#include "Scottoiler.h"
#include "../main.h"

/*Code module from AVR command shell project.
See appropriate header file for detais.*/

//Command line functions and their data

uint8_t help_text[] PROGMEM = {


		  "KosmOiler shell v3.0 by Kosmonavt			\r\n\r\n\
		  Currently supports commands:					\r\n\
		  h - display this help;						\r\n\
		  p - lists parameters;							\r\n\
		  b <time in sec> - set time valve on;			\r\n\
		  ot <%> - Flap on per trip (%);				\r\n\
		  t <chain> - set trip; 						\r\n\
		  f <state>	- on/off flap 						\r\n\
		  \t state: on/off								\r\n\
		  s <state> - set state system					\r\n\
		  \t state: 0 - AUTO							\r\n\
		  \t 1 - OFF									\r\n\
		  imp <num> - set impulses per rev chain		\r\n\
		  ps <time> - set period on pump				\r\n\
		  fon? - flap on per seconds;					\r\n\
		  es - exit and save params						\r\n\
		  r - reboot MPU								\r\n"

};

// команда "r"
void(* resetFunc) (void) = 0;

// команда "h"
void print_help(uint8_t* p_arg[],uint8_t num_args)
{
	TIO_PrintPgmMsg(help_text);
}

// команда "p"
void print_params(uint8_t* p_arg[],uint8_t num_args)
{

	printf("Pump Off: %1d rev\r\n", Scottoiler::instance()->getOffWheel());

    printf("Pump On: %1d rev\r\n", Scottoiler::instance()->getOnWheel());

    printf("Pump Time On: %1d s\r\n", Scottoiler::instance()->GetOnSec());

    printf("Mode: %d\r\n", state);

    printf("Impulse's per Rev Chain: %1d\r\n", Scottoiler::instance()->GetChainImp());

    if (Scottoiler::instance()->GetState())
    	printf("Pump: %1s", "ON\r\n");
    else
    	printf("Pump: %1s", "OFF\r\n");

    printf("Pump Num On: %d\r\n", Scottoiler::instance()->GetNon());

    printf("Num Impulse's Speed: %d\r\n", sp);

    printf("Temperature:%1d C\r\n", Scottoiler::instance()->GetTemperature());

    printf("Voltage:%1d.%d V\r\n", voltage/1000, voltage%1000);

}

// function9[]
// команда "ot?"
void print_OnPumpPerRev(uint8_t* p_arg[],uint8_t num_args)
{
    printf("On Pump:\t %d %%", Scottoiler::instance()->getOnWheel());
}

//function10[]
// команда "t?"
void print_RevPerPumpStroke(uint8_t* p_arg[],uint8_t num_args)
{
    printf("Rev's Wheel:\t %dn\r\n", Scottoiler::instance()->GetAllWheel());
}

//function11[]
void print_TimeOnFlap(uint8_t* p_arg[],uint8_t num_args)
{
    printf("Flap Time On:\t %ds\r\n", Scottoiler::instance()->GetOnSec());
}

// _ot?
void get_OnPumpPerRev(uint8_t* p_arg[], uint8_t num_args)
{
	printf("_ot?%d\r\n", Scottoiler::instance()->getOnWheel());
}

void get_RevPerPumpStroke(uint8_t* p_arg[], uint8_t num_args)
{
	printf("_t?%d\r\n", Scottoiler::instance()->GetAllWheel());
}

void get_TimeOnFlap(uint8_t* p_arg[], uint8_t num_args)
{
	printf("_fon?%d\r\n", Scottoiler::instance()->GetOnSec());
}

void get_State(uint8_t* p_arg[], uint8_t num_args)
{
	printf("_m?%d\r\n", state);
}

void get_ChainImp(uint8_t* p_arg[], uint8_t num_args)
{
	printf("_imp?%d\r\n", Scottoiler::instance()->GetChainImp());
}

void get_PumpPeriod(uint8_t* p_arg[], uint8_t num_args)
{
	printf("_ps?%d\r\n", Scottoiler::instance()->GetPumpPeriod());
}

void get_Param(uint8_t* p_arg[], uint8_t num_args)
{
	//uint8_t pumpPer = 60/Scottoiler::instance()->GetPumpPeriod();

	printf("_p?%d %d %d %d %d %d\r\n",
								Scottoiler::instance()->getOffWheel(),
								Scottoiler::instance()->getOnWheel(),
								Scottoiler::instance()->GetOnSec(),
								Scottoiler::instance()->GetChainImp(),
								state,
								Scottoiler::instance()->GetPumpPeriod());
}

void get_onlineParam(uint8_t* p_arg[], uint8_t num_args)
{
	printf("_on?%d.%01d %d %d ",
								voltage/1000, voltage%1000,
								Scottoiler::instance()->GetTemperature(),
								sp
								//6,//Scottoiler::instance()->getOnWheel(),
								);
								//,
								//
								//Scottoiler::instance()->getOffWheel());
	printf("%d ", Scottoiler::instance()->GetNon());
	printf("%d ", Scottoiler::instance()->getNOnWheel());
	printf("%d ", Scottoiler::instance()->getNOffWheel());
	printf("%d\r\n", Scottoiler::instance()->GetMode());
}

uint8_t valvecmd_on[] PROGMEM = {"on"};
uint8_t valvecmd_off[] PROGMEM = {"off"};

uint8_t valvemsg_ok[] PROGMEM = {"VALVE OK\r\n"};

uint8_t zummercmd_on[] PROGMEM = {"on"};
uint8_t zummercmd_off[] PROGMEM = {"off"};

uint8_t msg_ok[] PROGMEM = {"OK\r\n"};
uint8_t msg_err_noparam[] PROGMEM = {"Error - too few parameters.\r\n"};
uint8_t msg_err_unknown[] PROGMEM = {"Error - unknown parameter.\r\n"};

volatile uint8_t ledtype;

// команда "f <state>"
void handle_valve(uint8_t* p_arg[],uint8_t num_args)
{
	if (num_args > 0)
	{

		if (str_equal_pgm(p_arg[0], valvecmd_on))
		{
			Scottoiler::instance()->On();//
			TIO_PrintPgmMsg(valvemsg_ok);

			return;
		}

		if (str_equal_pgm(p_arg[0], valvecmd_off))
		{
			Scottoiler::instance()->Off();
			TIO_PrintPgmMsg(valvemsg_ok);

			return;
		}

		TIO_PrintPgmMsg(msg_err_unknown);
	} else
		TIO_PrintPgmMsg(msg_err_noparam);
}

void set_blobs(uint8_t* p_arg[],uint8_t num_args)
{
	uint16_t sec;

	if (num_args > 0)
	{
		sec = str_to_uint16(p_arg[0]);
		Scottoiler::instance()->SetOnSec(sec);
		TIO_PrintPgmMsg(msg_ok);
	} else
		TIO_PrintPgmMsg(msg_err_noparam);
}

void set_onwheel(uint8_t* p_arg[],uint8_t num_args)
{
	uint16_t on_wheel;

	if (num_args > 0)
	{
		on_wheel = str_to_uint16(p_arg[0]);
		//Scott.SetOn();
		Scottoiler::instance()->SetOnWheel(on_wheel);
		TIO_PrintPgmMsg(msg_ok);
	} else
		TIO_PrintPgmMsg(msg_err_noparam);
}

void set_offwheel(uint8_t* p_arg[],uint8_t num_args)
{
	uint16_t all_wheel;

	if (num_args > 0)
	{
		all_wheel = str_to_uint16(p_arg[0]);
		Scottoiler::instance()->SetOffWheel(all_wheel);
		TIO_PrintPgmMsg(msg_ok);
	} else
		TIO_PrintPgmMsg(msg_err_noparam);
}

void set_state(uint8_t* p_arg[], uint8_t num_args)
{
	if (num_args > 0)
	{
		state = (StateOiler) str_to_uint16(p_arg[0]);
		TIO_PrintPgmMsg(msg_ok);
	}
	else
		TIO_PrintPgmMsg(msg_err_noparam);
}

void set_ChainImp(uint8_t* p_arg[], uint8_t num_args)
{
	if (num_args > 0)
	{

		Scottoiler::instance()->SetChainImp(str_to_uint16(p_arg[0]));
	}
	else
		TIO_PrintPgmMsg(msg_err_noparam);
}

void set_PumpPeriod(uint8_t* p_arg[], uint8_t num_args)
{
	uint8_t pumpPer;

	if (num_args > 0)
	{
		pumpPer = str_to_uint16(p_arg[0]);
		Scottoiler::instance()->SetPumpPeriod(pumpPer);
	}
	else
		TIO_PrintPgmMsg(msg_err_noparam);
}

void  cmd_exit(uint8_t* p_arg[],uint8_t num_args)
{
	fMainProg = 1;
	fSaveParams = 1;
}

void  cmd_reboot(uint8_t* p_arg[],uint8_t num_args)
{
	resetFunc();
}

//Function table

void (*sys_func[])(uint8_t* p_arg[],uint8_t num_args) PROGMEM = {

/*1*/   print_help,
/*2*/   print_params,
/*3*/	set_blobs,
/*4*/	set_onwheel,
/*5*/	set_offwheel,
/*6*/	handle_valve,
/*7*/	cmd_exit,
/*8*/	cmd_reboot,
/*9*/	print_OnPumpPerRev,
/*10*/	print_RevPerPumpStroke,
/*11*/	print_TimeOnFlap,
/*12*/  set_state,
/*13*/	get_OnPumpPerRev,
/*14*/	get_RevPerPumpStroke,
/*15*/	get_TimeOnFlap,
/*16*/	get_State,
/*17*/	set_ChainImp,
/*18*/	get_ChainImp,
/*19*/	get_Param,
/*20*/	set_PumpPeriod,
/*21*/	get_PumpPeriod,
/*22*/	get_onlineParam

};

//Command line alias table

uint8_t funcname1[] PROGMEM = {"h"};
uint8_t funcname2[] PROGMEM = {"p"};
uint8_t funcname19[] PROGMEM = {"_p?"};
uint8_t funcname3[] PROGMEM = {"b"};
uint8_t funcname4[] PROGMEM = {"ot"};
uint8_t funcname9[] PROGMEM = {"ot?"};
uint8_t funcname13[] PROGMEM = {"_ot?"};
uint8_t funcname5[] PROGMEM = {"t"};
uint8_t funcname10[] PROGMEM = {"t?"};
uint8_t funcname14[] PROGMEM = {"_t?"};
uint8_t funcname6[] PROGMEM = {"f"};
uint8_t funcname11[] PROGMEM = {"fon?"};
uint8_t funcname15[] PROGMEM = {"_fon?"};
uint8_t funcname16[] PROGMEM = {"_m?"};
uint8_t funcname12[] PROGMEM = {"m"};
uint8_t funcname17[] PROGMEM = {"imp"};
uint8_t funcname18[] PROGMEM = {"_imp?"};
uint8_t funcname20[] PROGMEM = {"ps"};
uint8_t funcname21[] PROGMEM = {"_ps?"};
uint8_t funcname7[] PROGMEM = {"es"};
uint8_t funcname8[] PROGMEM = {"r"};
uint8_t funcname22[] PROGMEM = {"_on?"};

uint8_t *sys_func_names[] PROGMEM = {

    funcname1,
    funcname2,
	funcname3,
	funcname4,
	funcname5,
	funcname6,
	funcname7,
	funcname8,
	funcname9,
	funcname10,
	funcname11,
	funcname12,
	funcname13,
	funcname14,
	funcname15,
	funcname16,
	funcname17,
	funcname18,
	funcname19,
	funcname20,
	funcname21,
	funcname22
};
