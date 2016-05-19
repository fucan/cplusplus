#include <windows.h>
#include <stdarg.h>
#include "myconio.h"

enum cColorIndex {OPT_FOREG, OPT_BACKG, OPT_ATTR};
enum cInputStyle {NORM_INPUT_STYLE, PASSWORD_INPUT_STYLE, NOECHO_INPUT_STYLE};

static int	g_curwin_left	= 1;
static int	g_curwin_top	= 1;
static int	g_curwin_right	= 80;
static int	g_curwin_bottom	= 25;
static int	g_curwin_width	= 80;
static int	g_curwin_height	= 25;
int			tabstop;
static bool g_bshowtab		= TRUE;
static HANDLE g_console;
	/* ��ʾģʽ */
static enum text_modes g_old_screen_mode = C80;
static enum text_modes g_cur_screen_mode = C80;
	/* ����ɫ�Ƿ���� */
static bool	b_backg_high_video	= FALSE;
	/* һ�����뻺���� */
#define MAX_MY_INPUT_BUFFER_SIZE	1024
static char g_inputbuf[MAX_MY_INPUT_BUFFER_SIZE] = {0};
	/* ��ѭ������ʵ��, ��ʼ��ͷ��βΪ 0 */
static int	g_front = 0, g_rear = 0;

	/* �����ɫ���õ���ʵ�ֺ��� */
static int get_text_color_info(enum cColorIndex cGetIndex);
	/* ������ɫ���õ���Ҫʵ�ֺ��� */
static void set_text_color_info(enum cColorIndex cSetIndex, int wCharColor);
	/* ����ַ�����Ҫʵ�ֺ��� */
static int putchar_handle(int x, int y, int ch, bool bMoveCursor);
	/* �����ʽ���ַ�������Ҫʵ�ֺ��� */
static int printf_handle(int x, int y, bool bMoveCursor,
						 const char * format, va_list ap);
	/* �����ַ�������Ҫʵ�ֺ��� */
static char *gets_handle(int x, int y, bool bMoveCursor, char *pStrBuf, enum cInputStyle cStyle);
	/* ���ĳ���ַ������Ե���Ҫʵ�ֺ��� */
static int get_certain_char_attr_handle(int x, int y, bool bMoveCursor);
	/* ���ù��ģʽ����Ҫʵ�ֺ��� */
static void set_cursor_mode_handle(int size, bool bVisiable);

/*
 * initscr -- ��ʼ������
 *
 * ˵��: ���������ڳ�����ʼ����
 */
void initscr(void)
{
		/* ���ȫ�־�� */
	g_console = GetStdHandle(STD_OUTPUT_HANDLE);
	PERR(g_console != INVALID_HANDLE_VALUE, "GetStdHandle");

		/* ��ʼ��������ر��� */
	g_curwin_left	= 1;
	g_curwin_top	= 1;
	g_curwin_right	= 80;
	g_curwin_bottom	= 25;
	g_curwin_width	= 80;
	g_curwin_height	= 25;

		/* �Ʊ������Ĭ��Ϊ4 */
	tabstop		= 4;

		/* Ĭ����ʾģʽΪ 80 * 25, ��ɫ */
	g_old_screen_mode = C80;
	g_cur_screen_mode = C80;

		/* ���ô��ڼ����� buffer �Ĵ�С */
	resize_con_buf_and_window(80, 25);

		/* ������ʾҳ, Ĭ��Ϊ MS_DOS_OUTPUT_CP, ������ʾ��չ Ascii */
	set_codepage(MS_DOS_OUTPUT_CP);

		/* ���ÿ���̨���� */
	set_con_title("VC - ��FTP�ͻ���  [����Ұ��]");

		/* ��ʼ�����뻺���� */
	memset(g_inputbuf, 0, MAX_MY_INPUT_BUFFER_SIZE);
	g_front = g_rear = 0;
	return;
}


/*
 * endwin -- �������� 
 *
 * ˵��: ���������ڳ������ʱ����
 */
void endwin(void)
{
		/* Ҳûʲô, �رվ�� */
	CloseHandle(g_console);
}


/*
 * perr -- ���������Ϣ
 * @szFilename: �� __FILE__ ����
 * @line: �� __LINE__ ����
 * @szApiName: ��������ĺ�����
 * @dwError: ������, ��GetLastError()����
 */
void perr(const char *szFileName, int line, const char *szApiName, int err)
{
	TCHAR	szTemp[1024];
	DWORD	cMsgLen;
	TCHAR	*msgBuf;
	int		iButtonPressed;

		/* ��ʽ��������Ϣ */
	sprintf(szTemp, "%s: Error %d from %s on line %d:\n",
		szFileName, err, szApiName, line);
	cMsgLen = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 40, NULL, err,
		MAKELANGID(0, SUBLANG_ENGLISH_US), (LPTSTR)&msgBuf, 1024,
		NULL);
	if (cMsgLen == 0)	/* ����δ�ҵ���Ӧ�Ĵ�����Ϣ */
		sprintf(szTemp + strlen(szTemp), "Unable to obtain error message text!\n"
		"%s: Error %d from %s on line %d", __FILE__,
		GetLastError(), "FormatMessage", __LINE__);
	else
		strcat(szTemp, msgBuf);
	strcat(szTemp, "\n\nContinue execution?");
	MessageBeep(MB_ICONEXCLAMATION);
	iButtonPressed = MessageBox(NULL, TEXT(szTemp), "Console API Error",
		MB_ICONEXCLAMATION | MB_YESNO | MB_DEFBUTTON2);

	if (cMsgLen > 0)
		LocalFree((HLOCAL)msgBuf);

	if (iButtonPressed == IDNO)
		exit(EXIT_FAILURE);
	return;
}


/*
 * resize_con_buf_and_window -- resize the console buffer and window
 * @xsize
 * @ysize: specifies the size of buffer and window
 *
 * Remarks: It seems that 80 * 46 is the best(if you do not want the scroll bar)
 * xsize must be smaller than 80, otherwise an error occurs.
 * Try it!
 */
void resize_con_buf_and_window(int xsize, int ysize)
{
	CONSOLE_SCREEN_BUFFER_INFO	csbi;
	bool		bSuccess;
	SMALL_RECT	srWindowRect;
	COORD		coordScreen;

	if (xsize <= 0 || xsize > 80 ||
		ysize <= 0)
		return;

	bSuccess = GetConsoleScreenBufferInfo(g_console, &csbi);
	PERR(bSuccess, "GetConsoleScreenBufferInfo");

	coordScreen = GetLargestConsoleWindowSize(g_console);
	PERR(coordScreen.X | coordScreen.Y, "GetLargestConsoleWindowSize");

	srWindowRect.Right = (SHORT) (min(xsize, coordScreen.X) - 1);
	srWindowRect.Bottom = (SHORT) (min(ysize, coordScreen.Y) - 1);
	srWindowRect.Left = srWindowRect.Top = (SHORT) 0;
		/* ȷ�����ڻ������Ĵ�С(������ڴ��ڵĴ�С) */
	coordScreen.X = (SHORT) xsize;
	coordScreen.Y = (SHORT) ysize;
		/* ���ִ��ڻ������ȴ��ڴ�С��ʱ, �ȵ������ڴ�С, �ڵ�����������С */
	if ((DWORD) csbi.dwSize.X * csbi.dwSize.Y > (DWORD) xsize * ysize) {
		bSuccess = SetConsoleWindowInfo(g_console, TRUE, &srWindowRect);
		PERR(bSuccess, "SetConsoleWindowInfo");

		bSuccess = SetConsoleScreenBufferSize(g_console, coordScreen);
		PERR(bSuccess, "SetConsoleScreenBufferSize");
	}
		/* ���ִ��ڻ������ȴ��ڴ�ССʱ, �ȵ�����������С, �ڵ������ڴ�С */
	if ((DWORD) csbi.dwSize.X * csbi.dwSize.Y < (DWORD) xsize * ysize) {
		bSuccess = SetConsoleScreenBufferSize(g_console, coordScreen);
		PERR(bSuccess, "SetConsoleScreenBufferSize");

		bSuccess = SetConsoleWindowInfo(g_console, TRUE, &srWindowRect);
		PERR(bSuccess, "SetConsoleWindowInfo");
	}
		/* ���ô�����صĲ��� */
	g_curwin_left	= 1;		/* ��ǰ���ڵ����ϽǺ����� */
	g_curwin_top		= 1;		/* ��ǰ���ڵ����Ͻ������� */
	g_curwin_right	= xsize;	/* ��ǰ���ڵ����½Ǻ����� */
	g_curwin_bottom	= ysize;	/* ��ǰ���ڵ����½������� */
	g_curwin_width	= xsize;	/* ��ǰ���ڵĿ�� */
	g_curwin_height	= ysize;	/* ��ǰ���ڵĸ߶� */
	return;
}


/*
 * SetOutputCodePage -- ���������ʾ����ҳ, ���Դ��� 437 (MS-DOS),
 *                �����Ļ�������ʾ��չ�� ascii ��
 * @codepageID: ����ҳ����
 */
void set_codepage(unsigned int codepageID)
{
	bool	bSuccess;

	bSuccess = SetConsoleOutputCP(codepageID);
	PERR(bSuccess, "SetConsoleOutputCP");
	return;
}


/*
 * get_codepage -- ��õ�ǰ��ʾҳ�ı��(Ϊ����ʽ����������������, ֱ�ӵ���API)
 * ����ֵ: ��ǰ��ʾҳ���
 */
unsigned int get_codepage(void)
{
	return GetConsoleCP();
}


/*
 * set_con_title -- ���ÿ���̨�ı���
 * @title: ���⴮
 */
void set_con_title(const char *title)
{
	bool bSuccess;

	if (NULL == title)
		return;

	bSuccess = SetConsoleTitle(title);
	PERR(bSuccess, "SetConsoleTitle");
	return;
}


/*
 * pos_screen2window -- ����Ļ����ת��Ϊ��������
 */
void pos_screen2window(int scr_x, int scr_y, int *win_x, int *win_y)
{
	if ((win_x == NULL && win_y == NULL) ||
		scr_x <= 0 || scr_x > get_conx() ||
		scr_y <= 0 || scr_y > get_cony())
		return;

	if (win_x != NULL) *win_x = scr_x - g_curwin_left  + 1;
	if (win_y != NULL) *win_y = scr_y - g_curwin_top + 1;
	return;
}


/*
 * pos_window2screen -- ����������ת��Ϊ��Ļ����
 */
void pos_window2screen(int win_x, int win_y, int *scr_x, int *scr_y)
{
	if ((scr_x == NULL && scr_x == NULL) ||
		win_x <= 0 || win_x > g_curwin_width ||
		win_y <= 0 || win_y > g_curwin_height)
		return;

	if (scr_x != NULL) *scr_x = g_curwin_left + win_x - 1;
	if (scr_y != NULL) *scr_y = g_curwin_top + win_y - 1;
	return;
}


/*
 * Swap -- ��������, �������������͵ı���
 * @item1: ָ�򽻻���1��ָ��
 * @item2: ָ�򽻻���2��ָ��
 * @size: �����Ĵ�С(��sizeof�ؼ��ֻ��)
 */
void Swap(void *item1, void *item2, size_t size)
{
	void *vTmp;

	if (item1 == NULL || item2 == NULL)
		return;
		/* �ڶ��Ϸ���ռ����ʱ���� */
	vTmp = malloc(size);
	PERR(vTmp, "malloc");
		/* ���� */
	memcpy(vTmp, item1, size);
	memcpy(item1, item2, size);
	memcpy(item2, vTmp, size);
		/* �ͷ���ʱ���� */
	free(vTmp);
	return;
}


/*
 * gotoxy -- �ƶ����
 * @x
 * @y: Ŀ�ĵص�����ֵ(����ڵ�ǰ����ڶ���)
 *˵��: �������Ͻǵ�����Ϊ(1, 1)
 */
