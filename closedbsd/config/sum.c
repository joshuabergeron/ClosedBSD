/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */

#include "main.h"
#include "db.h"

int 
summary(void)
{
	WINDOW *w;
	int i = 3;
	int f, n, k;
	FILE *stream = NULL;
	DB *db;
	struct device d;
	DBT dbt;
	DBT key;
	char buf[256];
	char *s;

	dialog_clear();

	/* initialize summary box */
	w = newwin(22, 74, 2, 2);
	draw_box(w, 0, 0, 22, 74, dialog_attr, border_attr);
	draw_shadow(w, 0, 0, 23, 75);
	wattrset(w, title_attr);
	mvwprintw(w, 0, 18, "[ ClosedBSD: Configuration summary ]");
	wattrset(w, dialog_attr);

	/* initialize dev db */
	bzero(&d, sizeof(d));
	bzero(&dbt, sizeof(dbt));
	bzero(&key, sizeof(key));
	db = dbopen(DEVICES, O_RDWR, 0600, DB_BTREE, NULL);

	mvwprintw(w, 2, 8, "%c", 218);
	mvwprintw(w, 2, 65, "%c", 191);
	for (n = 9; n < 65; n++)
		mvwprintw(w, 2, n, "%c", 196);

	mvwprintw(w, 2, 27, "%c", 194);
	mvwprintw(w, 2, 46, "%c", 194);

	wattrset(w, title_attr);
	mvwprintw(w, 2, 14, " Device ");
	mvwprintw(w, 2, 35, " IP ");
	mvwprintw(w, 2, 52, " Netmask ");
	wattrset(w, dialog_attr);

	while (db->seq(db, &key, &dbt, R_NEXT)== 0){
		bcopy(dbt.data, &d, dbt.size);
		mvwprintw(w, i, 8, "%c %16s %c %16s %c %16s %c ", 179, d.device, 179, d.ip, 179, d.netmask, 179);
		i += 1;
	}

	mvwprintw(w, i, 8, "%c", 192);
	mvwprintw(w, i, 65, "%c", 217);
	for (n = 9; n < 65; n++)
		mvwprintw(w, i, n, "%c", 196);

	mvwprintw(w, i, 27, "%c", 193);
	mvwprintw(w, i, 46, "%c", 193);

	f = i + 4;

	stream = fopen("/etc/rules", "r");
	if (!stream)
		mvwprintw(w, f, 6, "There are currently no firewall rules to display");
	else {
		mvwprintw(w, i + 2, 6, "Firewall rules currently in place: ");

		while (fgets(buf, sizeof(buf), stream) != NULL && f < 21) {
			strtok(strdup(buf), "q");
			s = strtok(NULL, "q");

			s[strlen(s) - 1] = '\0';
			mvwprintw(w, f, 6, "%s", s);

			bzero(buf, sizeof(buf));
			f++;
		}
	}

	mvwprintw(w, 22, 37, "Press any key to exit.");

	wmove(w, 0, 0);
	wrefresh(w);
	db->close(db);

	while ((k = wgetch(w) != ERR)) {
		switch (k) {
		default:
			dialog_clear();
			return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
		}
	}

	return DITEM_SUCCESS | DITEM_RESTORE | DITEM_CONTINUE;
}
