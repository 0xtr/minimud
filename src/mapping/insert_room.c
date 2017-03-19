#include "../mapping/insert_room.h"

int32_t insert_room(struct NewRoom rconfig)
{
	uint8_t *sqlerr = NULL;

	struct RoomRecord *map = lookup_room(rconfig.coords);

	if (map != NULL) {
		free(map);
		return EXIT_SUCCESS;
	}

	uint8_t *querystr = (uint8_t *)sqlite3_mprintf(
		"INSERT INTO ROOMS (name, desc, x, y, z, north, east, south, west, up, down, "
		"northeast, southeast, southwest, northwest, owner, last_modified_by, flags)"
		"VALUES (%Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q);", 
		(char *)rconfig.name, (char *)rconfig.desc, (char)rconfig.coords.x, (char)rconfig.coords.y, (char)rconfig.coords.z, 
		"0", "0", "0", "0", "0", "0", "0", "0", "0", "0", 
		(char *)rconfig.owner, (char *)rconfig.flags);

	if (sqlite3_exec(get_roomdb(), (char *)querystr, room_callback, 0, (char **)sqlerr) != SQLITE_OK) {
		fprintf(stdout, "SQLITE3 room_insert error!\n%s\n", sqlite3_errmsg(get_roomdb()));
		sqlite3_free(sqlerr);
		return EXIT_FAILURE;
	}
	sqlite3_free(querystr);

	return EXIT_SUCCESS;
}