void 	_Cdecl gotoxy			(int x, int y)
{
	COORD	coordDest;
	bool	bSuccess;
	int		sScrDestX, sScrDestY;

		/* ����������ת��Ϊ��Ļ����(��Ϊ��, ���Ͻ�Ϊ(1, 1), ����(0, 0)) */
	pos_window2screen(x, y, &sScrDestX, &sScrDestY);
		/* ����������ȷ�Ͳ��ı�λ�� */
	if (x <= 0 || x > g_curwin_width ||
		y <= 0 || y > g_curwin_height)
		return;
		/* ת��Ϊʵ�ʵ���Ļ���� */
	coordDest.X = (SHORT) (sScrDestX - 1);
	coordDest.Y = (SHORT) (sScrDestY - 1);
	bSuccess = SetConsoleCursorPosition(g_console, coordDest);
	PERR(bSuccess, "SetConsoleCursorPosition");
}

/*
 * usr_getch ���� ����һ���ַ����������أ�
 */
#define VK_PAGEUP	0x21
#define VK_PAGEDOWN	0x22
int		_Cdecl usr_getch		(void)
{
	HANDLE	hStdIn;
	DWORD	dwInputMode;
	BOOL	bSuccess;
	int		ch1, ch2, retval;
	DWORD	dwInputEvents;
	INPUT_RECORD	inputBuf;
	WORD	wKeyCode;
	DWORD	wKeyState;

		/* �ȿ����뻺��������û���ַ�, �еĻ�ֱ����ȡ */
	if (g_front != g_rear) {
		g_front = (g_front + 1) % MAX_MY_INPUT_BUFFER_SIZE;
		retval = g_inputbuf[g_front];
		return retval;
	}

	hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	PERR(hStdIn != INVALID_HANDLE_VALUE, "GetStdHandle");
		/* ��ñ�׼����ģʽ */
	bSuccess = GetConsoleMode(hStdIn, &dwInputMode);
	PERR(bSuccess, "GetConsoleMode");
		/* ����׼��������Ϊ���ַ�����(���л���, ���ɼ�, ������ͬʱȡ��) */
	bSuccess = SetConsoleMode(hStdIn, dwInputMode & ~(ENABLE_LINE_INPUT |
		ENABLE_ECHO_INPUT | ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT | ENABLE_PROCESSED_INPUT));
	PERR(bSuccess, "SetConsoleMode");
		/* �����뻺������ȡ��һ���ַ� */

	for ( ; ; ) {
		bSuccess = ReadConsoleInput(hStdIn, &inputBuf, 1, &dwInputEvents);
		PERR(bSuccess, "ReadConsoleInput");

		if (inputBuf.Event.KeyEvent.bKeyDown && inputBuf.EventType == KEY_EVENT) {
			wKeyCode = inputBuf.Event.KeyEvent.wVirtualKeyCode;
			wKeyState = inputBuf.Event.KeyEvent.dwControlKeyState;
			if (wKeyCode == VK_SHIFT ||
				wKeyCode == VK_LSHIFT ||
				wKeyCode == VK_RSHIFT ||
				wKeyCode == VK_CONTROL ||
				wKeyCode == VK_LCONTROL ||
				wKeyCode == VK_RCONTROL ||
				wKeyCode == VK_MENU ||
				wKeyCode == VK_LMENU ||
				wKeyCode == VK_RMENU ||
				wKeyCode == VK_PAUSE ||
				wKeyCode == VK_NUMLOCK ||
				wKeyCode == VK_SCROLL ||
				wKeyCode == VK_LWIN ||
				wKeyCode == VK_RWIN ||
				wKeyCode == VK_APPS ||
				wKeyCode == VK_CAPITAL ||
				wKeyCode == VK_CLEAR ||
				wKeyCode == VK_CONVERT ||
				wKeyCode == VK_NONCONVERT ||
				wKeyCode == VK_MODECHANGE)
				continue;
			break;
		}
	}

	ch1 = inputBuf.Event.KeyEvent.uChar.AsciiChar;
	ch2 = inputBuf.Event.KeyEvent.wVirtualKeyCode;
		/* ���ж� ALT �� */
	if ((wKeyState & RIGHT_ALT_PRESSED) ||
		(wKeyState & LEFT_ALT_PRESSED)) {
		retval = 0;
		if (0 != ch1) {
			switch (ch1) {
			case 'A':
			case 'a':
				usr_ungetch(0x1E);
				break;
			case 'B':
			case 'b':
				usr_ungetch(0x30);
				break;
			case 'C':
			case 'c':
				usr_ungetch(0x2E);
				break;
			case 'D':
			case 'd':
				usr_ungetch(0x20);
				break;
			case 'E':
			case 'e':
				usr_ungetch(0x12);
				break;
			case 'F':
			case 'f':
				usr_ungetch(0x21);
				break;
			case 'G':
			case 'g':
				usr_ungetch(0x22);
				break;
			case 'H':
			case 'h':
				usr_ungetch(0x23);
				break;
			case 'I':
			case 'i':
				usr_ungetch(0x17);
				break;
			case 'M':
			case 'm':
				usr_ungetch(0x32);
				break;
			case 'N':
			case 'n':
				usr_ungetch(0x31);
				break;
			case 'O':
			case 'o':
				usr_ungetch(0x18);
				break;
			case 'P':
			case 'p':
				usr_ungetch(0x19);
				break;
			case 'Q':
			case 'q':
				usr_ungetch(0x10);
				break;
			case 'S':
			case 's':
				usr_ungetch(0x1F);
				break;
			case 'T':
			case 't':
				usr_ungetch(0x14);
				break;
			case 'V':
			case 'v':
				usr_ungetch(0x2F);
				break;
			case 'X':
			case 'x':
				usr_ungetch(0x2D);
				break;
			/* �Լ����... */
			}
		}
		else {
			switch (ch2) {
			case VK_F1:
				usr_ungetch(0x68);
				break;
			case VK_F2:
				usr_ungetch(0x69);
				break;
			case VK_F3:
				usr_ungetch(0x6A);
				break;
			case VK_F4:
				usr_ungetch(0x6B);
				break;
			case VK_F5:
				usr_ungetch(0x6C);
				break;
			case VK_F6:
				usr_ungetch(0x6D);
				break;
			case VK_F7:
				usr_ungetch(0x6E);
				break;
			case VK_F8:
				usr_ungetch(0x6F);
				break;
			}
		}
	}	/* ���ж� CTRL �� */
	else if ((wKeyState & RIGHT_CTRL_PRESSED) ||
			(wKeyState & LEFT_CTRL_PRESSED)) {
		retval = ch1;
	}	/* ��������ȵ� */
	else if (0 != ch1) {
		retval = ch1;
	}
	else if (!((wKeyState & RIGHT_ALT_PRESSED) ||
		(wKeyState & LEFT_ALT_PRESSED) ||
		(wKeyState & RIGHT_CTRL_PRESSED) ||
		(wKeyState & LEFT_CTRL_PRESSED) ||
		(wKeyState & SHIFT_PRESSED))) {
		retval = 0;
		switch (ch2) {
		case VK_LEFT:
			usr_ungetch(0x4B);
			break;
		case VK_UP:
			usr_ungetch(0x48);
			break;
		case VK_RIGHT:
			usr_ungetch(0x4D);
			break;
		case VK_DOWN:
			usr_ungetch(0x50);
			break;
		case VK_ESCAPE:
			retval = 0x1B;
			break;
		case VK_F1:
			usr_ungetch(0x3B);
			break;
		case VK_F2:
			usr_ungetch(0x3C);
			break;
		case VK_F3:
			usr_ungetch(0x3D);
			break;
		case VK_F4:
			usr_ungetch(0x3E);
			break;
		case VK_F5:
			usr_ungetch(0x3F);
			break;
		case VK_F6:
			usr_ungetch(0x40);
			break;
		case VK_F7:
			usr_ungetch(0x41);
			break;
		case VK_F8:
			usr_ungetch(0x42);
			break;
		case VK_F9:
			usr_ungetch(0x43);
			break;
		case VK_F10:
			usr_ungetch(0x44);
			break;
		case VK_F11:
			usr_ungetch(0x85);
			break;
		case VK_F12:
			usr_ungetch(0x86);
			break;
		case VK_HOME:
			usr_ungetch(0x47);
			break;
		case VK_PAGEUP:
			usr_ungetch(0x49);
			break;
		case VK_PAGEDOWN:
			usr_ungetch(0x51);
			break;
		case VK_END:
			usr_ungetch(0x4f);
			break;
		case VK_DELETE:
			usr_ungetch(0x53);
			break;
		case VK_INSERT:
			usr_ungetch(0x52);
			break;
		}
	}
		/* �ָ���ǰ������ģʽ */
	bSuccess = SetConsoleMode(hStdIn, dwInputMode);
	PERR(bSuccess, "SetConsoleMode");
	return retval;
}


/*
 * usr_ungetch -- ���ַ����·��ص����뻺����
 */
int		_Cdecl usr_ungetch		(int ch)
{
		/* �����뻺��������һ��ѭ������, ���зǿ�ʱ���ܼ��� */
	if ((g_rear + 1) % MAX_MY_INPUT_BUFFER_SIZE != g_front) {
		g_rear = (g_rear + 1) % MAX_MY_INPUT_BUFFER_SIZE;
		g_inputbuf[g_rear] = ch;
	}

	return ch;
}


/*
 * bioskey -- ��ԭ���� bioskey ������������
 */
int	bioskey(int cmd)
{
	int		iKeyLow, iKeyHigh, key;
	int		iSpecialKeyState = 0;
	short	sTmpState;

	if (cmd != 0 && cmd != 1 && cmd != 2)
		return 0;

		/* ��ð���ֵ */
	if (cmd == 0) {
		iKeyLow = usr_getch();
		iKeyHigh = 0;
		if (iKeyLow == 0)
			iKeyHigh = usr_getch();
		key = (iKeyHigh << 8) + iKeyLow;
		return key;
	}	/* �ж��Ƿ��а������� */
	else if (cmd == 1) {
		if (kbhit()) {
			key = usr_getch();
			if (key == 0) {
				usr_ungetch(0);
				key = usr_getch();
			}
			usr_ungetch(key);
			return 1;
		}
		return 0;
	}
		/* ��ѯ�����������״̬ */
		/*  �ұ� SHIFT ��, ���µĻ���־�����λ��1 */
	sTmpState = GetKeyState(VK_RSHIFT);
	if (get_high_byte(&sTmpState, sizeof(short)))
		set_Nth_bit(&iSpecialKeyState, sizeof(int), 1);

		/*  ��� SHIFT ��, ���µĻ���־�ĵ� 2 λ��1 */
	sTmpState = GetKeyState(VK_LSHIFT);
	if (get_high_byte(&sTmpState, sizeof(short)))
		set_Nth_bit(&iSpecialKeyState, sizeof(int), 2);

		/*  �ұ� CONTROL ��, ���µĻ���־�ĵ� 3 λ��1 */
	sTmpState = GetKeyState(VK_RCONTROL);
	if (get_high_byte(&sTmpState, sizeof(short)))
		set_Nth_bit(&iSpecialKeyState, sizeof(int), 3);
		/*  ��� CONTROL ��, ���µĻ���־�ĵ� 3 λ��1 */
	sTmpState = GetKeyState(VK_LCONTROL);
	if (get_high_byte(&sTmpState, sizeof(short)))
		set_Nth_bit(&iSpecialKeyState, sizeof(int), 3);

		/* ALT ��, ���µĻ���־�ĵ� 4 Ϊ�� 1 */
	sTmpState = GetKeyState(VK_MENU);
	if (get_high_byte(&sTmpState, sizeof(short)))
		set_Nth_bit(&iSpecialKeyState, sizeof(int), 4);

		/* SCROLLLOCK ��, �򿪵Ļ���־�ĵ� 5 Ϊ�� 1 */
	sTmpState = GetKeyState(VK_SCROLL);
	if (get_low_byte(&sTmpState, sizeof(short)) == 1)
		set_Nth_bit(&iSpecialKeyState, sizeof(int), 5);

		/* NUMLOCK ��, �򿪵Ļ���־�ĵ� 6 Ϊ�� 1 */
	sTmpState = GetKeyState(VK_NUMLOCK);
	if (get_low_byte(&sTmpState, sizeof(short)) == 1)
		set_Nth_bit(&iSpecialKeyState, sizeof(int), 6);

		/* CapsLOCK ��, �򿪵Ļ���־�ĵ� 7 λ�� 1 */
	sTmpState = GetKeyState(VK_CAPITAL);
	if (get_low_byte(&sTmpState, sizeof(short)) == 1)
		set_Nth_bit(&iSpecialKeyState, sizeof(int), 7);

		/* INSERT ��, ���µĻ���־�ĵ� 8 Ϊ�� 1 */
	sTmpState = GetKeyState(VK_INSERT);
	if (get_high_byte(&sTmpState, sizeof(short)))
		set_Nth_bit(&iSpecialKeyState, sizeof(int), 8);

	return iSpecialKeyState;
}


