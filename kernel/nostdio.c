#include "type.h"
#include "proc.h"
#include "proto.h"
#include "protect.h"
#include "string.h"
#include "const.h"


int printf(const char *fmt,...);
int vsprintf(char *buf,const char *fmt,va_list args);
int sprintf(char *buf, const char *fmt, ...);

int printf(const char *fmt,...)
{

	int i;
	char buf[256];
	va_list arg=(va_list) ((char *)(&fmt) + 4);
	i=vsprintf(buf,fmt,arg);
	buf[i]=0;
	printx(buf);
	return i;
}

/*======================================================================*
                                i2a
 *======================================================================*/
PRIVATE char* i2a(int val, int base, char ** ps)
{
	int m = val % base;
	int q = val / base;
	if (q) {
		i2a(q, base, ps);
	}
	*(*ps)++ = (m < 10) ? (m + '0') : (m - 10 + 'A');

	return *ps;//the return value is not the result, but the result it ps
}


/*======================================================================*
                                vsprintf
 *======================================================================*/
/*
 *  为更好地理解此函数的原理，可参考 printf 的注释部分。
 */
PUBLIC int vsprintf(char *buf, const char *fmt, va_list args)
{
	char*	p;

	va_list	p_next_arg = args;
	int	m;

	char	inner_buf[1024];
	char	cs;
	int	align_nr;

	for (p=buf;*fmt;fmt++) {
		if (*fmt != '%') {//got it directly
			*p++ = *fmt;
			continue;
		}
		else {		/* a format string begins */
			align_nr = 0;
		}

		fmt++;//skip %

		if (*fmt == '%') {//the second %
			*p++ = *fmt;
			continue;
		}
		else if (*fmt == '0') {//"%0d"
			cs = '0';
			fmt++;
		}
		else {
			cs = ' ';
		}
		while (((unsigned char)(*fmt) >= '0') && ((unsigned char)(*fmt) <= '9')) {//%12d:align
			align_nr *= 10;
			align_nr += *fmt - '0';
			fmt++;
		}

		char * q = inner_buf;
		memset(q, 0, sizeof(inner_buf));//initial

		switch (*fmt) {
		case 'c':
			*q++ = *((char*)p_next_arg);//p_next_arg is a pointer
			p_next_arg += 4;
			break;
		case 'x':
			m = *((int*)p_next_arg);
			i2a(m, 16, &q);//16 jingzhi 
			p_next_arg += 4;
			break;
		case 'd':
			m = *((int*)p_next_arg);
			if (m < 0) {
				m = m * (-1);
				*q++ = '-';
			}
			i2a(m, 10, &q);
			p_next_arg += 4;
			break;
		case 's':
			strcpy(q, *((char **)p_next_arg));
			q += strlen(*((char **)p_next_arg));
			p_next_arg += 4;//still +4
			break;
		default:
			break;
		}

		int k;
		int blanknum=align_nr-strlen(inner_buf);
		if(blanknum >0){
			
		}
		else
			blanknum=0;

		for (k = 0; k < blanknum ; k++) {
			*p++ = cs;//pad the blank
		}
		q = inner_buf;
		while (*q) {
			*p++ = *q++;
		}
	}

	*p = 0;

	return (p - buf);
}


/*======================================================================*
                                 sprintf
 *======================================================================*/
int sprintf(char *buf, const char *fmt, ...)
{
	va_list arg = (va_list)((char*)(&fmt) + 4);        /* 4 是参数 fmt 所占堆栈中的大小 */
	return vsprintf(buf, fmt, arg);
}
