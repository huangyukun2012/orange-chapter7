
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            main.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "nostdio.h"
#include "msg.h"
#include "fs.h"
#include "unistd.h"
#include "err.h"

/*======================================================================*
                            tinix_main
 *======================================================================*/
PUBLIC int tinix_main()
{
	disp_str("-----\"tinix_main\" begins-----\n");

	TASK*		p_task		= task_table;
	PROCESS*	p_proc		= proc_table;
	char*		p_task_stack	= task_stack + STACK_SIZE_TOTAL;
	t_16		selector_ldt	= SELECTOR_LDT_FIRST;

	int i;
	t_8 privilege;
	t_8 rpl;
	int eflags;
	int prio;

	for(i=0;i<NR_PROCS;i++){
		if(i<NR_TASKS){
			p_task=task_table+i;
			privilege=PRIVILEGE_TASK;
			rpl=RPL_TASK;
			eflags=0x1202;
			prio=15;
		}
		else{
			p_task=user_proc_table+i-NR_TASKS;
			privilege=PRIVILEGE_USER;
			rpl=RPL_USER;
			eflags=0x0202;
			prio=5;
		}
		strcpy(p_proc->name, p_task->name);	// name of the process
		p_proc->pid	= i;			// pid

		p_proc->ldt_sel	= selector_ldt;
		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | privilege << 5;	// change the DPL
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3], sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;	// change the DPL
		p_proc->regs.cs		= ((8 * 0) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ds		= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.es		= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.fs		= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ss		= ((8 * 1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.gs		= (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;
		p_proc->regs.eip	= (t_32)p_task->initial_eip;
		p_proc->regs.esp	= (t_32)p_task_stack;
		p_proc->regs.eflags	= eflags ;	// IF=1, IOPL=1, bit 2 is always 1.

	//_proc->nr_tty=0;
		p_task_stack -= p_task->stacksize;
	
		p_proc->p_flags=0;
		p_proc->p_msg=0;
		p_proc->p_recvfrom=NO_TASK;
		p_proc->p_sendto=NO_TASK;
		p_proc->has_int_msg=0;
		p_proc->q_sending=0;
		p_proc->next_sending=0;
		
		p_proc->ticks=p_proc->priority=prio;

		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;

	}

	/*proc_table[NR_TASKS+0].nr_tty=0;//A
	proc_table[NR_TASKS+1].nr_tty=1;//B
	proc_table[NR_TASKS+2].nr_tty=1;//C
	*/
	k_reenter	= 0;
	ticks		= 0;

	p_proc_ready	= proc_table;
	
	init_clock();
	init_keyboard();

	restart();

	while(1){}
}

/*======================================================================*
                               TestA
 *======================================================================*/
void TestA()
{//very important: the stdin and stdout must be open , or the printf will not work
	char tty_name[] = "/dev_tty1";
	int fd_stdin = open(tty_name, O_RDWR);
	assert(fd_stdin == 0);
	int fd_stdout = open(tty_name, O_RDWR);
	assert(fd_stdout == 1);
	
	char *filenames[]={
		"/foo", "/bar", "/test"
	};
	
	int i;
	int nr=sizeof(filenames)/sizeof(filenames[0]);


	for(i=0;i<nr;i++){
		int fd=open(filenames[i], O_CREAT | O_RDWR);
		assert(fd!=-1);
		printf("Created file : %s\n",filenames[i]);
		assert(close(fd)==0);
	}

	char *rmfilenames[]={
		"/bar", "/test", "/foo"  
	};

	nr=sizeof(rmfilenames)/sizeof(rmfilenames[0]);

	for(i=0;i<nr;i++){
		if(unlink(rmfilenames[i]) == 0){
			printf("file removed: %s\n", rmfilenames[i]);
		}
		else{
			printf("Faild removing file \"%s\"", rmfilenames[i]);
		}
	}
	spin("Test A");
	while(0){
#ifdef DEBUG
		printf("testA\n");
#endif
		int fd=open("/test",O_CREAT);
		printf("fd: %d\n",fd);
		close(fd);
		spin("TestA");
	}
}


/*======================================================================*
                               TestB
 *======================================================================*/
void TestB()
{
	int i = 0;
	char tty_name[] = "/dev_tty1";
	int fd_stdin = open(tty_name, O_RDWR);
	assert(fd_stdin == 0);
	int fd_stdout = open(tty_name, O_RDWR);
	assert(fd_stdout == 1);
	char rdbuf[128];

	while(1){
		printf("$:");
		int r = read(fd_stdin, rdbuf,70);
		rdbuf[r] = 0;
		if(strcmp(rdbuf, "hello") == 0){
			printf("hello world\n");

		}
		else{
			if(rdbuf[0]){
				printf("{%s}\n", rdbuf);
			}
		}
	}
	assert(0);//never arrive here
}


/*======================================================================*
                               TestC
 *======================================================================*/
void TestC()
{
	int i = 0;
	spin("Test C. . . \n");
	while(1){
		printf("C");
		milli_delay(10);
	}
}

int get_ticks()
{
	MESSAGE msg;
	reset_msg(&msg);
	msg.type=GET_TICKS;
	send_recv(BOTH,TASK_SYS,&msg);
	return msg.RETVAL;
}