/*
 * set_Nth_bit -- �������ĵ� n λ�� 1
 * @val: �������������
 * @size: �����ĳ���, �ֽ�Ϊ��λ
 * @index: �� index λ
 */
void set_Nth_bit(void *val, size_t size, int index)
{
	if ((size_t) index > (size * 8) || index <= 0)
		return;

	if (size == 1)
		*(char *)val = *(char *)val | ((char) 1 << (index - 1));
	else if (size == 2)
		*(short *)val = *(short *)val | ((short) 1 << (index - 1));
	else if (size == 8)
		*(long *)val = *(long *)val | ((long) 1 << (index - 1));
	else
		*(int *)val = *(int *)val | ((int) 1 << (index - 1));
	return;
}


/*
 * reset_Nth_bit -- �������ĵ� n λ�� 0
 * @val: �������������
 * @size: �����ĳ���, �ֽ�Ϊ��λ
 * @index: �� index λ
 */
void reset_Nth_bit(void *val, size_t size, int index)
{
	if ((size_t) index > (size * 8) || index <= 0)
		return;

	if (size == 1)
		*(char *)val = *(char *)val & ~((char) 1 << (index - 1));
	else if (size == 2)
		*(short *)val = *(short *)val & ~((short) 1 << (index - 1));
	else if (size == 8)
		*(long *)val = *(long *)val & ~((long) 1 << (index - 1));
	else
		*(int *)val = *(int *)val & ~((int) 1 << (index - 1));
	return;
}


/*
 * get_low_byte -- ��õ��ֽڵ�����
 */
char get_low_byte(void *val, size_t size)
{
	if (NULL == val)
		return 0;

	if (1 == size)
		return *(char *)val;

	return *(short *)val & 0x00FF;
}


/*
 * get_high_byte -- ��ø��ֽڵ�����
 */
char get_high_byte(void *val, size_t size)
{
	if (NULL == val)
		return 0;

	if (1 == size)
		return *(char *)val;

	return ((*(short *)val & 0xFF00) >> 8);
}


/*
 * test_Nth_bit --�����Ե� n λ�Ƿ�Ϊ 1
 * ����ֵ: Ϊ 1 ���� TRUE, Ϊ 0 ���� FALSE.
 */
bool test_Nth_bit(void *val, size_t size, int index)
{
	long	lTmp = 0;

	if (NULL == val ||
		(size_t) index > (8 * size) || index <= 0)
		return FALSE;

	set_Nth_bit(&lTmp, sizeof(long), index);
	if (1 == size)
		return (char) (*(char *) val & (char) lTmp);
	else if (2 == size)
		return (char) (*(short *) val & (short) lTmp);
	else if (4 == size)
		return (char) (*(int *) val & (int) lTmp);
	else if (8 == size)
		return (char) (*(long *)val & (long) lTmp);
	return FALSE;
}


/*
 * flush_inputbuf -- �����뻺������յ�
 */
void flush_inputbuf(void)
{
		/* �ܼ�, �����Զ��в��� */
	g_front = g_rear = 0;
}

/*
 * get_conx -- ��õ�ǰ����̨�buffer�ĺ����С
 * ����ֵ: �����Сֵ
 */
int get_conx(void)
{
	CONSOLE_SCREEN_BUFFER_INFO	csbi;
	bool	bSuccess;

	bSuccess = GetConsoleScreenBufferInfo(g_console, &csbi);
	PERR(bSuccess, "GetConsoleScreenBufferInfo");
	return (csbi.dwSize.X);
}


/*
 * get_cony -- ��õ�ǰ����̨�buffer�������С
 * ����ֵ: �����Сֵ
 */
int get_cony(void)
{
	CONSOLE_SCREEN_BUFFER_INFO	csbi;
	bool	bSuccess;

	bSuccess = GetConsoleScreenBufferInfo(g_console, &csbi);
	PERR(bSuccess, "GetConsoleScreenBufferInfo");
	return (csbi.dwSize.Y);
}


/*
 * where_scr_x ���� ��ù�����Ļˮƽ����
 */
int where_scr_x(void)
{
	int sCurScreenX;

	where_scr_xy(&sCurScreenX, NULL);
	return sCurScreenX;
}


/*
 * where_scr_y ���� ��ù�����Ļ��ֱ����
 */
int where_scr_y(void)
{
	int sCurScreenY;

	where_scr_xy(NULL, &sCurScreenY);
	return sCurScreenY;
}


/*
 * where_scr_xy ���� ��ù�����Ļ����
 */
void where_scr_xy(int *scr_x, int *scr_y)
{
	CONSOLE_SCREEN_BUFFER_INFO	csbi;
	bool	bSuccess;

	if (scr_x == NULL && scr_y == NULL)
		return;

	bSuccess = GetConsoleScreenBufferInfo(g_console, &csbi);
	PERR(bSuccess, "GetConsoleCursorInfo");
		/* ��������õĹ����Ϣ��ͨ��ָ�뷵������ֵ(��Ļ����) */
	if (scr_x != NULL) *scr_x = csbi.dwCursorPosition.X + 1;
	if (scr_y != NULL) *scr_y = csbi.dwCursorPosition.Y + 1;
	return;
}


/*
 * wherexy -- ��õ�ǰ��������
 */
void	_Cdecl wherexy			(int *x, int *y)
{
	int		scr_x, scr_y;

	if (x == NULL && y == NULL)
		return;
	where_scr_xy(&scr_x, &scr_y);
	pos_screen2window(scr_x, scr_y, x, y);
	return;
}


/*
 * wherex -- ��õ�ǰ���ĺ�����
 */
int  	_Cdecl wherex			(void)
{
	int	sCurCursorX;

	wherexy(&sCurCursorX, NULL);
	return sCurCursorX;
}


/*
 * wherey -- ��õ�ǰ����������
 */
int  	_Cdecl wherey			(void)
{
	int	sCurCursorY;

	wherexy(NULL, &sCurCursorY);
	return sCurCursorY;
}


/*
 * gettextcolor -- ��õ�ǰ��ɫ���õ�ǰ��ɫ
 */
int		_Cdecl gettextcolor		(void)
{
	return get_text_color_info(OPT_FOREG);
}


/*
 * gettextbackground -- ��õ�ǰ��ɫ���õı���ɫ
 */
int		_Cdecl gettextbackground(void)
{
	return get_text_color_info(OPT_BACKG);
}


/*
 * gettextattr -- ��õ�ǰ��������������
 */
int		_Cdecl gettextattr		(void)
{
	return get_text_color_info(OPT_ATTR);
}


/*
 * get_text_color_info -- ����������õ���Ҫʵ�ֺ���
 */
static int get_text_color_info(enum cColorIndex cGetIndex)
{
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	bool	bSuccess;
	int		wTmpCharAttr;

	bSuccess = GetConsoleScreenBufferInfo(g_console, &csbi);
	PERR(bSuccess, "GetConsoleScreenBufferInfo");

	wTmpCharAttr = csbi.wAttributes;
	if (cGetIndex == OPT_FOREG)
		return (wTmpCharAttr & (int) 0x0000000F);
	else if (cGetIndex == OPT_BACKG)
		return ((wTmpCharAttr & (int) 0x000000F0) >> 4);
	return wTmpCharAttr;
}


/*
 * textattr -- �����ַ�����
 * @newattr: �µ�����
 */
void	_Cdecl textattr			(int newattr)
{
	set_text_color_info(OPT_ATTR, newattr);
}


/*
 * textbackground -- �����ַ��ı���ɫ
 * newcolor -- ���õ��±���ɫ
 */
void 	_Cdecl textbackground	(int newcolor)
{
	if (!b_backg_high_video)
		newcolor &= ~(int) 0x00000008;

	set_text_color_info(OPT_BACKG, newcolor);
}


/*
 * textcolor -- �����ַ���ǰ��ɫ
 * newcolor -- ���õ���ǰ��ɫ
 */
void 	_Cdecl textcolor		(int newcolor)
{
	set_text_color_info(OPT_FOREG, newcolor);
}


/*
 * set_text_color_info -- ������ɫ�ȵ����Ե���Ҫʵ�ֺ���
 * @cSetIndex: ���ñ�ʶ, ��ʾ���Ƕ�ǰ��ɫ����, ���ǶԱ���ɫ����, ���Ƕ��������Ե�����
 * @wCharColor: ���õ��µ���
 */
static void set_text_color_info(enum cColorIndex cSetIndex, int wCharColor)
{
	bool	bSuccess;
	int		wTmpCharAttr, wCharOldAttr;
	DWORD	dwCharsWritten;
	COORD	coordBuf = {0, 0};

		/* ��������ģʽֻ�кڰ���ɫ */
	if (g_cur_screen_mode == BW40 || g_cur_screen_mode == BW80) {
		wTmpCharAttr = (int) ((LIGHTGRAY + (BLACK << 4)) | (wCharColor & (int) 0x0000FF88));
		bSuccess = SetConsoleTextAttribute(g_console, (WORD) wTmpCharAttr);
	}
	else if (g_cur_screen_mode == MONO) {
			/* ��õ�ǰ����ɫ���� */
		wCharOldAttr = gettextattr();
			/* ���ǵ�ɫģʽҪ�ı�ǰ��ɫ */
		if (cSetIndex == OPT_FOREG)
			wTmpCharAttr = (wCharOldAttr & (int) 0x0000FF80) | (wCharColor & (int) 0x0000000F);
		else if (cSetIndex == OPT_BACKG)
			wTmpCharAttr = (wCharOldAttr & (int) 0x0000FF0F) | ((wCharColor << 4) & (int) 0x00000080);
		else if (cSetIndex == OPT_ATTR)
			wTmpCharAttr = wCharColor & (int) 0x0000FF8F;
		bSuccess = FillConsoleOutputAttribute(g_console, (WORD) wTmpCharAttr,
							(DWORD) (get_conx() * get_cony()), coordBuf, &dwCharsWritten);
		PERR(bSuccess, "FillConsoleOutputAttribute");

		bSuccess = SetConsoleTextAttribute(g_console, (WORD) wTmpCharAttr);		
	}
	else {
			/* �����ǲ�ɫģʽ, �����������ַ�������, ��һ�������� */
		if (cSetIndex == OPT_ATTR)
			bSuccess = SetConsoleTextAttribute(g_console, (WORD) wCharColor);
		else {
				/* �Ȼ�õ�ǰ����ɫ����, �Ա��������� */
			wCharOldAttr = gettextattr();
				/* �������Ҫ��������ɫ */
			if (cSetIndex == OPT_FOREG)
				wTmpCharAttr = (wCharOldAttr & (int) 0x0000FFF0) | (wCharColor & (int) 0x0000000F);
			else if (cSetIndex == OPT_BACKG)
				wTmpCharAttr = (wCharOldAttr & (int) 0x0000FF0F) | ((wCharColor << 4) & (int) 0x000000F0);

			bSuccess = SetConsoleTextAttribute(g_console, (WORD) wTmpCharAttr);
		}		
	}
	PERR(bSuccess, "SetConsoleTextAttribute");
	return;
}


/*
 * fill_rect_attr -- �ı�ָ�������ַ�������, ���ı䵱ǰ����ɫ����
 * @left:
 * @top: �������Ͻ�
 * @right:
 * @bottom: �������½�
 * @char_attr: ָ��������
 */
