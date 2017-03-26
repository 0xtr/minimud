#include "../mapping/insert_room.h"

int32_t insert_room(struct NewRoom rconfig)
{
	uint8_t *sqlerr = NULL;
	uint8_t param_x[2 * sizeof(rconfig.coords.x)] = {0};
	uint8_t param_y[2 * sizeof(rconfig.coords.y)] = {0};
	uint8_t param_z[2 * sizeof(rconfig.coords.z)] = {0};
	snprintf((char *)param_x, sizeof(rconfig.coords.x), "%d", rconfig.coords.x);
	snprintf((char *)param_y, sizeof(rconfig.coords.y), "%d", rconfig.coords.y);
	snprintf((char *)param_z, sizeof(rconfig.coords.z), "%d", rconfig.coords.z);

	uint8_t *querystr = (uint8_t *)sqlite3_mprintf(
		"INSERT OR IGNORE INTO ROOMS (name, desc, x, y, z, north, east, south, west, up, down, "
		"northeast, southeast, southwest, northwest, owner, last_modified_by, flags)"
		"VALUES (%Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q);", 
		(char *)rconfig.name, (char *)rconfig.desc, 
		param_x, param_y, param_z, 
		"0", "0", "0", "0", "0", "0", "0", "0", "0", "0", 
		(char *)rconfig.owner, (char *)rconfig.owner, (char *)rconfig.flags);

	if (sqlite3_exec(get_roomdb(), (char *)querystr, room_callback, 0, (char **)sqlerr) != SQLITE_OK) {
		fprintf(stdout, "SQLITE3 room_insert error!\n%s\n", sqlite3_errmsg(get_roomdb()));
		sqlite3_free(sqlerr);
		return EXIT_FAILURE;
	}
	sqlite3_free(querystr);

	return EXIT_SUCCESS;
}
