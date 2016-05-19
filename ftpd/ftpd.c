#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <stdarg.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h>
#include "dxyh.h"
//#include "dxyh_thread.h"
#include "ftpd.h"
//#include "error.h"
//#include "record.h"

extern log_t* logfd;

static void ftpd_usage(void);
static void ftpd_verbose(void);
static void ftpd_help(void);
static void ftpd_sig_chld(int signo);
static void ftpd_sig_int(int signo);
static void ftpd_chld_sig_quit(int signo);
static void ftpd_ctrl_conn_handler(int ctrlfd);
static const char* ftpd_serv_resp_num2msg(int num);
int ftpd_send_resp(int ctrlfd,int num, ...);
static void parent_atlast(void);


int			ftpd_debug_on;
int			ftpd_record_on;
int			ftpd_quit_flag;
int			ftpd_hash_print;
int			ftpd_tick_print;
uint16_t	ftpd_serv_port; 
char		ftpd_cur_dir[PATH_MAX];
int			ftpd_cur_pasv_fd;
int			ftpd_cur_pasv_connfd;
int			ftpd_cur_port_fd;
int			ftpd_cur_type;
const struct ftpd_user_st *ftpd_cur_user;
int			ftpd_nchild;
pid_t pids[MAX_CHIHLD_NUM];

/*
 *parent's SIGINT handler
 */
static void ftpd_sig_int(int signo)
{
	int i;
	FTPD_DEBUG_LOG(ERROR,"ftpd interrupted by signal SIGINT!\n");
	for (i=1;i<ftpd_nchild+1;++i)
	{
		if(pids[i]!=-1)
		{
			kill(pids[i],SIGQUIT);
		}
	}
	exit(EXIT_FAILURE);
}

static void parent_atlast(void)
{
	FTPD_DEBUG_LOG(INFO,"Server is shutdown!\n");
	if(ftpd_record_on)	/* close the log file if necessary*/
	{
		log_close(logfd);
	}
}

void ftpd_parse_args(int argc,char **argv)
{
	int do_verbose,do_help;
	int err_flg;
	char c,log_filename[PATH_MAX];
	struct option longopts[]={
		{"port",required_argument,NULL,'p'},
		{"debug",no_argument,NULL,'d'},
		{"record",optional_argument,NULL,'r'},
		{"verbose",no_argument,&do_verbose,1},
		{"help",no_argument,&do_help,1},
		{0,0,0,0}
	};

	do_verbose=OFF;
	do_help =OFF;
	err_flg=0;

	while((c=getopt_long(argc,argv,":hr::vp::dW;",longopts,NULL))!=-1) {
		switch (c) {
			case 'd':
			ftpd_debug_on=ON;
			ftpd_hash_print=ON;	/*show '#' within transfer*/
			ftpd_tick_print=ON;	/*try it, you will konw*/
			break;
		}
	}

	if(err_flg) {
		//ftpd_usage();
		exit(EXIT_FAILURE);
	}
}

/*
 * initialization,add parent-process's init stuff here
 */
void ftpd_init(void)
{
	int i;
	ftpd_debug_on=OFF;/* default is OFF*/
	ftpd_record_on =OFF;
	ftpd_hash_print=OFF;
	ftpd_tick_print=OFF;
	ftpd_quit_flag=0;
	ftpd_serv_port=SERV_PORT;/* defined in dxyh.h, 9877 as def*/
	ftpd_cur_port_fd=-1;
	ftpd_cur_pasv_fd=-1;
	ftpd_cur_type=TYPE_A;
	pids[0]=getpid();
	printf("%d\n", pids[0]);
	for (i=1;i<MAX_CHIHLD_NUM;++i)
	{
		pids[i]=-1;
	}
	ftpd_nchild=0;
	signal(SIGPIPE,SIG_IGN);/*ignore signal SIG_IGN*/
	//signal(SIGCHLD,ftpd_sig_chld);/*install other signals ' handler*/
	//signal(SIGINT,ftpd_sig_int);
	//my_lock_mutex_init();	/*initialize the mutex-lock*/
	atexit(parent_atlast);	/*show something or do some cheanup */
}

/*
*parent process create a listen fd
*/
int ftpd_create_serv()
{
	int listenfd;
	const int on=1;
	struct sockaddr_in servaddr;

	listenfd=socket(AF_INET,SOCK_STREAM,0);

	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(ftpd_serv_port);
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	bind(listenfd,(SA*)&servaddr,sizeof(servaddr));

	listen(listenfd,LISTENQ);
	printf("leave ftpd_create_serv listenfd=%d\n",listenfd);
	return listenfd;
}

/*
*main loop server is always listening and fork a child to handle
*the new client
*/

int ftpd_do_loop(int listenfd)
{
	int ctrlfd;
	pid_t childpid;

	for (;;) {
		printf("wait client connect!!!\n");
		if (-1 == (ctrlfd=accept(listenfd,NULL,NULL))) {
			printf("accept error\n");
			continue;
		}

		if (ftpd_debug_on) {
			struct sockaddr_in clitaddr;
			socklen_t clitlen;
			getpeername(ctrlfd,(SA*)&clitaddr,&clitlen);
		}

		if(-1 == (childpid=fork())) {
			continue;
		}else if (0 == childpid) {
			close(listenfd);
			signal(SIGCHLD,SIG_IGN);
			signal(SIGINT,SIG_IGN);
			//signal(SIGQUIT,ftpd_chld_sig_quit);
			//Chdir("/");
			//getcwd
		}
		close(ctrlfd);
	}
}