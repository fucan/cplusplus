#ifndef _MYCONIO_H
#define _MYCONIO_H

#if     !defined(_WIN32) && !defined(_MAC)
#error ERROR: Only Mac or Win32 targets supported!
#endif

#if _STDC__
#define _Cdecl
#else
#define _Cdecl cdecl
#endif

#ifndef __OLDCONIO__

struct text_info {
	unsigned char winleft;
	unsigned char wintop;
	unsigned char winright;
	unsigned char winbottom;
	unsigned char attribute;
	unsigned char normattr;
	unsigned char currmode;
	unsigned char screenheight;
	unsigned char screenwidth;
	unsigned char curx;
	unsigned char cury;
};

enum text_modes { LASTMODE=-1, BW40=0, C40, BW80, C80, MONO=7 };

#if	!defined(__COLORS)
#define __COLORS

enum COLORS {
	BLACK,
	BLUE,
	GREEN,
	CYAN,
	RED,
	MAGENTA,
	BROWN,
	LIGHTGRAY,
	DARKGRAY,
	LIGHTBLUE,
	LIGHTGREEN,
	LIGHTCYAN,
	LIGHTRED,
	LIGHTMAGENTA,
	YELLOW,
	WHITE
};
#endif

#include <stdio.h>
#include <conio.h>

//#define BLINK		128

void	_Cdecl clreol			(void);
void	_Cdecl clrscr			(void);
void	_Cdecl delline			(void);
int		_Cdecl gettext			(int left, int top, int right, int bottom,
								 void *destin);
void	_Cdecl gettextinfo		(struct text_info *r);
void 	_Cdecl gotoxy			(int x, int y);
void	_Cdecl highvideo		(void);
void 	_Cdecl insline			(void);
void	_Cdecl lowvideo			(void);
int		_Cdecl movetext			(int left, int top, int right, int bottom,
								 int destleft, int desttop);
void	_Cdecl normvideo		(void);
int		_Cdecl puttext			(int left, int top, int right, int bottom,
								 void *source);
void	_Cdecl textattr			(int newattr);
void 	_Cdecl textbackground	(int newcolor);
void 	_Cdecl textcolor		(int newcolor);
void 	_Cdecl textmode			(int newmode);
int  	_Cdecl wherex			(void);
int  	_Cdecl wherey			(void);
void 	_Cdecl window			(int left, int top, int right, int bottom);
#endif

char   *_Cdecl usr_cgets		(char *str);
int		_Cdecl usr_cprintf		(const char *format, ...);
int		_Cdecl usr_cputs		(const char *str);
#define usr_cscanf	scanf
// int		_Cdecl usr_cscanf		(const char *format, ...);
int		_Cdecl usr_getch		(void);
int		_Cdecl usr_getche		(void);
char   *_Cdecl usr_getpass		(const char *prompt);
int		_Cdecl usr_kbhit		(void);
int		_Cdecl usr_putch		(int c);
int		_Cdecl usr_ungetch		(int ch);
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void	_Cdecl initscr			(void);										/* ��������ڳ���ʼʱ����, ��ʼ��һЩ�� */
void	_Cdecl endwin			(void);										/* ��������ڳ������ʱ���� */
int		_Cdecl usr_putchar		(int c);									/* ����ַ�, �� usr_putch ����һ�� */
int		_Cdecl usr_putcharxy	(int x, int y, int c);						/* �ڴ�����ָ�����괦����ַ� */
int		_Cdecl usr_cprintfxy	(int x, int y, const char *format, ...);	/* �ڴ�����ָ�����괦�����ʽ�� */
int		_Cdecl usr_cprintfcolor	(int attr, const char *format, ...);		/* �ڴ����е�ǰλ�ô����ָ����ɫ���Եĸ�ʽ�� */
int		_Cdecl usr_cprintfcolorxy(int x, int y, int attr, const char *fmt, ...);/* �ڴ���ָ�����괦���ָ����ɫ���Ը�ʽ�� */
void	_Cdecl wherexy			(int *x, int *y);							/* ��õ�ǰ���Ĵ�������ֵ */
void	_Cdecl where_scr_xy		(int *scr_x, int *scr_y);					/* ��õ�ǰ������Ļ����ֵ */
int		_Cdecl where_scr_x		(void);										/* ��õ�ǰ������Ļˮƽ����ֵ */
int		_Cdecl where_scr_y		(void);										/* ��õ�ǰ������Ļ��ֱ����ֵ */
int		_Cdecl get_curwin_width	(void);										/* ��õ�ǰ���ڵĿ�� */
int		_Cdecl get_curwin_height(void);										/* ��õ�ǰ���ڵĸ߶� */
void	_Cdecl get_curwin		(int *l, int *t, int *r, int *b);			/* ��õ�ǰ���ڵ���Ļ����ֵ */
int		_Cdecl gettextcolor		(void);										/* ��õ�ǰ��ǰ��ɫ */
int		_Cdecl gettextbackground(void);										/* ��õ�ǰ�ı���ɫ */
int		_Cdecl gettextattr		(void);										/* ��õ�ǰ����ɫ����(ǰ/����ɫ), ��������ʾҳ, Ҳ�����ַ����������� */
int		_Cdecl bioskey			(int cmd);									/* �� TC �µ� bioskey ���, �������� */
void	_Cdecl clreoc			(void);										/* �� clreol ���, ɾ����ǰλ�õ��������¶˵���һ�� */
void	_Cdecl inscolumn		(void);										/* �� insline ���, ����һ�� */
void	_Cdecl delcolumn		(void);										/* �� delline ���, ɾ��һ�� */
void	_Cdecl backghighvideo	(void);										/* ����ɫ���� */
void	_Cdecl showcursor		(void);										/* ��ʾ��� */
void	_Cdecl closecursor		(void);										/* �رչ�� */
void	_Cdecl setcursorsize	(int size);									/* ���ù���С(0~100) */
int		_Cdecl get_cur_char_attr(void);										/* ��õ�ǰ��������ַ������� */
int		_Cdecl get_char_attrxy	(int x, int y);								/* ���ָ��λ���ַ������� (��������) */
char   *_Cdecl usr_cgetsxy		(int x, int y, char *strbuf);				/* ָ��λ�ÿ�ʼ�����ַ���, strbuf[0]Ϊ��������ַ��� */
char   *_Cdecl usr_cgetline		(char buff[], int maxlen);					/* ��ǰλ�������ַ��������maxlen���ַ� */
char   *_Cdecl input_passwd		(char *strbuf);								/* ��ǰλ�ÿ�ʼ��������(���� '*') */
char   *_Cdecl input_passwdxy	(int x, int y, char *strbuf);				/* ָ��λ�ÿ�ʼ��������(���� '*') */
char   *_Cdecl input_noecho		(char *strbuf);								/* ��ǰλ�ÿ�ʼ����(������) */
char   *_Cdecl input_noechoxy	(int x, int y, char *strbuf);				/* ָ��λ�ÿ�ʼ����(������) */
char   *_Cdecl skip_blanks		(const char *str);							/* �ӹ�str�ַ����е�ǰ���ո��Ʊ�� */

	/* ������Ӧ�ļ�ֵ */
