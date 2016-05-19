#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <Winsock2.h>
#include "myconio.h"
#include "error.h"

/*
 * std_err_sys ���� ���������Ϣ������ϵͳ�Ĵ�����Ϣ��
 */
void std_err_sys(const char *cause, ...)
{
	va_list ap;

	va_start(ap, cause);
	err_handle(1, cause, ap);
	va_end(ap);
	return;
} /* end std_err_sys */

/*
 * std_err_sys_q ���� ���������Ϣ������ϵͳ�Ĵ�����Ϣ�������˳�
 */
void std_err_sys_q(const char *cause, ...)
{
	va_list ap;

	va_start(ap, cause);
	err_handle(1, cause, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
} /* end std_err_sys_q */

/*
 * std_err_msg ���� ���������Ϣ
 */
void std_err_msg(const char *cause, ...)
{
	va_list ap;

	va_start(ap, cause);
	err_handle(0, cause, ap);
	va_end(ap);
	return;
}  /* end std_err_msg */

/*
 * std_err_msg_q ���� ���������Ϣ�����˳�
 */
void std_err_msg_q(const char *cause, ...)
{
	va_list ap;

	va_start(ap, cause);
	err_handle(0, cause, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}  /* end std_err_msg_q */

/*
 * sock_err_sys ���� ���winsock�Ĵ�����Ϣ������ϵͳ�Ĵ�����Ϣ��
 */
void sock_err_sys(const char *cause, ...)
{
	va_list ap;

	va_start(ap, cause);
	err_handle(2, cause, ap);
	va_end(ap);
	return;
}  /* end sock_err_sys */

/*
 * sock_err_sys_q ���� ���winsock�Ĵ�����Ϣ������ϵͳ�Ĵ�����Ϣ�������˳�
 */
void sock_err_sys_q(const char *cause, ...)
{
	va_list ap;

	va_start(ap, cause);
	err_handle(2, cause, ap);
	va_end(ap);
	WSACleanup();
	exit(EXIT_FAILURE);
}  /* end sock_err_sys_q */

/*
 * win_err_sys ���� ���win-sdk�Ĵ�����Ϣ������ϵͳ�Ĵ�����Ϣ��
 */
void win_err_sys(const char *cause, ...)
{
	va_list ap;

	va_start(ap, cause);
	err_handle(3, cause, ap);
	va_end(ap);
	return;
}  /* end win_err_sys */

/*
 * win_err_sys_q ���� ���win-sdk�Ĵ�����Ϣ������ϵͳ�Ĵ�����Ϣ�������˳�
 */
void win_err_sys_q(const char *cause, ...)
{
	va_list ap;

	va_start(ap, cause);
	err_handle(3, cause, ap);
	va_end(ap);
	exit(EXIT_FAILURE);
}  /* end win_err_sys_q */

/*
 * err_handle ���� ��Ҫ�Ĵ�������
 */
void err_handle(int errnoflg, const char *fmt, va_list ap)
{
	int		errno_save, n;
	char	line[MAXLINE], *msgbuff = NULL;

	vsprintf(line, fmt, ap);
	if (1 == errnoflg) {
		n = strlen(line);
		errno_save = errno;
		sprintf(line+n, ": %s", strerror(errno));
		errno = errno_save;
	} else if (errnoflg > 1){
		strcat(line, ": ");
		n = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | 40, NULL, 
			2 == errnoflg ? WSAGetLastError() : GetLastError(), 
			MAKELANGID(0, SUBLANG_ENGLISH_US), (LPTSTR) &msgbuff, MAXLINE, NULL);
		strcat(line, msgbuff);
		if (n > 0)
			LocalFree((HLOCAL) msgbuff);
	}
	strcat(line, "\n\r");
	fflush(stdout);
	usr_cprintf(line, stderr);
	fflush(stderr);
	return;
} /* end of err_handle */