void fill_rect_attr(int left, int top, int right, int bottom, int char_attr)
{
	COORD	coordStart, coordEnd;
	COORD	coordBuf;
	bool	bSuccess;
	DWORD	dwFillSize, dwCharsWritten;
	int		wTmpOutputCharAttr, iOldCharAttr;

		/* ��������, ����� */
	if (left <= 0 || left > get_conx() ||
		right <= 0 || right > get_conx() ||
		top <= 0 || top > get_cony() ||
		bottom <= 0 || bottom > get_cony())
		return;

	iOldCharAttr = gettextattr();
		/* ����������ɫ */
	if (g_cur_screen_mode == BW40 || g_cur_screen_mode == BW80)
		wTmpOutputCharAttr = (char_attr & (int) 0x0000FF88) | (int) 0x00000007;
	else if (g_cur_screen_mode == MONO)
		wTmpOutputCharAttr = iOldCharAttr & (int) 0x0000FF8F;
	else
		wTmpOutputCharAttr = char_attr;
	
		/* ����ı������ʵ���������� */
	coordStart.X = (SHORT) (min(left, right) - 1);
	coordStart.Y = (SHORT) (min(top, bottom) - 1);
	coordEnd.X = (SHORT) (max(left, right) - 1);
	coordEnd.Y = (SHORT) (max(top, bottom) - 1);
		/* ����ÿ����仺�����Ĵ�С */
	dwFillSize = (coordEnd.X - coordStart.X + 1);
	for (coordBuf = coordStart; coordBuf.Y <= coordEnd.Y; ++coordBuf.Y) {
		bSuccess = FillConsoleOutputAttribute(g_console, (WORD) wTmpOutputCharAttr, dwFillSize,
			coordBuf, &dwCharsWritten);
		PERR(bSuccess, "FillConsoleOutputAttribute");	
	}
		/* �����ַ������Ļ����Բ��ı���ǰ����ɫ���� */
	return;
}


/*
 * fill_rect_char -- ���ض����Ե��ַ����ָ������, ���ı䵱ǰ����ɫ����
 * @left:
 * @top: �������Ͻ�
 * @right:
 * @bottom: �������½�
 * @char_attr: ָ��������
 * @cFillChar: �����ַ�
 */
void fill_rect_char(int left, int top, int right, int bottom,
				   int char_attr, int cFillChar)
{
	COORD	coordStart, coordEnd;
	COORD	coordBuf;
	bool	bSuccess;
	DWORD	dwFillSize, dwCharsWritten;
	int		wTmpOutputCharAttr, iOldCharAttr;

		/* ��������, ����� */
	if (left <= 0 || left > get_conx() ||
		right <= 0 || right > get_conx() ||
		top <= 0 || top > get_cony() ||
		bottom <= 0 || bottom > get_cony())
		return;

	iOldCharAttr = gettextattr();

	coordStart.X = (SHORT) (min(left, right) - 1);
	coordStart.Y = (SHORT) (min(top, bottom) - 1);
	coordEnd.X = (SHORT) (max(left, right) - 1);
	coordEnd.Y = (SHORT) (max(top, bottom) - 1);

		/* ����������ɫ */
	if (g_cur_screen_mode == BW40 || g_cur_screen_mode == BW80)
		wTmpOutputCharAttr = (char_attr & (int) 0x0000FF88) | (int) 0x00000007;
	else if (g_cur_screen_mode == MONO)
		wTmpOutputCharAttr = iOldCharAttr & (int) 0x0000FF8F;
	else
		wTmpOutputCharAttr = char_attr;

		/* ����ÿ����仺�����Ĵ�С */
	dwFillSize = (coordEnd.X - coordStart.X + 1);
	for (coordBuf = coordStart; coordBuf.Y <= coordEnd.Y; ++coordBuf.Y) {		
			/* ������ַ�(��ɫ��һ������ȷ) */
		bSuccess = FillConsoleOutputCharacter(g_console, (TCHAR) cFillChar, dwFillSize,
			coordBuf, &dwCharsWritten);
		PERR(bSuccess, "FillConsoleOutputCharacter");
			/* ������ȷ��������ַ�����ɫ */
		bSuccess = FillConsoleOutputAttribute(g_console, (WORD) wTmpOutputCharAttr, dwFillSize,
			coordBuf, &dwCharsWritten);
		PERR(bSuccess, "FillConsoleOutputAttribute");	
	}
	return;
}


/*
 * clrscr -- �Ե�ǰ��Ĵ��ڽ���ˢ��
 */
void	_Cdecl clrscr			(void)
{
	int	char_attr = gettextattr();

		/* ��������һ�������������(ע: ���ۺ�ʱ, �����fillRectangle ������ֵ������Ļ����) */
	fill_rect_char(g_curwin_left, g_curwin_top, g_curwin_right, g_curwin_bottom,
		char_attr, ' ');
		/* ˢ���󽫹����Ϊ(1, 1)�� */
	gotoxy(1, 1);
	return;
}


/*
 * window -- ����һ���µĻ����
 * @left:
 * @top: �������Ͻ�
 * @right:
 * @bottom: �������½�
 */
void 	_Cdecl window			(int left, int top, int right, int bottom)
{
			/* �������� */
	if (left <= 0 || left > get_conx() ||
		right <= 0 || right > get_conx() ||
		top <= 0 || top > get_cony() ||
		bottom <= 0 || bottom > get_cony())
		return;
		/* �������� */
	if (top > bottom)
		Swap(&top, &bottom, sizeof(int));
	if (left > right)
		Swap(&left, &right, sizeof(int));
		/* ���õ�ǰ��ô������� */
	g_curwin_left		= left;
	g_curwin_top		= top;
	g_curwin_right	= right;
	g_curwin_bottom	= bottom;
	g_curwin_width	= right - left + 1;
	g_curwin_height	= bottom - top + 1;
		/* ���õ�ǰ���λ�� */
	gotoxy(1, 1);
	return;
}


/*
 * textmode -- ������ʾģʽ
 * @newmode: �µ���ʾģʽ, ���嶨���ͷ�ļ�
 */
void 	_Cdecl textmode			(int newmode)
{
	switch (newmode) {
	case BW40:
			/* ��ԭ���Ѿ�������ģʽ�Ͳ��ı� */
		if (BW40 == g_old_screen_mode)
			return;
			/* �ڰ� 40 * 25, �ı䴰�ڴ�С */
		resize_con_buf_and_window(40, 25);
		g_old_screen_mode = g_cur_screen_mode;
		g_cur_screen_mode = BW40;
		break;
	case C40:
		if (C40 == g_old_screen_mode)
			return;
			/* ��ɫ 40 * 25, �ı䴰�ڴ�С, ����һ�� */
		resize_con_buf_and_window(40, 25);
		g_old_screen_mode = g_cur_screen_mode;
		g_cur_screen_mode = C40;
		break;
	case BW80:
		if (BW80 == g_old_screen_mode)
			return;
		resize_con_buf_and_window(80, 25);
		g_old_screen_mode = g_cur_screen_mode;
		g_cur_screen_mode = BW80;	
		break;
	case C80:
		if (C80 == g_old_screen_mode)
			return;
		resize_con_buf_and_window(80, 25);
		g_old_screen_mode = g_cur_screen_mode;
		g_cur_screen_mode = C80;
		break;
	case MONO:
		if (MONO == g_old_screen_mode)
			return;
		resize_con_buf_and_window(80, 25);
		g_old_screen_mode = g_cur_screen_mode;
		g_cur_screen_mode = MONO;	
		break;
	case LASTMODE:
			/* ���ó���һ�ε�ģʽ */
		textmode(g_old_screen_mode);
		break;
	default:
		break;
	}

	b_backg_high_video = FALSE;
	textattr(LIGHTGRAY + (BLACK << 4));
	clrscr();
	return;
}


/*
 * highvideo -- ǰ��ɫ����
 *
 * ˵��: ʵ���� VC ��֧�ֱ���ɫ������, ������и����� backghighvideo
 */
void	_Cdecl highvideo		(void)
{
	int	wCharForeg;

	wCharForeg = gettextcolor();
	wCharForeg |= FOREGROUND_INTENSITY;
	textcolor(wCharForeg);
	return;
}


/*
 * normvideo -- ���ַ������Իָ�ΪĬ��
 *
 * ˵��: һ������, �ָ�Ϊ: �ڵװ���
 */
void	_Cdecl normvideo		(void)
{
	b_backg_high_video = FALSE;
	textattr(LIGHTGRAY + (BLACK << 4));
}


/*
 * gettextinfo -- ��ô�����Ϣ
 * @r: ������Ϣ�ṹ��ָ��
 */
void	_Cdecl gettextinfo		(struct text_info *r)
{
	int		iTmpCharAttr;

	if (NULL == r)
		return;

	iTmpCharAttr = gettextattr();

		/* ��õ�ǰ��ô������� */
	r->winleft		= g_curwin_left;
	r->wintop		= g_curwin_top;
	r->winright		= g_curwin_right;
	r->winbottom	= g_curwin_bottom;
		/* ��ǰ��ɫ���� */
	r->attribute = gettextattr();
		/* ǰ��ɫ�Ƿ���� */
	r->normattr = iTmpCharAttr & (int) 0x00000008 ? 0 : 1;
		/* ��õ�ǰ���ڵ����ֵ */
	r->screenwidth	= g_curwin_width;
	r->screenheight	= g_curwin_height;
		/* ��õ�ǰ����λ�� */
	wherexy((int *) &r->curx, (int *) &r->cury);
		/* ��õ�ǰ����ʾģʽ */
	r->currmode = g_cur_screen_mode;
	return;
}


/*
 * usr_putchar -- �� putchar ���, ���һ���ַ�
 * @c: Ҫ������ַ�
 * ����ֵ: ����ַ��� ascii
 */
int		_Cdecl usr_putchar		(int c)
{
	return putchar_handle(0, 0, c, FALSE);
}


/*
 * usr_putch -- �� usr_putchar ����һ��
 * @c: Ҫ������ַ�
 * ����ֵ: ����ַ��� ascii
 */
int		_Cdecl usr_putch		(int c)
{
	return putchar_handle(0, 0, c, FALSE);
}


/*
 * usr_putcharxy -- �ڴ�����ָ��λ������ַ�
 * @x:
 * @y: ������ַ�����(��������)
 * @c: Ҫ������ַ�
 * ����ֵ: ����ַ��� ascii
 */
int		_Cdecl usr_putcharxy	(int x, int y, int c)
{
	return putchar_handle(x, y, c, TRUE);
}


/*
 * putchar_handle -- ����ַ�����Ҫʵ�ֺ���
 * @x:
 * @y: ������ַ�����(��������)
 * @c: Ҫ������ַ�
 * @bMoveCursor: �ƶ�����Ƿ���Ч
 * ����ֵ: ����ַ��� ascii
 */
static int putchar_handle(int x, int y, int ch, bool bMoveCursor)
{
	bool	bSuccess;
	DWORD	dwCharWritten;
	COORD	coordCurPos;
	int		char_attr;
	int		scr_x, scr_y,
			win_x, win_y;
	bool	bScrollWindow = FALSE;

		/* �������� */
	if (bMoveCursor && (x <= 0 || x > g_curwin_width || y <= 0 || y > g_curwin_height))
		return EOF;


		/* ��õ�ǰ����ɫ���� */
	char_attr = gettextattr();
		/* �ƶ���겢��ù��ĵ�ǰλ�� */
	if (bMoveCursor) {
		gotoxy(x, y);
		win_x = x;
		win_y = y;
	}
	else
		wherexy(&win_x, &win_y);

		/* ����������ж� */
	if (ch == '\n') {
			/* ���ѵ����ڵ�������һ��, ���Ϲ����� */
		if (win_y == g_curwin_height)
			movetext(g_curwin_left, g_curwin_top, g_curwin_right, g_curwin_bottom,
					g_curwin_left, g_curwin_top - 1);
		else
			gotoxy(win_x, ++win_y);
		return ch;
	}

	if (ch == '\r') {
		gotoxy(1, wherey());
		return ch;
	}

	if (ch == '\b') {
		gotoxy(wherex() > 1 ? wherex() - 1 : 1, wherey());
		return ch;
	}

	if (ch == '\t' && g_bshowtab) {
		win_x += ((tabstop + 1) - (win_x % (tabstop + 1)));
		if (win_x > g_curwin_width) {
			if (win_y == g_curwin_height) {
				movetext(g_curwin_left, g_curwin_top, g_curwin_right, g_curwin_bottom,
						g_curwin_left, g_curwin_top - 1);
				gotoxy(win_x - g_curwin_width, g_curwin_height);
			}
			else
				gotoxy(win_x - g_curwin_width, wherey()+1);
		} else
			gotoxy(win_x, wherey());
		return ch;
	}
		/* ת��Ϊ��Ļ���� */
	pos_window2screen(win_x, win_y, &scr_x, &scr_y);
		/* ת��Ϊ�������� */
	coordCurPos.X = (SHORT) (scr_x - 1);
	coordCurPos.Y = (SHORT) (scr_y - 1);
		/* ��� */
	bSuccess = FillConsoleOutputCharacter(g_console, (TCHAR) ch, 1,
		coordCurPos, &dwCharWritten);
	PERR(bSuccess, "FillConsoleOutputCharacter");
	bSuccess = FillConsoleOutputAttribute(g_console, (WORD) char_attr, 1,
		coordCurPos, &dwCharWritten);
	PERR(bSuccess, "FillConsoleOutputCharacter");

		/* ����ַ�����"���", ���ѵ����ڵ����ұ� */
	if (win_x == g_curwin_width) {
			/* ���絽�˴��ڵ����±� */
		if (win_y == g_curwin_height) {
				/* ���� */
			movetext(g_curwin_left, g_curwin_top, g_curwin_right, g_curwin_bottom,
					g_curwin_left, g_curwin_top - 1);
			gotoxy(1, g_curwin_height);
		}
		else
			gotoxy(1, ++win_y);
	}
	else {
			/* һ���ַ�ֻҪ����һ��λ�� */
		gotoxy(++win_x, win_y);
	}

	return ch;
}


