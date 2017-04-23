#include "../mapping/insert_room.h"

struct room_atom *insert_room(struct room_blueprint rconfig)
{
	uint8_t param_x[2 * sizeof(rconfig.coords.x)] = {0};
	uint8_t param_y[2 * sizeof(rconfig.coords.y)] = {0};
	uint8_t param_z[2 * sizeof(rconfig.coords.z)] = {0};
	snprintf((char *)param_x, sizeof(rconfig.coords.x), "%d", rconfig.coords.x);
	snprintf((char *)param_y, sizeof(rconfig.coords.y), "%d", rconfig.coords.y);
	snprintf((char *)param_z, sizeof(rconfig.coords.z), "%d", rconfig.coords.z);

	run_sql(sqlite3_mprintf(
		"INSERT OR IGNORE INTO ROOMS (name, desc, x, y, z, north, east, south, west, up, down, "
		"northeast, southeast, southwest, northwest, owner, last_modified_by, flags)"
		"VALUES (%Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q);", 
		(char *)rconfig.name, (char *)rconfig.desc, 
		param_x, param_y, param_z, 
		"-1", "-1", "-1", "-1", "-1", "-1", "-1", "-1", "-1", "-1", 
		(char *)rconfig.owner, (char *)rconfig.owner, (char *)rconfig.flags), 0, DB_ROOM);

	struct room_atom *room = lookup_room(rconfig.coords);

	return room;
}
