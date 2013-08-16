/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/time.h>

char salt[10];
struct timeval tv;

static unsigned char itoa64[] =
"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

void 
to64(char *s, long v, int n)
{
	while (--n >= 0) {
		*s++ = itoa64[v & 0x3f];
		v >>= 6;
	}
}

/* Generate DES Salt */
char *
dessalt(void)
{
	srandomdev();
	gettimeofday(&tv, 0);

	to64(&salt[0], random(), 3);
	to64(&salt[3], tv.tv_usec, 3);
	to64(&salt[6], tv.tv_sec, 2);
	salt[8] = '\0';

	return (char *)&salt;
}

int 
main(int argc, char *argv[])
{
	char *s;
	FILE *stream = NULL;
	FILE *stream2 = NULL;
	struct passwd *pw;
	int pstat;
	char buf[1024];
	char tmpbuf[1024];
	char modbuf[1024];
	char *uname;
	pid_t pid;
	char *quiox;

	s = dessalt();
	s[strlen(s) - 2] = '\0';

	pw = getpwnam("root");

	pw->pw_change = 0;
	pw->pw_passwd = crypt(getpass("New root password: "), s);
	snprintf(modbuf, sizeof(modbuf), "%s:%s:%d:%d::0:0:%s:%s:%s\n", pw->pw_name, pw->pw_passwd, pw->pw_uid, pw->pw_gid, pw->pw_gecos, pw->pw_dir, pw->pw_shell);

	stream = fopen("/etc/master.passwd", "r+");
	stream2 = fopen("/etc/mpw.tmp", "a+");
	while (fgets(buf, sizeof(buf), stream) != NULL) {
		snprintf(tmpbuf, sizeof(tmpbuf), buf);
		uname = strtok(tmpbuf, ":");
		if ((strcmp(uname, pw->pw_name) == 0)) {
			fprintf(stream2, "%s", modbuf);
			bzero(buf, sizeof(buf));
		}
		fprintf(stream2, "%s", buf);
	}
	fclose(stream2);
	fclose(stream);

	printf("passwd: updating the database\n");
	if (system("/sbin/pwd_mkdb /etc/mpw.tmp") == -1) {
		printf("passwd: error making db\n");
		unlink("/etc/mpw");
		return -1;
	}
	printf("passwd: writing information to floppy\n");
	printf("please insert the ClosedBSD diskette and press a key\n");
	if (getchar() > 0) {
		if (system("/sbin/writetcnp") == -1) {
			printf("passwd: error writing to floppy\n");
			unlink("/etc/mpw.tmp");
			return -1;
		}
	}
	unlink("/etc/mpw.tmp");
	printf("passwd: done\n");
	return 0;

}