/*
 * movetext -- �ƶ���Ļ�������ݵ�ָ��λ��
 * @left:
 * @top: ָ����������Ͻ�
 * @right:
 * @bottom: ָ����������½�
 * @destleft: 
 * @desttop: Ŀ��λ��
 * ����ֵ: �ɹ� 1, ʧ�� 0
 */
int		_Cdecl movetext			(int left, int top, int right, int bottom,
								 int destleft, int desttop)
{
	bool		bSuccess;
	COORD		coordDest;
	CHAR_INFO	chiFill;
	int			char_attr;
	SMALL_RECT	srctScrollRect, srctClipRect;

		/* �������� */
	if (left <= 0 || left > get_conx() ||
		right <= 0 || right > get_conx() ||
		top <= 0 || top > get_cony() ||
		bottom <= 0 || bottom > get_cony() ||
		destleft <=0 || destleft > get_conx() ||
		desttop <= 0 || desttop> get_cony())
		return 0;

		/* �������� */
	if (top > bottom)
		Swap(&top, &bottom, sizeof(int));
	if (left > right)
		Swap(&left, &right, sizeof(int));

		/* ����Ҫ�ƶ������� */
	srctScrollRect.Left		= (SHORT) (left - 1);
	srctScrollRect.Top		= (SHORT) (top -  1);
	srctScrollRect.Right	= (SHORT) (right - 1);
	srctScrollRect.Bottom	= (SHORT) (bottom - 1);
		/* ���ü�������(Ҳ�������������) */
	srctClipRect.Left	= (SHORT) (g_curwin_left - 1);
	srctClipRect.Top	= (SHORT) (g_curwin_top - 1);
	srctClipRect.Right	= (SHORT) (g_curwin_right - 1);
	srctClipRect.Bottom	= (SHORT) (g_curwin_bottom - 1);
		/* ����Ŀ�ĵ����� */
	coordDest.X = (SHORT) (destleft - 1);
	coordDest.Y = (SHORT) (desttop - 1);
		/* ��õ�ǰ��ɫ������ */
	char_attr = gettextattr();
		/* �����ƶ��������µ�"�հ�"��������ַ������� */
	chiFill.Char.AsciiChar = ' ';	/* �ÿո���� */
	chiFill.Attributes = (WORD) char_attr;
		/* �����ƶ� */
	bSuccess = ScrollConsoleScreenBuffer(g_console, &srctScrollRect,
		&srctClipRect, coordDest, &chiFill);
	PERR(bSuccess, "ScrollConsoleScreenBuffer");
	return 1;
}


/*
 * usr_cprintf -- �� cprintf ����, �ӵ�ǰλ�������ʽ��
 * @format: ��ʽ��
 * ����ֵ: ����������ַ�����
 */
int		_Cdecl usr_cprintf		(const char *format, ...)
{
	va_list	ap;
	int		dwCharsWrittenInTotal;

	if (NULL == format)
		return 0;

		/* ����ɱ�����б� */
	va_start(ap, format);
	dwCharsWrittenInTotal = printf_handle(0, 0, FALSE, format, ap);
	va_end(ap);
	return dwCharsWrittenInTotal;
}


/*
 * usr_cprintfxy -- ��ָ��λ�ÿ�ʼ�����ʽ��
 * @x:
 * @y: ָ����λ�� (��������)
 * @format: ��ʽ��
 * ����ֵ: ����������ַ�����
 */
int		_Cdecl usr_cprintfxy		(int x, int y, const char *format, ...)
{
	va_list	ap;
	int dwCharsWrittenInTotal;

	if (x <= 0 || x > g_curwin_width ||
		y <= 0 || y > g_curwin_height ||
		NULL == format)
	return 0;

	va_start(ap, format);
	dwCharsWrittenInTotal = printf_handle(x, y, TRUE, format, ap);
	va_end(ap);
	return dwCharsWrittenInTotal;
}


/*
 * usr_cprintfcolorxy -- �ڴ����е�ǰλ�ô����ָ����ɫ���Եĸ�ʽ��
 */
int		_Cdecl usr_cprintfcolor	(int attr, const char *format, ...)
{
	va_list	ap;
	int		dwCharsWrittenInTotal,
			iOldAttr;

	if (NULL == format)
		return 0;

	iOldAttr = gettextattr();
	textattr(attr);
		/* ����ɱ�����б� */
	va_start(ap, format);
	dwCharsWrittenInTotal = printf_handle(0, 0, FALSE, format, ap);
	va_end(ap);
	textattr(iOldAttr);
	return dwCharsWrittenInTotal;
}


/*
 * usr_cprintfcolorxy -- �ڴ�����ָ�����괦���ָ����ɫ���Եĸ�ʽ�� 
 */
int		_Cdecl usr_cprintfcolorxy(int x, int y, int attr, const char *format, ...)
{
	va_list	ap;
	int		dwCharsWrittenInTotal,
			iOldAttr;

	if (x <= 0 || x > g_curwin_width ||
		y <= 0 || y > g_curwin_height || NULL == format)
		return 0;

	iOldAttr = gettextattr();
	textattr(attr);

	va_start(ap, format);
	dwCharsWrittenInTotal = printf_handle(x, y, TRUE, format, ap);
	va_end(ap);
	textattr(iOldAttr);
	return dwCharsWrittenInTotal;
}


/*
 * usr_cputs -- ����ַ���
 * @str: Ҫ������ַ���
 * ����ֵ: ����������ַ�����
 */
int		_Cdecl usr_cputs		(const char *str)
{
	int		dwCharsWrittenInTotal;

	if (NULL == str)
		return 0;

	dwCharsWrittenInTotal = usr_cprintf(str);
	/* usr_putchar('\n'); */	/* ԭ��� cputs ���������Ҳû��������з� */
	return dwCharsWrittenInTotal;
}


/*
 * printf_handle -- ����ַ�������Ҫ����
 * @x:
 * @y: ���λ��
 * @bMoveCursor: �Ƿ��ƶ����
 * @format: ��ʽ��
 * @ap: �ɱ��
 * ����ֵ: ����������ַ�����
 */
static int printf_handle(int x, int y, bool bMoveCursor,
						 const char * format, va_list ap)
{
	char	szTmpBuf[BUFSIZE], *p = NULL;
	bool	bSuccess;
	DWORD	dwCharsWritten;
	int		dwCharsIntendToWrite, dwCharsWrittenInTotal = 0;
	COORD	coordCurPos;
	int		char_attr;
	int		scr_x, scr_y,
			win_x, win_y,
			i, iIndex;
	bool	bLfOccurs = FALSE,
			bCrOccurs = FALSE,
			bBakOccurs = FALSE,
			bTabOccurs = FALSE;

		/* �������� */
	if (bMoveCursor && (x <= 0 || x > g_curwin_width || y <= 0 || y > g_curwin_height))
		return EOF;

		/* ��õ�ǰ����ɫ���� */
	char_attr = gettextattr();

	if (bMoveCursor) {
		gotoxy(x, y);
		win_x = x;
		win_y = y;
	}
	else
		wherexy(&win_x, &win_y);
	vsprintf(szTmpBuf, format, ap);
	p = szTmpBuf;
	for ( ; ; ) {
				/* ת��Ϊ��Ļ���� */
		pos_window2screen(wherex(), wherey(), &scr_x, &scr_y);
			/* ת��Ϊʵ���������� */
		coordCurPos.X = (SHORT) (scr_x - 1);
		coordCurPos.Y = (SHORT) (scr_y - 1);
			/* ֻ���ڹ涨������(����ڷ�Χ֮��)���, �����ͻ��л���� */
		dwCharsIntendToWrite = min(lstrlen(p), (g_curwin_width - wherex() + 1));
		dwCharsWrittenInTotal += dwCharsIntendToWrite;

		bLfOccurs	= FALSE,
		bCrOccurs	= FALSE,
		bBakOccurs	= FALSE,
		bTabOccurs	= FALSE;
		iIndex = 0;
		for (i = 0; i < dwCharsIntendToWrite; ++i) {
				/* ��ʽ���к��лس� */
			if (p[i] == '\n') {
				iIndex = i;
				bCrOccurs = TRUE;
				break;
			}	/* ��ʽ���к��� '\r' */
			else if (p[i] == '\r') {
				iIndex = i;
				bLfOccurs = TRUE;
				break;
			}	/* ��ʽ���к��� '\b' */
			else if (p[i] == '\b') {
				iIndex = i;
				bBakOccurs = TRUE;
				break;
			}	/* ��ʽ���к��� '\t' */
			else if (p[i] == '\t' && g_bshowtab) {
				iIndex = i;
				bTabOccurs = TRUE;
				break;
			}
		}

		if (bLfOccurs || bCrOccurs || bBakOccurs || bTabOccurs) {
			bSuccess = WriteConsole(g_console, p, iIndex, &dwCharsWritten, NULL);
			PERR(bSuccess, "WriteConsole");
			if (bLfOccurs)			usr_putchar('\r');
			else if (bCrOccurs)		usr_putchar('\n');
			else if (bBakOccurs)	usr_putchar('\b');
			else if (bTabOccurs)	usr_putchar('\t');
			p += (iIndex + 1);
			++dwCharsWrittenInTotal;
			continue;
		}
			/* ����һ����� */
		bSuccess = WriteConsole(g_console, p, dwCharsIntendToWrite, &dwCharsWritten, NULL);
		PERR(bSuccess, "WriteConsole");

		if (where_scr_x() > g_curwin_right) {
			if (coordCurPos.Y >= g_curwin_bottom - 1) {
					/* �������ƶ�����������ʵ�� */
				movetext(g_curwin_left, g_curwin_top, g_curwin_right, g_curwin_bottom,
					g_curwin_left, g_curwin_top - 1);
					/* ����ƶ������ڵ����½� */
				gotoxy(1, g_curwin_height);
			}
			else
				gotoxy(1, wherey()+1);
		}
		p += dwCharsIntendToWrite;
			/* ���������˾��˳���ʾ */
		if (*p == '\0')
			break;
	}
	return dwCharsWrittenInTotal;
}


/*
 * usr_getche -- ������, ����ַ�
 * ����ֵ: ������ַ��� ascii
 */
int		_Cdecl usr_getche		(void)
{
	int		ch;

	ch = usr_getch();
		/* ���ո�������ַ���ʾ����, �������Ч������ʵ�ֵ� */
	usr_putchar(ch);
	return ch;
}


/*
 * clreol -- clear end of line
 */
