#include "cmd_interp.h"

/*Code module from AVR command shell project.
See appropriate header file for detais.*/

uint8_t* arg_ptr[MAX_ARGS_NUM];
uint8_t args_num;

//Splitting arguments with zeroes and putting their start
//addresses to array / storing their total number
void split_args(uint8_t* cmdline)
{
    uint8_t i;

    args_num=0;
    i=0;

    while ((i<MAX_CMD_LEN) && (cmdline[i]!=0) && (args_num<MAX_ARGS_NUM))
    {
        i++;

        if ((is_regular_char(cmdline[i])) && (cmdline[i-1]==' '))
        {
            arg_ptr[args_num]=&(cmdline[i]);
            args_num++;
        }
    }

    for (i=0; i<MAX_CMD_LEN; i++)
    {
        if (cmdline[i]==' ')
            cmdline[i]=0;
    }
}

//Extract and start appropriate fn
uint8_t cmd_exec(uint8_t *cmdline)
{
    uint8_t i=0;

    split_args(cmdline);

    while (!(str_equal_pgm(cmdline,(uint8_t *)pgm_read_word(&(sys_func_names[i])))))
    {
        i++;

        if (i>=FUNC_NUM)
        {
            return 0;
        }
    }

    ((void (*)(uint8_t**,uint8_t))pgm_read_word(&(sys_func[i])))(arg_ptr,args_num);

	return 1;
}
