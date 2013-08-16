/*
 * Copyright (c) 2002 The ClosedBSD Project. All Rights Reserved.
 *
 * Please see the file COPYRIGHT for copyright and redistribution information.
 */

#include "db.h"

int 
insert_device(struct device d)
{
	char buf[256];
	DBT data, key;
	DB *db;

	db = dbopen(DEVICES, O_CREAT | O_RDWR, 0600, DB_BTREE, NULL);

	bzero(&key, sizeof(key));
	key.data = &d.id;
	key.size = sizeof(d.id);

	bzero(&data, sizeof(data));
	data.data = &d;
	data.size = sizeof(d);

	if ((db->put)(db, &key, &data, 0))
		return -1;

	if ((db->close)(db))
		return -1;

	if (d.dhcp == DHCP_DISABLED) {
		if (d.ext == 1) {
			snprintf(buf, sizeof(buf), "/sbin/ifconfig %s %s %s", d.device, d.ip, d.netmask);
			system(buf);

			snprintf(buf, sizeof(buf), "/sbin/route delete default >>/dev/null");
			system(buf);

			snprintf(buf, sizeof(buf), "/sbin/route add default %s >>/dev/null", d.gateway);
			system(buf);
		} else {
			snprintf(buf, sizeof(buf), "/sbin/ifconfig %s %s %s", d.device, d.ip, d.netmask);
			system(buf);
		}
	}
	if (d.dhcp == DHCP_ENABLED) {
		snprintf(buf, sizeof(buf), "/sbin/dhclient %s\n", d.device);
		system(buf);
	}
	return 0;
}

int 
unset_ext(int device_id)
{
	DB *db;
	struct device d;
	DBT dbt;
	DBT key;

	db = dbopen(DEVICES, O_RDWR, 0600, DB_BTREE, NULL);

	while (db->seq(db, &key, &dbt, R_NEXT)== 0){
		bcopy(dbt.data, &d, dbt.size);

		if (d.id == device_id) {
			d.ext = 0;
			insert_device(d);
			return 0;
		}
	}
	db->close(db);

	return -1;
}

char *
dev_ext()
{
	DB *db;
	struct device d;
	DBT dbt;
	DBT key;
	char *p;

	bzero(&d, sizeof(d));
	bzero(&dbt, sizeof(dbt));
	bzero(&key, sizeof(key));

	db = dbopen(DEVICES, O_RDWR, 0600, DB_BTREE, NULL);

	while (db->seq(db, &key, &dbt, R_NEXT)== 0){
		bcopy(dbt.data, &d, dbt.size);

		if (d.ext == 1) {
			db->close(db);

			p = strdup(d.device);
			return (char *)p;
		}
	}
	db->close(db);

	return (NULL);
}

int 
ext_exist()
{
	DB *db;
	struct device d;
	DBT dbt;
	DBT key;

	bzero(&d, sizeof(d));
	bzero(&dbt, sizeof(dbt));
	bzero(&key, sizeof(key));

	db = dbopen(DEVICES, O_RDWR, 0600, DB_BTREE, NULL);

	while (db->seq(db, &key, &dbt, R_NEXT)== 0){
		bcopy(dbt.data, &d, dbt.size);

		if (d.ext == 1) {
			db->close(db);

			return d.id;
		}
	}

	db->close(db);

	return -1;
}

int 
next_device_id()
{
	DB *db;
	struct device d;
	DBT dbt;
	DBT key;
	int i = 0;

	bzero(&d, sizeof(d));
	bzero(&dbt, sizeof(dbt));
	bzero(&key, sizeof(key));

	db = dbopen(DEVICES, O_RDWR, 0600, DB_BTREE, NULL);

	while (db->seq(db, &key, &dbt, R_NEXT)== 0)
		i += 1;

	db->close(db);

	return i;
}

char *
dtoi(char *device)
{
	DB *db;
	struct device d;
	DBT dbt;
	DBT key;

	bzero(&d, sizeof(d));
	bzero(&dbt, sizeof(dbt));
	bzero(&key, sizeof(key));

	db = dbopen(DEVICES, O_RDWR, 0600, DB_BTREE, NULL);

	while (db->seq(db, &key, &dbt, R_NEXT)== 0){
		bcopy(dbt.data, &d, dbt.size);

		if (!strcmp(d.device, device)) {
			db->close(db);

			return strdup(d.ip);
		}
	}

	db->close(db);

	return NULL;
}

char *
dev_ip(char *device)
{
	DB *db;
	struct device d;
	DBT dbt;
	DBT key;

	bzero(&d, sizeof(d));
	bzero(&dbt, sizeof(dbt));
	bzero(&key, sizeof(key));
	db = dbopen(DEVICES, O_RDWR, 0600, DB_BTREE, NULL);

	while (db->seq(db, &key, &dbt, R_NEXT)== 0){
		bcopy(dbt.data, &d, dbt.size);
		if (!strcmp(d.device, device)) {
			if (strlen(d.ip) > 1) {
				db->close(db);

				return strdup(d.ip);
			}
		}
	}

	db->close(db);

	return (NULL);

}

char *
dev_netmask(char *device)
{
	DB *db;
	struct device d;
	DBT dbt;
	DBT key;

	bzero(&d, sizeof(d));
	bzero(&dbt, sizeof(dbt));
	bzero(&key, sizeof(key));
	db = dbopen(DEVICES, O_RDWR, 0600, DB_BTREE, NULL);

	while (db->seq(db, &key, &dbt, R_NEXT)== 0){
		bcopy(dbt.data, &d, dbt.size);
		if (!strcmp(d.device, device)) {
			if (strlen(d.netmask) > 1) {
				db->close(db);

				return strdup(d.netmask);
			}
		}
	}
	db->close(db);

	return (NULL);
}

char *
dev_gateway(char *device)
{
	DB *db;
	struct device d;
	DBT dbt;
	DBT key;

	bzero(&d, sizeof(d));
	bzero(&dbt, sizeof(dbt));
	bzero(&key, sizeof(key));
	db = dbopen(DEVICES, O_RDWR, 0600, DB_BTREE, NULL);

	while (db->seq(db, &key, &dbt, R_NEXT)== 0){
		bcopy(dbt.data, &d, dbt.size);
		if (!strcmp(d.device, device)) {
			if (strlen(d.gateway) > 1) {
				db->close(db);

				return strdup(d.gateway);
			}
		}
	}
	db->close(db);

	return (NULL);
}

int 
device_exist(char *device)
{
	DB *db;
	struct device d;
	DBT dbt;
	DBT key;

	bzero(&d, sizeof(d));
	bzero(&dbt, sizeof(dbt));
	bzero(&key, sizeof(key));

	db = dbopen(DEVICES, O_RDWR, 0600, DB_BTREE, NULL);

	while (db->seq(db, &key, &dbt, R_NEXT)== 0){
		bcopy(dbt.data, &d, dbt.size);
		if (!strcmp(d.device, device)) {
			db->close(db);

			return d.id;
		}
	}
	db->close(db);

	return -1;
}