void	_Cdecl clreol			(void)
{
	int		scr_x, scr_y,
			win_x, win_y;
	DWORD	dwCharsWrittn;
	char	szTmp[256];
	int		len;
	COORD	coordCurPos;
	bool	bSuccess;
	int		char_attr;

		/* ��õ�ǰ����ɫ���� */
	char_attr = gettextattr();
		/* ��õ�ǰ����λ��(�������) */
	wherexy(&win_x, &win_y);
		/* ת��Ϊ��Ļ���� */
	pos_window2screen(win_x, win_y, &scr_x, &scr_y);
	coordCurPos.X = (SHORT) (scr_x - 1);
	coordCurPos.Y = (SHORT) (scr_y - 1);
		/* �������Ҫ�������ַ��� */
	len = g_curwin_width - win_x + 1;
		/* ��ʼ���... */
	memset(szTmp, ' ', len);
	bSuccess = WriteConsoleOutputCharacter(g_console, szTmp, len,
		coordCurPos, &dwCharsWrittn);
	PERR(bSuccess, "WriteConsoleOutputCharacter");
	bSuccess = FillConsoleOutputAttribute(g_console, (WORD) char_attr, len,
		coordCurPos, &dwCharsWrittn);
	PERR(bSuccess, "FillConsoleOutputAttribute");
	return;
}


/*
 * delline -- ɾ��������ڵ�����, ��������������Ϲ�
 */
void	_Cdecl delline			(void)
{
	int		sCurCursorX, sCurCursorY,
			scr_x, scr_y;

	wherexy(&sCurCursorX, &sCurCursorY);
		/* ���������һ���Ǿ�ֱ���ÿո�ȥ��� */
	if (sCurCursorY == g_curwin_height) {
		gotoxy(1, g_curwin_height);
		clreol();
			/* ������ƻ�ԭ���ĵط� */
		gotoxy(sCurCursorX, sCurCursorY);
	}
	else {
		pos_window2screen(sCurCursorX, sCurCursorY, &scr_x, &scr_y);
			/* ����������Կ������ƶ��������� */
		movetext(g_curwin_left, scr_y + 1, g_curwin_right, g_curwin_bottom, 
			g_curwin_left, scr_y);
	}

	return;
}


/*
 * insline -- insert line �ڹ�굱ǰλ�ò���һ��, ��������������¹�
 */
void 	_Cdecl insline			(void)
{
	int		sCurCursorX, sCurCursorY,
			scr_x, scr_y;

	wherexy(&sCurCursorX, &sCurCursorY);
		/* ���������һ���Ǿ�ֱ���ÿո�ȥ��� */
	if (sCurCursorY == g_curwin_height) {
		gotoxy(1, g_curwin_height);
		clreol();
			/* ������ƻ�ԭ���ĵط� */
		gotoxy(sCurCursorX, sCurCursorY);
	}
	else {
		pos_window2screen(sCurCursorX, sCurCursorY, &scr_x, &scr_y);
			/* ����������Կ������ƶ��������� */
		movetext(g_curwin_left, scr_y, g_curwin_right,
			g_curwin_bottom, g_curwin_left, scr_y + 1);
	}
	return;
}


/*
 * usr_cgets -- �����ַ���
 * ����ֵ: ����������ַ���
 *
 * ˵��: str[0] ΪҪ���������ַ�����, str[1] �᷵��ʵ������ĸ���, str + 2 ��ʼ�������������
 */
char   *_Cdecl usr_cgets		(char *str)
{
	char	*p = NULL;

		/* �������� */
	if (str == NULL)
		return NULL;

	p = gets_handle(0, 0, FALSE, str, NORM_INPUT_STYLE);
	return p;
}


/*
 * gets_handle -- ���뺯������Ҫʵ�ֺ���
 * @x:
 * @y: ���ĸ�λ�ÿ�ʼ����
 * @bMoveCursor: �Ƿ��ƶ����
 * @pStrBuf: ������ַ�������֮��
 * @cStyle: ���������
 *
 * ˵��: ����������� 3 ��, 1��ͨ��������, 2����'*'��������, 3��������������
 */
#ifndef MAX_INPUT_LEN
#define MAX_INPUT_LEN	(MAX_CHAR_LINE+3)
#endif
static char *gets_handle(int x, int y, bool bMoveCursor, char *pStrBuf, enum cInputStyle cStyle)
{	
	char	szTmpBuf[MAX_INPUT_LEN];
	char	*p;
	int		ch, len = 0, iMaxInputChars, i;
	int		win_x, win_y,
			sOriginX, sOriginY;
	bool	bFull = FALSE;

		/* �������� */
	if (pStrBuf == NULL ||
		(bMoveCursor && (x <= 0 || x > get_conx() || y <= 0 || y > get_cony())))
		return NULL;

	iMaxInputChars = pStrBuf[0];
	if (iMaxInputChars < 0)
		iMaxInputChars += (MAX_INPUT_LEN - 1);

		/* �ж��Ƿ���Ҫ�ƶ���� */
	if (bMoveCursor) {
		gotoxy(x, y);
		sOriginX = x;
		sOriginY = y;
	}
	else
		wherexy(&sOriginX, &sOriginY);

	p = szTmpBuf;
	for (i = 0; ; ++i) {
			/* ����һ���ַ� */
		ch = usr_getch();
			/* �س��ͽ��� */
		if (ch == VK_RETURN)
			break;
			/* ���� delete ������ backspace �� */
		if (VK_BACK == ch) {
				/* ���ǻ��ԵĻ�, ��Ҫ���ݴ��ڵĴ�С��λ�ÿ�������֮������ƶ�*/
			if (cStyle != NOECHO_INPUT_STYLE) {
					/* ���Ȼ�õ�ǰ����λ�� */
				wherexy(&win_x, &win_y);
				if (win_y > sOriginY) {
					if (win_x != 1) {
							/* �������, ��Ȼ���ҲҪ��Ӧ�ƶ� */
						usr_putcharxy(win_x - 1, win_y, ' ');
						gotoxy(win_x - 1, win_y);
					}
					else {
							/* �������ڴ��ڵ������, ��ô����һ�е�ĩβ����һ���ַ�, �������������һ�е���ĩβ */
						usr_putcharxy(g_curwin_width, win_y - 1, ' ');
						gotoxy(g_curwin_width, win_y - 1);
					}
				}
				else {
					if (win_x > sOriginX) {
							/* �������, ��Ȼ���ҲҪ��Ӧ�ƶ� */
						usr_putcharxy(win_x - 1, win_y, ' ');
						gotoxy(win_x - 1, win_y);
					}
					else {
							/* ����Ѿ��˶��������뿪ʼ�� */
						i = -1;
						p[0] = '\0';
						continue;
					}
				}
				p[i - 1] = '\0';
				i -= 2;
			}
			else {
					/* �������벻����(���λ�ò���) */
				if (i != 0) {
					p[i - 1] = '\0';
					i -= 2;
				}
				else {
					i = -1;
					p[0] = '\0';
				}
			}
			continue;
		}

		if (i >= iMaxInputChars - 1) {
			p[i] = '\0';
			bFull = TRUE;
		}
		else
			bFull = FALSE;

		if (bFull == FALSE)
			p[i] = ch;

		if (i > MAX_INPUT_LEN - 2 || bFull) {
			if (cStyle != NOECHO_INPUT_STYLE)
				putchar('\7');
			--i;
			continue;
		}

		if (cStyle == NORM_INPUT_STYLE) {
			if ('\t' == ch)	g_bshowtab = FALSE;
			usr_putchar(ch);
			if ('\t' == ch)	g_bshowtab = TRUE;
		}
		else if (cStyle == PASSWORD_INPUT_STYLE)
			usr_putchar('*');
	}

	if (i <= iMaxInputChars - 1)
		p[i] = '\0';

	memcpy(pStrBuf + 2, p, i + 1);
	pStrBuf[1] = i;
	if (wherey() < get_curwin_height())
		gotoxy(1, wherey()+1);
	else
		usr_cprintf("\r\n");
	return (pStrBuf + 2);
}


/*
 * usr_cgetline ���� �ӵ�ǰλ������һ���ַ������maxlen����
 */
char   *_Cdecl usr_cgetline(char buff[], int maxlen)
{
	buff[0] = maxlen;
	return usr_cgets(buff);
}


/*
 * gettext -- ����ָ������Ļ���ݵ�ָ���� buffer
 * @left:
 * @top: �������Ͻ�
 * @right:
 * @bottom: �������½�
 * @destin: ����֮��
 * ����ֵ: �ɹ�1, ʧ��0
 */
int		_Cdecl gettext			(int left, int top, int right, int bottom,
								 void *destin)
{
	bool	bSuccess;
	COORD	coordBufSize;
	COORD	coordDest = {0, 0};
	SMALL_RECT	srReadRegion;

		/* �������� */
	if (left <= 0 || left > get_conx() ||
		right <= 0 || right > get_conx() ||
		top <= 0 || top > get_cony() ||
		bottom <= 0 || bottom > get_cony() ||
		destin == NULL)
		return 0;

		/* �������� */
	if (top > bottom)
		Swap(&top, &bottom, sizeof(int));
	if (left > right)
		Swap(&left, &right, sizeof(int));

		/* ����Ҫ��ȡ���ݵľ������� */
	srReadRegion.Left	= (SHORT) (left - 1);
	srReadRegion.Top	= (SHORT) (top - 1);
	srReadRegion.Right	= (SHORT) (right - 1);
	srReadRegion.Bottom	= (SHORT) (bottom - 1);
	coordBufSize.X = (SHORT) (right - left + 1);
	coordBufSize.Y = (SHORT) (bottom - top + 1);
	bSuccess = ReadConsoleOutput(g_console, (PCHAR_INFO) destin, coordBufSize,
		coordDest, &srReadRegion);
	PERR(bSuccess, "ReadConsoleOutput");
	return 1;
}


/*
 * puttext -- ����ѱ�������ݵ�ָ��������
 * @left:
 * @top: �������Ͻ�
 * @right:
 * @bottom: �������½�
 * @source: ��Դ֮��
 * ����ֵ: �ɹ�1, ʧ��0
 *
 *
 * ˵��: �� VC �¿��ܺ� TC �е㲻̫һ��, ����Ĵ�СӦ�ó��� 4. (����: bufSize = 80 * 25 * 4)
 */
int		_Cdecl puttext			(int left, int top, int right, int bottom,
								 void *source)
{
	bool	bSuccess;
	COORD	coordBufSize;
	COORD	coordDest = {0, 0};
	SMALL_RECT	srReadRegion;

		/* �������� */
	if (left <= 0 || left > get_conx() ||
		right <= 0 || right > get_conx() ||
		top <= 0 || top > get_cony() ||
		bottom <= 0 || bottom > get_cony() ||
		source == NULL)
		return 0;
		/* ��������*/
	if (top > bottom)
		Swap(&top, &bottom, sizeof(int));
	if (left > right)
		Swap(&left, &right, sizeof(int));
		/* ���÷��þ������� */
	srReadRegion.Left	= (SHORT) (left - 1);
	srReadRegion.Top	= (SHORT) (top - 1);
	srReadRegion.Right	= (SHORT) (right - 1);
	srReadRegion.Bottom	= (SHORT) (bottom - 1);
	coordBufSize.X = (SHORT) (right - left + 1);
	coordBufSize.Y = (SHORT) (bottom - top + 1);
	bSuccess = WriteConsoleOutput(g_console, (PCHAR_INFO) source, coordBufSize,
					coordDest, &srReadRegion);
	PERR(bSuccess, "WriteConsoleOutput");
	return 1;
}


/*
 * usr_getpass -- �� getpass ���ƹ���, ����� prompt ����, Ȼ��Ҫ������, � 8 λ
 * @prompt: ��ʾ����
 * ����ֵ: ����������ַ���
 */
char   *_Cdecl usr_getpass		(const char *prompt)
{
	static char password[11];
	int	iOldAttr;
	int	iCurCursorX, iCurCursorY;	

	iOldAttr = gettextattr();
	textattr(LIGHTGRAY + (BLACK << 4));
	usr_cprintf(prompt);

		/* � 8 ���ַ�, ����'\0' �� 9 ���ַ� */
	password[0] = 8 + 1;
	gets_handle(0, 0, FALSE, password, NOECHO_INPUT_STYLE);
		/* ò���Ǹ����������ʱ���ַ����Ǻڵװ��� */
	textattr(iOldAttr);

	wherexy(&iCurCursorX, &iCurCursorY);
	if (iCurCursorY != g_curwin_height)
		gotoxy(1, ++iCurCursorY);

	return (password + 2);
}


/*
 * usr_kbhit -- �������Ƿ��м�����, ϵͳ���������, ֱ�ӵ��õ���
 */
