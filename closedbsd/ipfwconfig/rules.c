/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */

#include "fw.h"

dialogMenuItem rlist[32];

int __insert_rule(struct ruleset rule)
{
	FILE *stream = NULL;
	char cmd[256];
	char buf[256];
	
	stream = fopen(RULESCRIPT, "a+");
	if (stream == NULL) 
		return -1;

	// quick hack.  
	if (rule.action == RESET && rule.proto != TCP) {
		fclose(stream);
		return -1;
	}

	if (rule.action == FWD) {
		switch(rule.proto) {
		case TCP:	
			snprintf(cmd, sizeof(cmd), "/sbin/ipfw -q add fwd %s,%s tcp ", rule.other_a, rule.other_b);
			strcat(cmd, "from ");
			strcat(cmd, rule.src);
			if (strlen(rule.src_port) > 0) { 
				if (strncmp(rule.src_port, "any", 3) == 0) {
					strcat(cmd, " ");
				}
				else {
					strcat(cmd, " ");
					strcat(cmd, rule.src_port);
				}
			}

			strcat(cmd, "to ");
			strcat(cmd, rule.dst);
			if (strlen(rule.dst_port) > 0) { 
				if (strncmp(rule.dst_port, "any", 3) == 0) {
					strcat(cmd, " ");
				}  
				else {
					strcat(cmd, " ");
					strcat(cmd, rule.dst_port);
				}
			}
	
			fprintf(stream, "%s\n", cmd);
			fclose(stream);
			break;
	
		case UDP:
			snprintf(cmd, sizeof(cmd), "/sbin/ipfw -q add fwd %s,%s udp ", rule.other_a, rule.other_b);
			strcat(cmd, "from ");
			strcat(cmd, rule.src);
			if (strlen(rule.src_port) > 0) {
				if (strncmp(rule.src_port, "any", 3) == 0) {
					strcat(cmd, " ");
				}	
				else {		
					strcat(cmd, " ");
					strcat(cmd, rule.src_port);	
				}
			}

			strcat(cmd, "to ");
			strcat(cmd, rule.dst);
			if (strlen(rule.dst_port) > 0) {
				if (strncmp(rule.dst_port, "any", 3) == 0) {
					strcat(cmd, " ");
				}
				else {	
					strcat(cmd, " ");
					strcat(cmd, rule.dst_port);
				}
			}	
			fprintf(stream, "%s\n", cmd);
			fclose(stream);
			break;
		} 	
		snprintf(cmd, sizeof(cmd), "%s", cmd);
		system(cmd);
		return 0;	
	}
	
	switch(rule.proto) {
	case IP:
		switch(rule.action) {
		case ALLOW:
			snprintf(cmd, sizeof(cmd), "/sbin/ipfw -q add allow ip ");
			break;
		case DENY:
			snprintf(cmd, sizeof(cmd), "/sbin/ipfw -q add deny ip ");
			break;	
		case DIVERT:
			snprintf(cmd, sizeof(cmd), "/sbin/ipfw -q add divert 8668 ip ");
			break;
		
		}
		
      strcat(cmd, "from ");
      strcat(cmd, rule.src);
      strcat(cmd, " to ");
      strcat(cmd, rule.dst);
		
		if (rule.ifs == INCOMING)
         strcat(cmd, " in");
      if (rule.ifs == OUTGOING)
         strcat(cmd, " out");
		
		switch(rule.option) {
		case SSRR:
			strcat(cmd, " ipoptions ssrr");
			break;
		case LSRR:
			strcat(cmd, " ipoptions lsrr");
			break;
		case TS:	
			strcat(cmd, " ipoptions ts");
			break;
		}

				
		if (strlen(rule.other_a)) {
			strcat(cmd, " ");
			strcat(cmd, rule.other_a);
		}

		break;

	case TCP:
		switch(rule.action) {
		case ALLOW:
			snprintf(cmd, sizeof(cmd), "/sbin/ipfw -q add allow tcp ");
			break;
		case RESET:
			snprintf(cmd, sizeof(cmd), "/sbin/ipfw -q add reset tcp ");
			break;
		case DENY:
			snprintf(cmd, sizeof(cmd), "/sbin/ipfw -q add deny tcp ");
			break;
		}
		
      strcat(cmd, "from ");
      strcat(cmd, rule.src);

      if (strlen(rule.src_port) > 0) {
         strcat(cmd, " ");
         strcat(cmd, rule.src_port);
      }

      strcat(cmd, " to ");
      strcat(cmd, rule.dst);
  
      if (strlen(rule.dst_port) > 0) {
         strcat(cmd, " ");
         strcat(cmd, rule.dst_port);
      }
		
	 	if (rule.ifs == INCOMING)
         strcat(cmd, " in");
     	if (rule.ifs == OUTGOING)
         strcat(cmd, " out");

		switch(rule.option) {
		case SETUP:
			strcat(cmd, " setup"); 
			break;
		case ESTABLISHED:
			strcat(cmd, " established");
			break;
		case FRAG:	
			strcat(cmd, " frag");
			break;
		}
		break;

	case UDP:
		switch(rule.action) {
		case ALLOW:
			snprintf(cmd, sizeof(cmd), "/sbin/ipfw -q add allow udp ");
			break;
		case DENY:
			snprintf(cmd, sizeof(cmd), "/sbin/ipfw -q add deny udp ");
			break;
		}
		
		strcat(cmd, "from ");
   	strcat(cmd, rule.src);
  
   	if	(strlen(rule.src_port) > 0) {
      	strcat(cmd, " ");
      	strcat(cmd, rule.src_port);
   	}

   	strcat(cmd, " to ");
   	strcat(cmd, rule.dst);
  
   	if (strlen(rule.dst_port) > 0) {
      	strcat(cmd, " ");
      	strcat(cmd, rule.dst_port);
   	}
		
		if (rule.ifs == INCOMING)
         strcat(cmd, " in ");
      if (rule.ifs == OUTGOING)
         strcat(cmd, " out ");

		break;
	
	case ICMP:
		switch(rule.action) {
		case ALLOW:
			snprintf(cmd, sizeof(cmd), "/sbin/ipfw -q add allow icmp ");
			break;
		case DENY:
			snprintf(cmd, sizeof(cmd), "/sbin/ipfw -q add deny icmp ");	
			break;
		}
		
		strcat(cmd, "from ");
		strcat(cmd, rule.src);
		strcat(cmd, " to ");
		strcat(cmd, rule.dst);
	
	 	if (rule.ifs == INCOMING)
         strcat(cmd, " in");
      if (rule.ifs == OUTGOING)
         strcat(cmd, " out");

		switch(rule.option) {
		case ECHO_REQUEST:
			strcat(cmd, " icmptypes 5");
			break;
		case DEST_UNREACH:
			strcat(cmd, " icmptypes 3");
			break;
		case ECHO_REPLY:
 			strcat(cmd, " icmptypes 0");
			break;
		}
	
		break;
	}

	snprintf(buf, sizeof(buf), "%s", cmd);
	system(buf);
	
	fprintf(stream, "%s\n", cmd);
	fclose(stream);

	system("/sbin/chmod 744 /etc/rules");
 
	return 0;
}

   /*
   well, this is how i started out, i left it here because
   i wasn't sure if i wanted to get rid of it or not.  currently, 
   rules are stored in plaintext in /etc/rules as opposed to using
   the .db methods
   */

