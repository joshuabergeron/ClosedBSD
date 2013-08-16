#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <db.h>

#define RULES "/etc/rules.db"
#define NAT "/etc/nat" 

enum { ADD, FLUSH };
enum { ALLOW, DENY, DIVERT, FWD };
enum { IP, TCP, UDP, ICMP };

struct ruleset {
	int id;
	int cmd;
	int action;
	int proto;
	char src[16];
	char dst[16];
	char other_a[256];
	char other_b[256];
};
	
int insert_rule(DB *db, struct ruleset r);