int		_Cdecl usr_kbhit		(void)
{
	return _kbhit();
}


/*
 * usr_cscanf -- �����ʽ��
 */
#if 0
int		_Cdecl usr_cscanf		(const char *format, ...)
{
	int			retval;
	va_list		ap;

	va_start(ap, format);
	retval = cscanf(format, ap);
	va_end(ap);
	return retval;
}
#endif


/*
 * clreoc -- clear end of column
 */
void	_Cdecl clreoc			(void)
{
	int		win_x, win_y,
			len, i;

		/* ��õ�ǰ����λ��(�������) */
	wherexy(&win_x, &win_y);
		/* ��������Ҫ�������ַ��� */
	len = g_curwin_height - win_y + 1;
	for (i = 0; i < len; ++i)
		usr_putcharxy(win_x, win_y + i, ' ');
		/* ���ص�ԭ�ȵ�λ�� */
	gotoxy(win_x, win_y);
	return;
}

/*
 * inscolumn -- �ӹ��ָ��λ�ò���һ��
 */
void	_Cdecl inscolumn		(void)
{
	int		sCurCursorX, sCurCursorY,
			scr_x, scr_y;

	wherexy(&sCurCursorX, &sCurCursorY);
		/* ����������һ���Ǿ�ֱ���ÿո�ȥ��� */
	if (sCurCursorX == g_curwin_right) {
		gotoxy(sCurCursorX, 1);
		clreoc();
			/* ������ƻ�ԭ���ĵط� */
		gotoxy(sCurCursorX, sCurCursorY);
	}
	else {
		pos_window2screen(sCurCursorX, sCurCursorY, &scr_x, &scr_y);
			/* ����������Կ������ƶ��������� */
		movetext(scr_x, g_curwin_top, g_curwin_right, g_curwin_bottom,
			scr_x + 1, g_curwin_top);
	}
	return;
}


/*
 * delcolumn -- �ӹ��ָ��λ��ɾ��һ��
 */
void	_Cdecl delcolumn		(void)
{
	int		sCurCursorX, sCurCursorY,
			scr_x, scr_y;

	wherexy(&sCurCursorX, &sCurCursorY);
		/* ����������һ���Ǿ�ֱ���ÿո�ȥ��� */
	if (sCurCursorX == g_curwin_right) {
		gotoxy(sCurCursorX, 1);
		clreoc();
			/* ������ƻ�ԭ���ĵط� */
		gotoxy(sCurCursorX, sCurCursorY);
	}
	else {
		pos_window2screen(sCurCursorX, sCurCursorY, &scr_x, &scr_y);
			/* ����������Կ������ƶ��������� */
		movetext(scr_x + 1, g_curwin_top, g_curwin_right, g_curwin_bottom,
			scr_x, g_curwin_top);
	}
	return;
}


/*
 * get_cur_char_attr -- ��õ�ǰ�ַ�������
 * ����ֵ: ��õ��ַ�����
 */
int		_Cdecl get_cur_char_attr(void)
{
	return get_certain_char_attr_handle(0, 0, FALSE);
}

/*
 * get_char_attrxy -- ���ָ���ַ�������
 * @x:
 * @y: ָ��λ��
 * ����ֵ: ��õ��ַ�����
 */
int		_Cdecl get_char_attrxy	(int x, int y)
{
	if (x <= 0 || x > g_curwin_width ||
		y <= 0 || y > g_curwin_height)
		return 0;

	return get_certain_char_attr_handle(x, y, TRUE);
}

/*
 * get_certain_char_attr_handle -- ����ַ����Ե���Ҫʵ�ֺ���
 * @x:
 * @y: ָ����λ��
 * @bMoveCursor: �Ƿ��ƶ����
 * ����ֵ: ��õ��ַ�����
 */
static int get_certain_char_attr_handle(int x, int y, bool bMoveCursor)
{
	bool	bSuccess;
	DWORD	dwCharsWritten;
	COORD	coordSource;
	int		char_attr;
	int		sCurCursorX, sCurCursorY,
			scr_x, scr_y;

		/* �������� */
	if (bMoveCursor && (x <= 0 || x > g_curwin_width ||
		y <= 0 || y > g_curwin_height))
		return gettextattr();

		/* �ƶ���겢��ù��ĵ�ǰλ�� */
	if (bMoveCursor) {
		gotoxy(x, y);
		sCurCursorX = x;
		sCurCursorY = y;
	}
	else
		wherexy(&sCurCursorX, &sCurCursorY);
		/* ����������ת��Ϊ��Ļ���� */
	pos_window2screen(sCurCursorX, sCurCursorY, &scr_x, &scr_y);
		/* ת��Ϊʵ���������� */
	coordSource.X = (SHORT) (scr_x - 1);
	coordSource.Y = (SHORT) (scr_y - 1);
		/* ��ȡĿ���ַ������� */
	bSuccess = ReadConsoleOutputAttribute(g_console, (LPWORD) &char_attr, sizeof(WORD),
		coordSource, &dwCharsWritten);
	PERR(bSuccess, "ReadConsoleOutputAttribute");
	return char_attr;
}


/*
 * backghighvideo -- ����ɫ����
 */
void	_Cdecl backghighvideo	(void)
{
	int		iCharBackg;

	iCharBackg = gettextbackground();
	iCharBackg <<= 4;
	iCharBackg |= (int) BACKGROUND_INTENSITY;
	iCharBackg >>= 4;

	b_backg_high_video = TRUE;
	textbackground(iCharBackg);
	return;
}


/*
 * showcursor -- ��ʾ���
 */
void	_Cdecl showcursor		(void)
{
	set_cursor_mode_handle(-1, TRUE);
}


/*
 * closecursor -- �رչ��
 */
void	_Cdecl closecursor		(void)
{
	set_cursor_mode_handle(-1, FALSE);
}


/*
 * setcursorsize -- ���ù��Ĵ�С (1 ~ 100)
 * @size: Ҫ���õĹ��Ĵ�С
 */
void	_Cdecl setcursorsize	(int size)
{
	if (size <=0 || size > 100)
		return;

	set_cursor_mode_handle(size, TRUE);
}

/*
 * set_cursor_mode_handle -- ���ù�����Ե���Ҫʵ�ֺ���
 * @size: Ҫ���õĹ���С
 * @bVisiable: ����Ƿ�ɼ�
 */
static void set_cursor_mode_handle(int size, bool bVisiable)
{
	CONSOLE_CURSOR_INFO	cci;
	bool	bSuccess;
	bool	bModifySize = FALSE,
			bModifyShowOffState = FALSE;;

	bSuccess = GetConsoleCursorInfo(g_console, &cci);
	PERR(bSuccess, "GetConsoleCursorInfo");

	if (size > 0 && size <= 100) {
		cci.dwSize = (DWORD) size;
		bModifySize = TRUE;
	}

	if (!bModifySize) {
		if (cci.bVisible == bVisiable)
			return;

		cci.bVisible = bVisiable;
	}

	bSuccess = SetConsoleCursorInfo(g_console, &cci);
	PERR(bSuccess, "SetConsoleCursorInfo");
	return;
}


/*
 * input_passwd -- ��������(���� '*')
 * ����ֵ: ����������ַ���
 * ˵��: strbuf[0]Ϊ����������, strbuf[1] ����ʵ��������ַ�����
 */
char   *_Cdecl input_passwd		(char *strbuf)
{
	if (NULL == strbuf)
		return NULL;

	return gets_handle(0, 0, FALSE, strbuf, PASSWORD_INPUT_STYLE);
}


/*
 * ˵��: ���¼��������������������...
 */
char   *_Cdecl input_passwdxy	(int x, int y, char *strbuf)
{
	if (x <= 0 || x > g_curwin_width ||
		y <= 0 || y > g_curwin_height ||
		NULL == strbuf)
		return NULL;

	return gets_handle(x, y, TRUE, strbuf, PASSWORD_INPUT_STYLE);
}

char   *_Cdecl input_noecho		(char *strbuf)
{
	if (NULL == strbuf)
		return NULL;

	return gets_handle(0, 0, FALSE, strbuf, NOECHO_INPUT_STYLE);
}

char   *_Cdecl input_noechoxy	(int x, int y, char *strbuf)
{
	if (x <= 0 || x > g_curwin_width ||
		y <= 0 || y > g_curwin_height ||
		NULL == strbuf)
		return NULL;

	return gets_handle(x, y, TRUE, strbuf, NOECHO_INPUT_STYLE);
}

char   *_Cdecl usr_cgetsxy		(int x, int y, char *strbuf)
{
	if (x <= 0 || x > g_curwin_width ||
		y <= 0 || y > g_curwin_height ||
		NULL == strbuf)
		return NULL;

	return gets_handle(x, y, TRUE, strbuf, NORM_INPUT_STYLE);
}


/*
 * set_char_special_mode -- ���õ�ǰ�����ַ�����������
 *
 * ˵��: ֻ����������ʾҳ�²���Ч
 */
void set_char_special_mode(enum special_char_attr char_mode)
{
	int				wTmpCharAttr;
	unsigned int	wCodeCP;

	wCodeCP = get_codepage();
	if (wCodeCP != CHINESE_OUTPUT_CP)
		return;

	wTmpCharAttr = gettextattr();
	wTmpCharAttr = (wTmpCharAttr & (int) 0x000000FF) | char_mode;
	textattr(wTmpCharAttr);
	return;
}


/*
 * reset_char_special_mode -- �ָ���ǰ�����ַ�����������Ϊ��
 */
void reset_char_special_mode(void)
{
	unsigned int	wCodeCP;

	wCodeCP = get_codepage();
	if (wCodeCP != CHINESE_OUTPUT_CP)
		return;

	set_char_special_mode(RESET_NORM_MODE);
}


/*
 * get_char_special_mode -- ��õ�ǰ�����ַ�����������
 */
int  get_char_special_mode(void)
{
	int				wTmpCharMode;
	unsigned int	wCodeCP;

	wCodeCP = get_codepage();
	if (wCodeCP != CHINESE_OUTPUT_CP)
		return RESET_NORM_MODE;

	wTmpCharMode = gettextattr();
	return (wTmpCharMode & (int) 0x0000FF00);
}


/*
 * box -- ��һ������
 * @left:
 * @top: �������Ͻ�
 * @right:
 * @bottom: �������½� (��������)
 * @char_attr: �߿��ַ�������
 * @single: �Ƿ񵥿�, ����˫�߿�
 */
