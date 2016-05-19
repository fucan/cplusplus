#include "myconio.h"
#include <string.h>
#include "dxyh.h"
#include "login.h"
#include "record.h"
#include "error.h"

static void init_login_backg(void);

/*
 * initftp ���� ��Ĭ��ֵ��ʼ��ftp
 */
void initftp(void)
{
	ftp_recordflg	= FALSE;	// Ĭ�Ϲر���־
	ftp_debugflg	= FALSE;	// Ĭ�ϲ���ʾ������Ϣ
	ftp_runningflg	= TRUE;		// TRUE��ʾftp�ѿ�ʼ����
	logfile[0] = '\0';			// �����־�ļ���
	strcpy(ip_addr, "127.0.0.1"); // ��ʼ��Զ��ftp��������IP��ַ
	port = SERV_PORT;			// Զ��ftp�������˿�
}

/*
 *  config ���� ����ftp������
 */
Status config(void)
{
	char	str[MAX_CHAR_LINE+3], *p = NULL;
	int		ch;

	window(1, 1, 80, 25);
	textattr(LIGHTGRAY);
	clrscr();
	usr_cprintf("Do you want to load configure file?(y/n): ");
	ch = usr_getche();	// ��������һ���ַ�����������
	if (KEY_ENTER == ch || 'y' == ch || 'Y' == ch) {
		usr_cprintf("\n\r\nSpecify the file[default:\"ftp-configure.ini\"]: ");
		p = usr_cgetline(str, MAX_CHAR_LINE);
			// �������ļ���ʼ��ftp�ͻ���
		load_configure('\0' == *p ? "ftp-configure.ini" : p);
	}
	else {
		usr_cprintf("\n\rInput IP(such as: 192.168.0.1) : ");
		p = usr_cgetline(str, MAX_CHAR_LINE);
		strcpy(ip_addr, p);		// ����Զ�̷�����IP��ַ
		usr_cprintf("Input port: ");
		usr_cscanf("%u", &port);// ����Զ�̷�����ftp�˿�
		usr_cprintf("Turn into debug mode?(y/n) : ");
		ch = usr_getche();
		if ('\n' == ch || 'y' == ch || 'Y' == ch)
			ftp_debugflg = TRUE;// �򿪵���ģʽ
		usr_cprintf("\n\rEnable logfile?(y/n) : ");
		ch = usr_getche();		// ����־ģʽ
		if ('\n' == ch || 'y' == ch || 'Y' == ch) {
			ftp_recordflg = TRUE;
			usr_cprintf("\n\rPlease specify the logfile name: ");
			p = usr_cgetline(str, MAX_CHAR_LINE);	// ������־�ļ���
			strcpy(logfile, p);
		}
	}
		// ����һЩ������Ϣ
	if (ftp_debugflg)
		usr_cprintf("\r\n\n");
	FTP_DEBUG(__FILE__, __LINE__, "IP is: %s\n\r", ip_addr);
	FTP_DEBUG(__FILE__, __LINE__, "port = %u\n\r", port);
	FTP_DEBUG(__FILE__, __LINE__, "tabsize = %i\n\r", tabstop);
		// ��Ϊ��־ģʽ�����־�ļ�
	if (ftp_recordflg) {
		FTP_DEBUG(__FILE__, __LINE__, "Record mode is on, logfile is: %s\n\r", logfile);
		logfp = log_open(logfile, LOG_O_DEFAULT);
			// д��һЩ��Ϣ����־�ļ�
		log_msg(logfp, LVL_INFO, "Remote IP is: %s", ip_addr);
		log_msg(logfp, LVL_INFO, "Port is: %u", port);
	}
	usr_cprintfcolorxy(25, 15, LIGHTRED, "Press any key to continue...");
	usr_getch();
	return FTP_OK;
}

/*
 * get_word_form_str ���� ���ַ�������ȡһ������
 */
Status get_word_form_str(char *str, char line[], int maxlen)
{
	const char	*p = NULL;
	char		*q = NULL;
	int			counter;

	if (NULL == str || NULL == line || maxlen <= 0)
		return FTP_ERR;
		// ����ǰ���ո���Ʊ��
	p = skip_blanks(str);
	q = line;
	counter = 0;
	while (*p != ' ' && *p != '\n' && *p != '\t' && *p != '\0') {
		if (++counter >= maxlen)
			break;
		*q++ = *p++;
	}
	*q = '\0';
	return FTP_OK;
}