int insert_rule(struct ruleset rule)
{
	DB *db;
	DBT data, key;
	int id = next_rule();

	db = dbopen(RULESCRIPT, O_CREAT | O_RDWR, 0600, DB_BTREE, NULL);
 
	bzero(&key, sizeof(key));
	key.data = &id;
	key.size = sizeof(rule.id);

	bzero(&data, sizeof(data));		
	data.data = &rule;
	data.size = sizeof(rule);

	if ((db->put)(db, &key, &data, 0)) {
		db->close(db);
		return -1;
	}
	
	db->close(db);
	return 0;
}

int next_rule()
{
	DB *db;
	DBT dbt;
	DBT key;
	int i = 0;

	db = dbopen(RULESCRIPT, O_CREAT | O_RDWR, 0600, DB_BTREE, NULL);
	
	while (db->seq(db, &key, &dbt, R_NEXT) == 0) {
		i += 1;
	}
	
	db->close(db);
	return i;
}

int delete_rule()
{
	FILE *stream = NULL;
	int i = 0;
	char buf[256];
	char pr[3];
	char *s;

	dialog_clear();

	stream = fopen(RULESCRIPT, "r");
	if (stream == NULL) {
		end_dialog();
		return -1;
	}

	dialog_clear();

	bzero(rlist, sizeof(rlist));

   while (fgets(buf, sizeof(buf), stream) != NULL) {
      if (strlen(buf) > 1) {
         buf[strlen(buf) - 1] = '\0';
         if (i < 9)
            snprintf(pr, sizeof(pr), "0%d", i+1);
         else
            snprintf(pr, sizeof(pr), "%d", i+1);
  
			strtok(strdup(buf), "q");
			s = strtok(NULL, "q");
			rlist[i].prompt  = strdup(pr);
			rlist[i].title   = s;
			rlist[i].checked = NULL;
			rlist[i].fire    = drop_rule;
         i++;
      }
   }

	if (i == 0) { 
		dialog_msgbox("ClosedBSD", "There are no rules to delete    ", -1, -1, 1);
		fclose(stream);
		return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;	
	}	 
	
	fclose(stream);
	dialog_menu("ClosedBSD", "Please select a rule to remove", -1, -1, i, -i, rlist, NULL, NULL, NULL);

   return 0;
}

// add a ruleset to the file, and flush/reload the rules
int raw_add_rule(char *rule)
{
	FILE *stream = NULL;
	
	stream = fopen(RULESCRIPT, "a+");
	if (stream == NULL)
		return -1;

	fprintf(stream, "/sbin/ipfw -q %s\n", rule);
	fclose(stream);

	system("/sbin/ipfw -q flush");
	system("/bin/sh /etc/rules");

	return 0;
}

// kind of nasty.. 
int drop_rule(dialogMenuItem *self)
{
   FILE *stream = NULL;
   int d = atoi(self->prompt);
   int i = 0;
 
	unlink(RULESCRIPT);
   stream = fopen(RULESCRIPT, "a+");
   if (stream == NULL)
      return -1;

   for (i = 0; i <= 50; ++i) 
      if (rlist[i].title)
         if (atoi(rlist[i].prompt) != d)
            fprintf(stream, "/sbin/ipfw -q%s\n", rlist[i].title);

   fclose(stream);

	system("/sbin/ipfw -q flush");
	system("/sbin/chmod 755 /etc/rules");
	system("/bin/sh /etc/rules");

	dialog_clear();
	dialog_msgbox("ClosedBSD", "Rule deleted     ", -1, -1, 1);
	return 0; 
}