void box(int left, int top, int right, int bottom, int char_attr, bool single)
{
	bool	bSuccess;
	COORD	coordStart, coordEnd;
	COORD	coordBuf;
	int		dwFillSize, dwCharsWritten;
	int		wFrameType;
	int		iOldCharAttr, iTmpOutputCharAttr;
	int		i;
	int		scr_x, scr_y,
			sOldWinX, sOldWinY;
	int		cBoxCar[2][6] = {{218, 196, 191, 179, 192, 217},		/* ���߿�ASCII���� */
							{0xc9, 0xcd, 0xbb, 0xba, 0xc8, 0xbc}};	/* ˫�߿�ASCII���� */

		/* ��������, ����� */
	if (left <= 0 || left > g_curwin_width ||
		right <= 0 || right > g_curwin_width ||
		top <= 0 || top > g_curwin_height ||
		bottom <= 0 || bottom > g_curwin_height)
		return;

		/* ��������*/
	if (top > bottom)
		Swap(&top, &bottom, sizeof(int));
	if (left > right)
		Swap(&left, &right, sizeof(int));

		/* ����һ��ʼ������ֵ */
	wherexy(&sOldWinX, &sOldWinY);
		/* ת�����Ͻ����굽��Ļ���� */
	pos_window2screen(left, top, &scr_x, &scr_y);
	coordStart.X = (SHORT) (scr_x - 1);
	coordStart.Y = (SHORT) (scr_y - 1);
		/* ת�����½����굽��Ļ���� */
	pos_window2screen(right, bottom, &scr_x, &scr_y);
	coordEnd.X = (SHORT) (scr_x - 1);
	coordEnd.Y = (SHORT) (scr_y - 1);
		/* ̫С��, �������߿� */
	if (coordEnd.X - coordStart.X < 2 ||
		coordEnd.Y - coordStart.Y < 2)
		return;

		/* ���浱ǰ����ɫ���� */
	iOldCharAttr = gettextattr();

	if (g_cur_screen_mode == BW40 || g_cur_screen_mode == BW80)
		iTmpOutputCharAttr = (char_attr & (int) 0x0000FF88) | (int) 0x00000007;
	else if (g_cur_screen_mode == MONO)
		iTmpOutputCharAttr = iOldCharAttr & (int) 0x0000FF8F;
	else
		iTmpOutputCharAttr = char_attr;

		/* ���óɹ涨�ı߿���ɫ */
	textattr(iTmpOutputCharAttr);
		/* ���ñ߿�����, ˫���ǵ� */
	wFrameType = single ? 0 : 1;
		/* ������Ǿ���Ļ��߿��� */

		/* ���Ͻ� */
	usr_putcharxy(left, top, cBoxCar[wFrameType][0]);	
		/* ���Ͻ� */
	usr_putcharxy(right, top, cBoxCar[wFrameType][2]);	
		/* ���½� */
	usr_putcharxy(left, bottom, cBoxCar[wFrameType][4]);
		/* ���½� */
	usr_putcharxy(right, bottom, cBoxCar[wFrameType][5]);	
		/* �϶�ˮƽ�� */
	coordBuf = coordStart;
	++coordBuf.X;
	dwFillSize = (coordEnd.X - coordStart.X - 1);
	bSuccess = FillConsoleOutputCharacter(g_console, (TCHAR) cBoxCar[wFrameType][1], dwFillSize,
			coordBuf, &dwCharsWritten);
	PERR(bSuccess, "FillConsoleOutputCharacter");
	bSuccess = FillConsoleOutputAttribute(g_console, (WORD) iTmpOutputCharAttr, dwFillSize,
			coordBuf, &dwCharsWritten);
	PERR(bSuccess, "FillConsoleOutputAttribute");
		/* �¶�ˮƽ�� */
	coordBuf.Y = coordEnd.Y;
	bSuccess = FillConsoleOutputCharacter(g_console, (TCHAR) cBoxCar[wFrameType][1], dwFillSize,
			coordBuf, &dwCharsWritten);
	PERR(bSuccess, "FillConsoleOutputCharacter");
	bSuccess = FillConsoleOutputAttribute(g_console, (WORD) iTmpOutputCharAttr, dwFillSize,
			coordBuf, &dwCharsWritten);
	PERR(bSuccess, "FillConsoleOutputAttribute");
		/*�����������*/
	for (i = 0; i < bottom - top - 1; ++i) {
		usr_putcharxy(left, top + 1 + i, cBoxCar[wFrameType][3]);
		usr_putcharxy(right, top + 1 + i, cBoxCar[wFrameType][3]);
	}
		/* �ָ�ԭ�ȵ���ɫ���� */
	textattr(iOldCharAttr);
		/* �ص����Ͻ� */
	gotoxy(left, top);
	return;
}


/*
 * draw3Dwindow -- ����һ���� 3D Ч���Ļ����
 * @left:
 * @top: �������Ͻ�
 * @right:
 * @bottom: �������½� (��Ļ����)
 * @table_attr: ���ٵĴ��ڵ���ɫ����
 */
void draw3Dwindow(int left, int top, int right, int bottom, int table_attr)
{
	int		iOldCharAttr, iTmpOutputCharAttr;

		/* ������� */
	if (left <= 0 || left > get_conx() ||
		right <= 0 || right > get_conx() ||
		top <= 0 || top > get_cony() ||
		bottom <= 0 || bottom > get_cony())
		return;

	iOldCharAttr = gettextattr();

	if (g_cur_screen_mode == BW40 || g_cur_screen_mode == BW80)
		iTmpOutputCharAttr = (table_attr & (int) 0x0000FF88) | (int) 0x00000007;
	else if (g_cur_screen_mode == MONO)
		iTmpOutputCharAttr = iOldCharAttr & (int) 0x0000FF8F;
	else
		iTmpOutputCharAttr = table_attr;

		/* ��������*/
	if (top > bottom)
		Swap(&top, &bottom, sizeof(int));
	if (left > right)
		Swap(&left, &right, sizeof(int));

	fill_rect_char(left + 1, top + 1, right + 1, bottom + 1, BLACK, ' ');
		/* ���õ�ǰ�Ļ���� */
	window(left, top, right, bottom);
	textattr(table_attr);
	clrscr();
	return;
}


/*
 * draw_horizen_line -- ��ˮƽ��
 * @x1:
 * @y1: ��ʼ����
 * @x2: ��ֹ������
 * @char_attr: ������ɫ����(���ı䵱ǰ�������ڵ���ɫ����)
 * @ch: �������ַ�
 */
void draw_horizen_line(int x1, int y1, int x2, int char_attr, int ch)
{
	char	szTmp[128];
	int		len;
	bool	bSuccess;
	int		dwCharsWritten;
	COORD	coordFrom;
	int		scr_x, scr_y;
	int		iOldCharAttr, iTmpOutputCharAttr;

		/* �������� */
	if (x1 <= 0 || x1 > g_curwin_width ||
		x2 <= 0 || x2 > g_curwin_width ||
		y1 <= 0 || y1 > g_curwin_height)
		return;

	iOldCharAttr = gettextattr();

	if (g_cur_screen_mode == BW40 || g_cur_screen_mode == BW80)
		iTmpOutputCharAttr = (char_attr & (int) 0x0000FF88) | (int) 0x00000007;
	else if (g_cur_screen_mode == MONO)
		iTmpOutputCharAttr = iOldCharAttr & (int) 0x0000FF8F;
	else
		iTmpOutputCharAttr = char_attr;

		/* �������� */
	if (x1 > x2)
		Swap(&x1, &x2, sizeof(int));

	pos_window2screen(x1, y1, &scr_x,&scr_y);
	coordFrom.X = (SHORT) (scr_x - 1);
	coordFrom.Y = (SHORT) (scr_y - 1);
	len = x2 - x1 + 1;
	memset(szTmp, ch, len);
	bSuccess = WriteConsoleOutputCharacter(g_console, szTmp, len,
		coordFrom, &dwCharsWritten);
	PERR(bSuccess, "WriteConsoleOutputCharacter");
	bSuccess = FillConsoleOutputAttribute(g_console, (WORD) iTmpOutputCharAttr, len,
		coordFrom, &dwCharsWritten);
	PERR(bSuccess, "FillConsoleOutputAttribute");
	return;
}


/*
 * draw_vertical_line -- ��Ǧ����
 * @x1:
 * @y1: ��ʼ����
 * @y2: ��ֹ������
 * @char_attr: ������ɫ����(���ı䵱ǰ�������ڵ���ɫ����)
 * @ch: �������ַ�
 */
void draw_vertical_line(int x1, int y1, int y2, int char_attr, int ch)
{
	int		len, i,
			iOldCharAttr, iTmpOutputCharAttr;

	if (x1 <= 0 || x1 > g_curwin_width ||
		y1 <= 0 || y1 > g_curwin_width ||
		y2 <= 0 || y2 > g_curwin_height)
		return;

	iOldCharAttr = gettextattr();

	if (g_cur_screen_mode == BW40 || g_cur_screen_mode == BW80)
		iTmpOutputCharAttr = (char_attr & (int) 0x0000FF88) | (int) 0x00000007;
	else if (g_cur_screen_mode == MONO)
		iTmpOutputCharAttr = iOldCharAttr & (int) 0x0000FF8F;
	else
		iTmpOutputCharAttr = char_attr;

	if (y1 > y2)
		Swap(&y1, &y2, sizeof(int));

		/* ���óɹ涨��ɫ */
	textattr(iTmpOutputCharAttr);
		/* ���廭���� */
	len = y2 - y1 + 1;
	for (i = 0; i < len; ++i)
		usr_putcharxy(x1, y1 + i, ch);
		/* ���û�ԭ������ɫ */
	textattr(iOldCharAttr);
	return;
}


/*
 * comfirm_box -- ����һ��ȷ�Ͽ�, �� ESC �� ENTER ����
 * @left:
 * @top: ȷ�Ͽ����Ͻ�
 * @note_msg: �����Ϣ�ַ���
 * @btype: ȷ�Ͽ�����, ����ͨ�����Ϣ��, ������, ������
 */
void comfirm_box(int left, int top, const char *note_msg, enum box_type btype)
{
	char	*pcScreenBuf = NULL;
	int		iOldCharAttr, iTmpOutputCharAttr,
			len = 0, x, y,
			key;
	struct text_info r;

		/* ������� */
	if (left <= 0 || left > get_conx() ||
		top <= 0 || top > get_cony() ||
		NULL == note_msg)
		return;

	wherexy(&x, &y);
	closecursor();

	gettextinfo(&r);
	iOldCharAttr = r.attribute;

	if (NORM_TYPE == btype)
		MessageBeep(MB_ICONASTERISK);
	else if (WARN_TYPE == btype)
		MessageBeep(MB_ICONEXCLAMATION);
	else
		MessageBeep(MB_ICONHAND);

	if (g_cur_screen_mode == BW40 || g_cur_screen_mode == BW80)
		iTmpOutputCharAttr = WHITE + (BLACK << 4);
	else if (g_cur_screen_mode == MONO)
		iTmpOutputCharAttr = iOldCharAttr & (int) 0x0000FF8F;
	else if (NORM_TYPE == btype)
		iTmpOutputCharAttr = YELLOW + (BLUE << 4);
	else if (WARN_TYPE == btype)
		iTmpOutputCharAttr = LIGHTCYAN + (BLUE << 4);
	else
		iTmpOutputCharAttr = WHITE + (RED << 4);

	window(1, 1, get_conx(), get_cony());
	textattr(iTmpOutputCharAttr);

	len = lstrlen(note_msg);

	pcScreenBuf = (char *) malloc((len + 4 + 1) * (10 + 1) * 4);
	PERR(pcScreenBuf, "malloc");

	gettext(left, top, left + len + 4, top + 10, pcScreenBuf);
	fill_rect_char(left, top, left + len + 4, top + 10, iTmpOutputCharAttr, ' ');
	box(left, top, left + len + 4, top + 10, iTmpOutputCharAttr, TRUE);
	usr_putcharxy(left + 2, top, '|');
	if (NORM_TYPE == btype)
		usr_cprintf(" NOTE |");
	else if (WARN_TYPE == btype)
		usr_cprintf(" WARN |");
	else
		usr_cprintf(" ERROR |");

	window(left + 2, top + 3, left + len + 2, top + 4);
	usr_cputs(note_msg);

	window(1, 1, get_conx(), get_cony());
	box(left + len/2 - 3, top + 6, left + len/2 + 5, top + 8, iTmpOutputCharAttr, TRUE);
	usr_cprintfcolorxy(left + len/2 - 2, top + 7, WHITE + (BLACK << 4), "  O K  ");

	for ( ; ; ) {
		while (bioskey(1) == 0)
			;
		key = bioskey(0);
		if (KEY_ESC == key || KEY_ENTER == key)
			break;
		MessageBeep(MB_OK);
	}

	puttext(left, top, left + len + 4, top + 10, pcScreenBuf);
	free(pcScreenBuf);

	window(r.winleft, r.wintop, r.winright, r.winbottom);
	textattr(iOldCharAttr);
	showcursor();
	gotoxy(x, y);
	return;
}


/*
 * get_curwin_width ���� ��õ�ǰ���ڵĿ��
 */
int get_curwin_width(void)
{
	return g_curwin_width;
}


/*
 * get_curwin_height ���� ��õ�ǰ���ڵĸ߶�
 */
int get_curwin_height(void)
{
	return g_curwin_height;
}


/*
 * get_curwin ���� ��õ�ǰ���ڵ���Ļ����ֵ
 */
void get_curwin(int *left, int *top, int *right, int *bottom)
{
	if (left	!= NULL)	*left	= g_curwin_left;
	if (top		!= NULL)	*top	= g_curwin_top;
	if (right	!= NULL)	*right	= g_curwin_right;
	if (bottom	!= NULL)	*bottom	= g_curwin_bottom;
}


/*
 * skip_blanks ���� ��λstr�ַ����еĵ�һ���ǿ��ַ�
 */
char *skip_blanks(const char *str)
{
	if (NULL == str)
		return NULL;
	while (' ' == *str || '\t' == *str)
		++str;
	return (char *) str;
}