#define KEY_LEFT		0X4B00
#define KEY_UP			0X4800
#define KEY_RIGHT		0X4D00
#define KEY_DOWN		0X5000
#define KEY_ESC			0X1B
#define KEY_F1			0X3B00
#define KEY_F2			0X3C00
#define KEY_F3			0X3D00
#define KEY_F4			0X3E00
#define KEY_F5			0X3F00
#define KEY_F6			0X4000
#define KEY_F7			0X4100
#define KEY_F8			0X4200
#define KEY_F9			0X4300
#define KEY_F10			0X4400
#define KEY_F11			0X8500
#define KEY_F12			0X8600
#define KEY_ENTER		0X0D
#define KEY_HOME		0X4700
#define KEY_END			0X4F00
#define KEY_BACKSPACE	0X08
#define KEY_PAGEUP		0X4900
#define KEY_PAGEDOWN	0X5100
#define KEY_INSERT		0X5200
#define KEY_DELETE		0X5300

#define KEY_ALT_A		0X1E00
#define KEY_ALT_B		0X3000
#define KEY_ALT_C		0X2E00
#define KEY_ALT_D		0X2000
#define KEY_ALT_E		0X1200
#define KEY_ALT_F		0X2100
#define KEY_ALT_G		0X2200
#define KEY_ALT_H		0X2300
#define KEY_ALT_I		0X1700
#define KEY_ALT_M		0X3200
#define KEY_ALT_N		0X3100
#define KEY_ALT_O		0X1800
#define KEY_ALT_P		0X1900
#define KEY_ALT_Q		0X1000
#define KEY_ALT_S		0X1F00
#define KEY_ALT_T		0X1400
#define KEY_ALT_V		0X2F00
#define KEY_ALT_X		0X2D00
#define KEY_ALT_F1		0X6800
#define KEY_ALT_F2		0X6900
#define KEY_ALT_F3		0X6A00
#define KEY_ALT_F4		0X6B00
#define KEY_ALT_F5		0X6C00
#define KEY_ALT_F6		0X6D00
#define KEY_ALT_F7		0X6E00
#define KEY_ALT_F8		0X6F00

