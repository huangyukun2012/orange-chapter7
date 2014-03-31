
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define GLOBAL_VARIABLES_HERE

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "proc.h"
#include "global.h"
#include "tty.h"
#include "console.h"

PUBLIC	PROCESS	proc_table[NR_TASKS];

PUBLIC	char	task_stack[STACK_SIZE_TOTAL];

PUBLIC	TASK	task_table[NR_TASKS] = {{task_tty, STACK_SIZE_TTY, "tty"},
					{TestA, STACK_SIZE_TESTA, "TestA"},
					{TestB, STACK_SIZE_TESTB, "TestB"},
					{TestC, STACK_SIZE_TESTC, "TestC"}};

PUBLIC	t_pf_irq_handler	irq_table[NR_IRQ];

PUBLIC	t_sys_call		sys_call_table[NR_SYS_CALL] = {sys_get_ticks};

TTY	tty_table[NR_TTY];

CONSOLE		console_table[NR_CONSOLE];