/*
 * load_configure ���� �������ļ���ʼ����ftp�ͻ���
 */
Status load_configure(const char *configure_file)
{
	FILE	*fp = NULL;
	char	line[MAXLINE], str[MAX_CHAR_LINE],
			*p = NULL, *q = NULL;
	int		index;
	bool	quotation_markflg;
	
		// �������ļ�
	fp = fopen(configure_file, "r");
	if (NULL == fp) {
		std_err_msg("Open configure file error.");
		return FTP_ERR;
	}
	/* 
	 * ��#��ͷ����Ϊע���С�
	 * ������ı�׼��ʽ�����磺[REMOTE = 10.6.173.225]��
	 * ���е�10.6.173.225��Ҳ���Ǿ������ݣ�Ҳ������˫������������
	 *������������ļ���
	 */
	while (fgets(line, sizeof(line), fp) != NULL) {
		p = skip_blanks(line);
		if ('#' == *p || '\n' == *p)
			continue;
		if (*p != '[') {
			std_err_msg("Syntax error occurred in configure file: Missing '['.");
			return FTP_ERR;
		}
		get_word_form_str(++p, line, sizeof(line));
		q = p + strlen(line);
		p = skip_blanks(q);
		if (*p != '=') {
			std_err_msg("Missing operator '=', -_-!");
			return FTP_ERR;
		}
		q = ++p;
		p = skip_blanks(q);
		quotation_markflg = FALSE;
		if ('\"' == *p) {
			quotation_markflg = TRUE;
			++p;
		}
		index = 0;
		while (*p != '\n' && *p != '\0' 
				&& ((quotation_markflg && *p != '\"') || (!quotation_markflg && *p != ']'))) {
			if (0 == strncmp(line, "REMOTE", 6))
				ip_addr[index++] = *p++;
			else {
				if ((0 == strncmp(line, "RECORD", 6)) &&
					((',' == *p) || (' ' == *p)))
					break;
				str[index++] = *p++;
			}
		}
		if (0 == strncmp(line, "REMOTE", 6))
			ip_addr[index] = '\0';
		else {
			str[index] = '\0';
			if ((0 == strncmp(line, "DEBUG", 5)) &&
				(0 == stricmp(str, "yes") || (0 == stricmp(str, "ON"))))
				ftp_debugflg = TRUE;
			else if (0 == strncmp(line, "PORT", 4)) {
				port = atoi(str);
				if (port <= 0)
					port = SERV_PORT;
			}
			else if (0 == strncmp(line, "TABSIZE", 7)) {
				tabstop = atoi(str);
				if (tabstop <= 0)
					tabstop = TABSIZE;
			}
			else if ((0 == strncmp(line, "RECORD", 6)) &&
				(0 == stricmp(str, "yes") || (0 == stricmp(str, "ON")))) {
				ftp_recordflg = TRUE;
				q = p;
				if (NULL == (p = strstr(q, "LOGFILE"))) {
					err_msg("Missing referance \"LOGFILE\".");
					break;
				}
				q = p + 7;
				p = skip_blanks(q);
				if (*p != '=') {
					std_err_msg("Missing operator '=', -_-!");
					return FTP_ERR;
				}
				q = ++p;
				p = skip_blanks(q);
				quotation_markflg = FALSE;
				if ('\"' == *p) {
					quotation_markflg = TRUE;
					++p;
				}
				index = 0;
				while (*p != '\n' && *p != '\0'
					&& ((quotation_markflg && *p != '\"') || (!quotation_markflg && *p != ']'))) {
					logfile[index++] = *p++;
				}
				logfile[index] = '\0';
			}
		}
			// �����ڲ��������﷨�����򱨸�һ�£�Ȼ�����
		if ((quotation_markflg && (*p != '\"' || *(p+1) != ']')) ||
			(!quotation_markflg && *p != ']' && *p != ',')) {
			FTP_DEBUG_MSG("Syntax error occurred in configure file: Missing '\"' or ']'.");
			FTP_DEBUG_MSG("You can still go on...\n\r");
		}
	}
	Fclose(fp);
	return 0;
}

/*
 * init_login_backg ���� ��ʼ����¼����
 */