#define KEY_CTRL_A		0X1
#define KEY_CTRL_B		0X2
#define KEY_CTRL_C		0X3
#define KEY_CTRL_D		0X4
#define KEY_CTRL_E		0X5
#define KEY_CTRL_F		0X6
#define KEY_CTRL_G		0X7
#define KEY_CTRL_H		0X8
#define KEY_CTRL_I		0X9
#define KEY_CTRL_J		0XA
#define KEY_CTRL_K		0XB
#define KEY_CTRL_L		0XC
#define KEY_CTRL_M		0XD
#define KEY_CTRL_N		0XE
#define KEY_CTRL_O		0XF
#define KEY_CTRL_P		0X10
#define KEY_CTRL_Q		0X11
#define KEY_CTRL_R		0X12
#define KEY_CTRL_S		0X13
#define KEY_CTRL_T		0X14
#define KEY_CTRL_U		0X15
#define KEY_CTRL_V		0X16
#define KEY_CTRL_W		0X17
#define KEY_CTRL_X		0X18
#define KEY_CTRL_Y		0X19
#define KEY_CTRL_Z		0X1A

typedef char			bool;
#define FALSE			0
#define TRUE			1
#define MAX_CHAR_LINE	127
#define TABSIZE			8
#define BUFSIZE			4096
extern int tabstop;

#define MS_DOS_OUTPUT_CP		437		/* ����ʾ��չAscii��, ��������ʾ����! */
#define CHINESE_OUTPUT_CP		936		/* ����ʾ����, ��������ʾ��չAscii��! */
enum special_char_attr {
	GRID_HORIZONTAL	= 0x0400,			/* ÿ���ַ�������һ��С�̺� */
	GRID_LVERTICAL	= 0x0800,			/* ÿ���ַ�������һ��С���� */
	GRID_RVERTICAL	= 0x1000,			/* ÿ���ַ�������һ��С���� */
	REVERSE_VIDEO	= 0x4000,			/* ��ɫ��ת */
	UNDERSCORE		= 0x8000,			/* ÿ���ַ�������һ��С�̺�(�»���) */
	RESET_NORM_MODE	= 0x0000			/* ȡ���������� */
};

#define PERR(bSuccess, api) {if (!(bSuccess)) perr(__FILE__, __LINE__, \
	api, GetLastError());}
void perr(const char *szFileName, int line, const char *szApiName, int err);/* �������� */
void resize_con_buf_and_window(int xsize, int ysize);						/* �������ڼ����� buffer �Ĵ�С */
void set_codepage(unsigned int codepageID);									/* ������ʾҳ */
unsigned int get_codepage(void);											/* ���ÿ���̨�ı��� */
void set_con_title(const char *title);										/* �����ַ����������Եȵ� */
void set_char_special_mode(enum special_char_attr char_mode);
void reset_char_special_mode(void);
int  get_char_special_mode(void);

void pos_screen2window(int scr_x, int scr_y, int *win_x, int *win_y);		/* �������ܺ���, ����Ļ����ת��Ϊ�������� */
void pos_window2screen(int win_x, int win_y, int *scr_x, int *scr_y);		/* �������ܺ���, ����������ת��Ϊ��Ļ���� */
void Swap(void *item1, void *item2, size_t size);							/* �������ܺ���, �������ͱ������� */
int get_conx(void);															/* ��õ�ǰ����̨���ڵĺ��򳤶� */
int get_cony(void);															/* ��õ�ǰ����̨���ڵ����򳤶� */

void fill_rect_attr(int left, int top, int right, int bottom, int char_attr);	/* �ı�ָ�������ַ�����(���ı䵱ǰ����ɫ����) */
void fill_rect_char(int left, int top, int right, int bottom, int char_attr, int fill_char);/* ���ض������ַ����(ͬ��) */

void box(int left, int top, int right, int bottom, int char_attr, bool single);	/* ���߿� */
void draw3Dwindow(int left, int top, int right, int bottom, int table_attr);	/* ����һ���� 3D Ч���Ļ���� */

void draw_horizen_line(int x1, int y1, int x2, int char_attr, int ch);		/* ��ˮƽ�� */
void draw_vertical_line(int x1, int y1, int y2, int char_attr, int ch);		/* ����ֱ�� */

void set_Nth_bit(void *val, size_t size, int index);						/* �� n λ��λ */
void reset_Nth_bit(void *val, size_t size, int index);						/* �� n λ��λ */
char get_low_byte(void *val, size_t size);									/* ��õ��ֽ� */
char get_high_byte(void *val, size_t size);									/* ��ø��ֽ� */
bool test_Nth_bit(void *val, size_t size, int index);						/* ���Ե� n λ */
void flush_inputbuf(void);													/* ������뻺���� */

enum box_type {	/* ȷ�Ͽ������, �� 3 �� */
	NORM_TYPE,	/* ���׻��� */
	WARN_TYPE,	/* ���׺��� */
	ERROR_TYPE	/* ��װ��� */
};
void comfirm_box(int left, int top, const char *note_msg, enum box_type btype);
#endif
