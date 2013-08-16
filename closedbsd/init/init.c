#include <sys/types.h>
#include <sys/wait.h>
#include <sys/consio.h>
#include <libutil.h>
#include <sys/ttycom.h>
#include <sys/signal.h>
#include <sys/fcntl.h>
#include <signal.h>
#include <pwd.h>
#include <errno.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dialog.h>
#include <curses.h>
#include "ifdb.h"

#define CVERSION "1.0-RC1"
#define STOCK_ETC "/defaults/etc/"
#define TRU_RC "/etc/rc"

int vty = 0;
int do_single();
int do_rc();
enum { IMPORT_BOOT, CD_BOOT };
int boot_params = CD_BOOT;
char *floppy_device = "/dev/fd0";

int _menu1_bo(dialogMenuItem *self)
{
   boot_params = CD_BOOT;
   return 0;
}

int _menu1_im(dialogMenuItem *self)
{
   int ret;

	boot_params = IMPORT_BOOT;

	dialog_clear();	
	dialog_msgbox("ClosedBSD", "Please insert the floppy diskette with saved settings", -1, -1, -1);

	if ((ret = system("/sbin/mount /dev/fd0 /mnt")) == 0) 
		system("/sbin/restoretc");
	else
		dialog_msgbox("ClosedBSD", "Unable to restore settings. \n\nContact bugs@closedbsd.org!", -1, -1, -1);

	return 0;
}

int _menu1_op(dialogMenuItem *self)
{
   return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}

dialogMenuItem menu1[] = {
   { "Boot", "Boot the CD without loading saved settings", NULL, _menu1_bo },
   { "Import", "Import settings from floppy and boot the CD", NULL, _menu1_im },
};

int bootmenu(void)
{
   int ret;
   ret = dialog_menu("ClosedBSD Boot Menu",
                     "Please select the appropriate option:",
                     -1, -1, 2, -2, menu1, NULL, NULL, NULL);
   return ret;
}

int main(int argc, char **argv)
{
	pid_t pid;
	int rv;

	signal(SIGPIPE, SIG_IGN);

	putenv("TERM=cons25");
	switch(pid = fork()) {
		case -1:
			exit(40);
		case 0:
			rv = do_rc();
		default:
			wait(&rv);
			do_single();	
	}

	return 20;
}


int do_rc()
{
	int fd;	
	char *argg[3];
	int ret = 0;

	close(0);
	close(1);
	close(2);
	fd = open("/dev/console", O_RDWR);
	dup2(fd, 0);	
	dup2(fd, 1);
	dup2(fd, 2);

	login_tty(fd);
	setpgid(0, getpid());

	putenv("TERM=cons25");
	putenv("HOME=/");
	putenv("PATH=/bin:/sbin:/usr/bin:/usr/sbin");

	signal(SIGHUP, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGTERM, SIG_DFL);

	fprintf(stdout, "mounting /etc filesystem... done\n"); fflush(stdout);
   system("/sbin/mount_mfs -s 8192 swap /etc");
   fprintf(stdout, "mounting /var filesystem... done\n"); fflush(stdout);
   system("/sbin/mount_mfs -s 8192 swap /var");
   fprintf(stdout, "mounting /tmp filesystem... done\n"); fflush(stdout);
   system("/sbin/mount_mfs -s 8192 swap /tmp");
   fflush(stdout);

	system("/bin/cp /defaults/etc/* /etc/");
   system("/bin/cp -R /defaults/var/* /var/");

   init_dialog();
   dialog_clear();
   ret = bootmenu();
   endwin();


	if (ret == 0)
      switch(boot_params) {
         case CD_BOOT:
            system("/bin/cp /defaults/etc/* /etc/");
   			system("/bin/cp -R /defaults/var/* /var/");
				break;
         
			case IMPORT_BOOT:
            fprintf(stdout, "loading /etc contents... done"); fflush(stdout);
            break;
      }
	
	argg[0] = "-sh";
	argg[1] = "/etc/rc";
	argg[2] = 0;
	execv("/bin/sh", argg);
	
	close(fd);
	return 20;
}

int do_single()
{
	pid_t pid;
	int fd;
	char *arg[2];
	int rv, ret;
	char user[128];
	char *pass;
	struct passwd *pwd;
	char *ep, *salt;

	close(0);
	close(1);
	close(2);
	fd = open("/dev/ttyv0", O_RDWR);
	dup2(fd, 0);	
	dup2(fd, 1);
	dup2(fd, 2);

	login_tty(fd);

	setsid();
	setpgid(0, getpid());

	putenv("TERM=cons25");
	putenv("HOME=/");
	putenv("PATH=/bin:/sbin:/usr/bin:/usr/sbin");

	// device configuration 
	fflush(stdout);
	config_devices();

	// firewall configuration 
	ruleset();	
	natset();
	
	while (1) {
		switch(pid = fork()) {
			case -1: 
				return 5;
				break;
			
			case 0:
				rv = 10;
				signal(SIGSEGV, SIG_IGN);
				signal(SIGSTOP, SIG_IGN);	
				signal(SIGHUP, SIG_DFL);
				signal(SIGINT, SIG_DFL);
				signal(SIGQUIT, SIG_DFL);
				signal(SIGTERM, SIG_DFL);
	
				printf("ClosedBSD %s [ttyv0]\n\n", CVERSION);
				printf("login: ");
				scanf("%s", user);
				pass = getpass("password: ");
 				printf("\n\n");

				if (strlen(user) <= 0) 
					exit(rv);
				
				if (strlen(user) > 0) {
					pwd = getpwnam(user); 
					if (pwd) {
						salt = pwd->pw_passwd;	
						ep = crypt(pass, salt);
				
						if (strlen(salt) == 0) {
							system("/sbin/cat /etc/motd");
							printf("please set the root password with 'passwd'\n\n");
							chdir("/");
							arg[0] = "-sh";
							arg[1] = 0;
							execv("/bin/sh", arg);
							// HERE
							exit(rv);
						}
						else if (strlen(salt) > 0) {
							switch(ret = strcmp(ep, pwd->pw_passwd)) {
								case 1:
									printf("invalid login\n\n");
									exit(rv);
								case 0:
									system("/sbin/cat /etc/motd");
									chdir("/");
									// spawn a shell
									arg[0] = "-sh";
									arg[1] = 0;
									execv("/bin/sh", arg);
									exit(rv);
								case -1:
									printf("invalid login\n\n");
									exit(rv);
								default:
									printf("invalid login\n\n");
									exit(rv);
							}
							break;
						}
					}
					else {
						printf("invalid login\n\n");
						exit(rv);
					}
				}
			default:
				wait(&rv);
				rv = 0;
				break;
		}
	}
}