static void init_login_backg(void)
{
	int		x, y;

	clrscr();
	textcolor(LIGHTGREEN);
	window(3, 4, 50, 9);
	usr_cprintf("*** *** ***   ** *   *** *** *   * ***\r\n");
	usr_cprintf("*    *  *  * *   *    *  *   **  *  * \r\n");
	usr_cprintf("**   *  ***  *   *    *  **  * * *  * \r\n");
	usr_cprintf("*    *  *    *   *    *  *   *  **  * \r\n");
	usr_cprintf("*    *  *     **  ** *** *** *   *  * \r\n");
	textcolor(WHITE);
	window(44, 3, 79, 24);
	usr_cprintf("Note: \r\n");
	usr_cprintf("\n  This is a simple ftp client tool,\r\n");
	usr_cprintf("there are sth you can do: \r\n");
	usr_cprintf("\n   1. login\r\n");
	usr_cprintf("\n   2. config\r\n");
	usr_cprintf("\n   3. do ftp stuff\r\n");
	usr_cprintf("\n   4. type '!' to enter shell\r\n");
	usr_cprintf("\n  Hope to be useful...\r\n");
	usr_cprintf("\n\n\n  Author: dengxiayehu\r\n");
	usr_cprintf("  Mail: dengxiayehu@yeah.net\r\n");
	usr_cprintf("  Date: 2010 - 11, at JUST\r\n");
	window(1, 1, 80, 25);
	draw_vertical_line(43, 2, 24, RED, 179);
	box(4, 14, 36, 21, YELLOW+(BLUE<<4), TRUE);
	textcolor(YELLOW);
	usr_cprintfxy(1, 25, "Version: 1.1");
	textbackground(BLUE);
	window(5, 15, 35, 20);
	clrscr();
	usr_cprintfxy(3, 2, "User:  ");
	wherexy(&x, &y);
	usr_cprintfcolor(LIGHTGRAY<<4, "                ");
	draw_horizen_line(x, y+1, x+15, YELLOW+(BLUE<<4), 196);
	usr_cprintfxy(3, 5, "Pass:  ");
	wherexy(&x, &y);
	usr_cprintfcolor(LIGHTGRAY<<4, "                ");
	draw_horizen_line(x, y+1, x+15, YELLOW+(BLUE<<4), 196);
	textattr(BLACK+(LIGHTGRAY<<4));
}

/*
 * login ���� �����û���¼
 */
Status login(void)
{
	char	user[USER_LEN], passwd[PASSWD_LEN],
			str[MAX_CHAR_LINE+3];
	int		counter;

		// ��Ĭ��ֵ��ʼ��ftp
	initftp();
		// ��ʼ����¼����
	init_login_backg();
	counter = 0;
	while (counter++ < TRY_TIMES) { // TRY_TIMES�γ��Ե�¼���ᣬĬ��3
		gotoxy(10, 2);
		usr_cgetline(str, MAX_CHAR_LINE);	// �����û���
		strcpy(user, str+2);
		if ('#' == user[0])
			return FTP_ERR;
		str[0] = MAX_CHAR_LINE;
		input_passwdxy(10, 5, str);			// �������루��'*'����input_noechoxy�����ԣ�
		strcpy(passwd, str+2);
		if ((0 == strcmp(user, "ftpclient")) && //�û�����ftpclient�����룺123456
			(0 == strcmp(passwd, "123456")))
			return FTP_OK;
		if ('#' == passwd[0])				//����'#'ȡ����¼
			return FTP_ERR;
			// ʧ��ʱ����һ��ȷ�Ͽ򣬾���һ��
		comfirm_box(15, 8, "Error username or password!", ERROR_TYPE);
			// ��������ʱ��ԭ���Ĳ���
		usr_cprintfcolorxy(10, 2, LIGHTGRAY<<4, "                ");
		usr_cprintfcolorxy(10, 5, LIGHTGRAY<<4, "                ");
	}
	return FTP_ERR;
}

/*
 * atlast ���� �����������
 */
void atlast(void)
{
	window(1, 1, 80, 25);
	textattr(YELLOW);
	clrscr();
	usr_cprintfxy(25, 11, "3X for the using, bye..");
	closecursor();
	textattr(LIGHTGRAY);
	usr_cprintf("   \n\n\n");
	showcursor();
	log_msg(logfp, LVL_INFO, "Ftp client is shutdown!");
	if (ftp_recordflg)
		log_close(logfp);
	return;
}
