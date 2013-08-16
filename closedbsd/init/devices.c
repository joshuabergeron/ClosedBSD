#include "ifdb.h"

#define DATABASE "/etc/devices.db"

int config_devices()
{
	DB *db;
	DBT dbt;
	DBT key;
	struct device u;
	char buf[256];
	char cmd[256];
	struct stat st;
	
	bzero(&u, sizeof(u));
	bzero(&dbt, sizeof(dbt));
	bzero(&key, sizeof(key));

	fprintf(stdout, "\n");
	fprintf(stdout, "reading device database...");
	fflush(stdout);
	sleep(1);
	fprintf(stdout, " done\n");
	fflush(stdout);

	fprintf(stdout, "configuring devices: \n");
	fflush(stdout);
	db = dbopen(DATABASE, O_RDONLY, 0644, DB_BTREE, NULL);
	while (db->seq(db, &key, &dbt, R_NEXT) == 0) {
		bcopy(dbt.data, &u, dbt.size);
		
		switch(u.id) {
		case 0:
			fprintf(stdout, "\n");
			fprintf(stdout, "\tdevice: %s\n", u.device);
			fprintf(stdout, "\t\t[ip addr] %s\n", u.ip);
			fprintf(stdout, "\t\t[netmask] %s\n", u.netmask);
			if (strlen(u.gateway) && (u.ext == 1)) 
				fprintf(stdout, "\t\t[gateway] %s\n", u.gateway);

			snprintf(buf, sizeof(buf), "/sbin/ifconfig %s %s netmask %s", u.device, u.ip, u.netmask);
			system(buf);

			if (strlen(u.gateway) && (u.ext == 1)) {
            snprintf(cmd, sizeof(cmd), "/sbin/route add default %s >>/dev/null", u.gateway);
            system(cmd);
         }

			bzero(buf, sizeof(buf));
			fflush(stdout);
			break;
		
		default:
			fprintf(stdout, "\n");
		
			switch (u.dhcp) {	
			default:
				fprintf(stdout, "\tdevice: %s\n", u.device);
				fprintf(stdout, "\t\t[ip addr] %s\n", u.ip);
				fprintf(stdout, "\t\t[netmask] %s\n", u.netmask);
				if (strlen(u.gateway)) 
					fprintf(stdout, "\t\t[gateway] %s\n", u.gateway);
		
				snprintf(buf, sizeof(buf),"/sbin/ifconfig %s %s netmask %s", u.device, u.ip, u.netmask);
            system(buf);
	
				if (strlen(u.gateway) && (u.ext == 1)) {
					snprintf(cmd, sizeof(cmd), "/sbin/route add default %s >>/dev/null", u.gateway);
					system(cmd);
				}

				bzero(buf, sizeof(buf));
				fflush(stdout);	
				break;
			
			case DHCP_ENABLED:
				fprintf(stdout, "\tdevice: %s\n", u.device);
				fflush(stdout);
			
				if (stat("/etc/dhclient.conf", &st) == 0)
					snprintf(cmd, sizeof(cmd), "/sbin/dhclient -cf /etc/dhclient.conf");
				else
					snprintf(cmd, sizeof(cmd), "/sbin/dhclient %s", u.device);
				
				system(cmd); 
				break;
			}
		}	
	}

	fprintf(stdout, "\n");
	fflush(stdout);

	db->close(db);
	return 0;
}

