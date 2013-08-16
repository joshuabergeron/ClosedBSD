/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */
#include "fwdb.h"

int 
natset(void)
{
	FILE *stream = NULL;
	char buf[256];
	char nat[256];

	stream = fopen(NAT, "r");
	if (stream == NULL)
		return -1;

	while (fgets(buf, sizeof(buf), stream) != NULL) {
		if (strlen(buf) >= 3) {
			snprintf(nat, sizeof(nat), "/sbin/natd -n %s\n", buf);
			break;
		}
	}

	system(nat);
	return 0;
}

int 
ruleset(void)
{
	FILE *stream = NULL;
	char buf[256];
	char nat[256];

	/* first add divert */
	stream = fopen(NAT, "r");
	if (stream) {
		while (fgets(buf, sizeof(buf), stream) != NULL)
			snprintf(nat, sizeof(nat), "/sbin/ipfw -q add 85 divert 8668 ip from any to any via %s", buf);

		fclose(stream);
		system(nat);
	}
	/* then the ruleset */
	stream = fopen("/etc/rules", "r");
	if (stream == NULL)
		return -1;

	while (fgets(buf, sizeof(buf), stream) != NULL) {
		system(buf);
		bzero(buf, sizeof(buf));
	}

	return 0;
}

#if 0
int 
ruleset(void)
{
	DB *db;
	DBT dbt;
	DBT key;
	struct device u;
	struct ruleset rule;
	char cmd[1024];
	char nat[64];
	int done = 0;
	char from[64];
	char other_a[256];
	char other_b[256];

	bzero(&rule, sizeof(rule));
	bzero(&dbt, sizeof(dbt));
	bzero(&key, sizeof(key));

	fprintf(stdout, "configuring firewall:\n\n");
	fflush(stdout);
	system("/sbin/ipfw -f flush >> /dev/null");

	db = dbopen(RULES, O_RDWR, 0644, DB_BTREE, NULL);

	while (db->seq(db, &key, &dbt, R_NEXT)== 0){
		/* so much bs. */
		bzero(&rule, sizeof(rule));
		bzero(other_a, sizeof(other_a));
		bzero(cmd, sizeof(cmd));

		bcopy(dbt.data, &rule, dbt.size);

		if (strlen(rule.src) && strlen(rule.dst)) {
#ifdef DEBUG
			printf("key: %d\n", rule.id);
#endif
			bzero(cmd, sizeof(cmd));
			snprintf(cmd, sizeof(cmd), "/sbin/ipfw ");
			switch (rule.cmd) {
			case 0:
				strcat(cmd, "add ");
				break;
			case 1:
				strcat(cmd, "-f flush ");
				break;
			}
#ifdef DEBUG
			printf("%s\n", cmd);
#endif
			switch (rule.action) {
			case 0:
				strcat(cmd, "allow ");
				break;
			case 1:
				strcat(cmd, "deny ");
				break;
			case 2:
				strcat(cmd, "divert 8668 ");
				break;
			case 3:
				strcat(cmd, "fwd ");
				break;
			}
#ifdef DEBUG
			printf("%s\n", cmd);
#endif
			switch (rule.proto) {
			case 0:
				strcat(cmd, "ip ");
				break;
			case 1:
				strcat(cmd, "tcp ");
				break;
			case 2:
				strcat(cmd, "udp ");
				break;
			case 3:
				strcat(cmd, "icmp ");
				break;
			}
#ifdef DEBUG
			printf("%s\n", cmd);
#endif
			snprintf(from, sizeof(from), "from %s to %s ", rule.src, rule.dst);
			strcat(cmd, from);

#ifdef DEBUG
			printf("%s\n", cmd);
#endif

			if (strlen(rule.other_a) < 1)
				done = 1;
			else {
				snprintf(other_a, sizeof(other_a), "%s ", rule.other_a);
				strcat(cmd, other_a);
			}
#ifdef DEBUG
			printf("DONE\n");
#endif

			strcat(cmd, " >> /dev/null");
			system(cmd);
		}
	}
	system("/sbin/ipfw list");
	fprintf(stdout, " \n");
	fflush(stdout);

	db->close(db);

	bzero(&u, sizeof(u));
	bzero(&dbt, sizeof(dbt));
	bzero(&key, sizeof(key));

	db = dbopen("/etc/devices.db", O_RDONLY, 0644, DB_BTREE, NULL);

	while (db->seq(db, &key, &dbt, R_NEXT)== 0){
		bcopy(dbt.data, &u, dbt.size);

		if (strlen(u.gateway) && (u.ext == 1)) {
			fprintf(stdout, "\n");
			fprintf(stdout, "enabling natd on device: %s", u.device);
			snprintf(nat, sizeof(nat), "/sbin/natd -n %s", u.device);
			system(nat);
			fflush(stdout);
		}
	}

	db->close(db);

	return 0;
}

#endif
