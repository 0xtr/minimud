#include "room_adjustments.h"

static int32_t adjusting_room_exit(const int32_t pnum, const _Bool reverse, const int32_t x, const int32_t y, const int32_t z);
static int32_t get_new_room_id(void);

int32_t insert_room(struct NewRoom rconfig)
{
	uint8_t *sqlerr = NULL;
	const int32_t new_room_id = get_new_room_id();

	struct Map *map = lookup_room(rconfig.x, rconfig.y, rconfig.z, -1);

	if (map != NULL) {
		free(map);
		return EXIT_SUCCESS;
	}

	uint8_t *querystr = (uint8_t *)sqlite3_mprintf(
		"INSERT INTO ROOMS VALUES (%Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q, %Q);", 
		(char)new_room_id, (char *)rconfig.name, (char *)rconfig.desc, (char)rconfig.x, (char)rconfig.y, (char)rconfig.z, 
		"0", "0", "0", "0", "0", "0", "0", "0", "0", "0", 
		(char *)rconfig.owner, (char *)rconfig.flags);
	if (sqlite3_exec(get_roomdb(), (char *)querystr, callback, 0, (char **)sqlerr) != SQLITE_OK) {
		fprintf(stdout, "SQLITE3 room_insert error!\n%s\n", sqlite3_errmsg(get_roomdb()));
		sqlite3_free(sqlerr);
		return EXIT_FAILURE;
	}
	sqlite3_free(querystr);

	return EXIT_SUCCESS;
}

static int32_t get_new_room_id(void)
{
	struct Map *map = get_room();
	const int32_t new_id = map->id + 1;

	free(map);
	return new_id;
}

int32_t remove_room(const int32_t pnum)
{
	uint8_t *sqlerr = NULL;
	int32_t x = get_player_coord(X_COORD_REQUEST, pnum);
	int32_t y = get_player_coord(Y_COORD_REQUEST, pnum);
	int32_t z = get_player_coord(Z_COORD_REQUEST, pnum);
	struct Map *map = lookup_room(x, y, z, pnum);

	if (map == NULL)
		return -1;

	if (strcmp((char *)map->owner, (char *)get_player_pname(pnum)) != 0) {
		free_room(map);
		return -2;
	}

	uint8_t *querystr = (uint8_t *)sqlite3_mprintf("DELETE FROM ROOMS WHERE xloc LIKE %Q AND yloc LIKE %Q AND zloc LIKE %Q;", (char)x, (char)y, (char)z);
	if (sqlite3_exec(get_roomdb(), (char *)querystr, callback, 0, (char **)sqlerr) != SQLITE_OK) {
		fprintf(stdout, "SQLITE3 failure in remove_room; could not delete the room:\n%s\n", sqlite3_errmsg(get_roomdb()));
		sqlite3_free(querystr);
		sqlite3_free(sqlerr);
		free_room(map);
		return -3;
	}
	sqlite3_free(querystr);

	if (map->north == true)
		adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -2, x, y + 1, z);
	if (map->south == true)
		adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -1, x, y - 1, z);
	if (map->east == true)
		adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -4, x + 1, y, z);
	if (map->west == true)
		adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -3, x + 1, y, z);

	if (map->up == true)
		adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -6, x, y, z + 1);
	if (map->down == true)
		adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -5, x, y - 1, z);

	if (map->northeast == true)
		adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -9, x + 1, y + 1, z);
	if (map->southeast == true)
		adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -10, x + 1, y - 1, z);
	if (map->southwest == true)
		adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -7, x - 1, y - 1, z);
	if (map->northwest == true)
		adjust_room_details(ADJUSTING_ROOM_EXIT, 0, -8, x - 1, y + 1, z);
	//status = check_exits_and_adjust(ADJUSTING_ROOM_EXIT, x, y, z);

	assert(remove_players_from_room(x, y, z) == EXIT_SUCCESS);
	free_room(map);
	return EXIT_SUCCESS;
}

int32_t remove_players_from_room(const int32_t x, const int32_t y, const int32_t z)
{
	// get_players_in_room(x, y, z);
	for (size_t i = 0; i < get_num_of_players(); ++i) {
		if (get_player_in_use(i) == 1) {
			if (get_player_coord(X_COORD_REQUEST, i) == x &&
				get_player_coord(Y_COORD_REQUEST, i) == y &&
				get_player_coord(Z_COORD_REQUEST, i) == z) {
				print_to_player(i, PRINT_REMOVED_FROM_ROOM);
				adjust_player_location(i, -1, -1, -1);
				print_to_player(i, SHOWROOM);
			}
		}
	}
	return EXIT_SUCCESS;
}

int32_t adjust_room_details(const int32_t adjusting, const _Bool reverse, const int32_t pnum, const int32_t x, const int32_t y, const int32_t z)
{
	uint8_t *sqlerr = NULL;
	uint8_t *room   = NULL;

	if (compare_room_owner(pnum, x, y, z) == -1)
		return -3;

	if (adjusting == ADJUSTING_ROOM_DESC) {
		room = (uint8_t *)sqlite3_mprintf("UPDATE ROOMS SET rdesc = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
			get_player_store(pnum), get_player_pname(pnum), (char)x, (char)y, (char)z);
	} else if (adjusting == ADJUSTING_ROOM_NAME) {
		room = (uint8_t *)sqlite3_mprintf("UPDATE ROOMS SET rname = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
			get_player_store(pnum), get_player_pname(pnum), (char)x, (char)y, (char)z);
	} else if (adjusting == ADJUSTING_ROOM_EXIT) {
		// room = adjusting_room_exit(pnum);
	} else if (adjusting == ADJUSTING_ROOM_FLAG) {
		// got to get current, then add to rflags
		// room = adjusting_room_flag(pnum);
		room = (uint8_t *)sqlite3_mprintf("UPDATE ROOMS SET rflags = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
			get_player_pname(pnum), (char)x, (char)y, (char)z);
	}

	lookup_room_exits(x, y, z, -1);

	if (sqlite3_exec(get_roomdb(), (char *)room, callback, 0, (char **)sqlerr) != SQLITE_OK) {
		fprintf(stdout, "SQLITE3 room update error:\n%s\n", sqlite3_errmsg(get_roomdb()));
		sqlite3_free(room);
		sqlite3_free(sqlerr);
		return EXIT_FAILURE;
	}

	sqlite3_free(room);
	return EXIT_SUCCESS;
}

int32_t get_opposite_dir(const uint8_t *dir)
{
	const uint8_t VALID_DIRECTIONS[10][10] = {
		"north", "south", "west", "east", "northeast", "southwest", "northwest",
		"southeast", "up", "down"
	};

	size_t i;
	_Bool found = 0;
	for (i = 0; i < 10; ++i) {
		if (strcmp((char *)VALID_DIRECTIONS[i], (char *)dir) == 0) {
			found = true;
			break;
		}
	}
	if (found == false)
		return EXIT_FAILURE;

	if (strcmp((char *)dir, "south") == 0	 ||
		strcmp((char *)dir, "east") == 0	  ||
		strcmp((char *)dir, "southwest") == 0 ||
		strcmp((char *)dir, "southeast") == 0 ||
		strcmp((char *)dir, "down") == 0) {
		return i - 1;
	} else {
		return i + 1;
	}
	return EXIT_FAILURE;
}

int32_t get_direction_as_number(const uint8_t *dir)
{
	if (strcmp((char *)dir, "north") || strcmp((char *)dir,"North"))
		return NORTH_DIR;
	if (strcmp((char *)dir, "south") || strcmp((char *)dir,"South"))
		return SOUTH_DIR;
	if (strcmp((char *)dir, "east") || strcmp((char *)dir,"East"))
		return EAST_DIR;
	if (strcmp((char *)dir, "west") || strcmp((char *)dir,"West"))
		return WEST_DIR;
	if (strcmp((char *)dir, "up") || strcmp((char *)dir,"Up"))
		return UP_DIR;
	if (strcmp((char *)dir, "down") || strcmp((char *)dir,"Down"))
		return DOWN_DIR;
	if (strcmp((char *)dir, "northeast") || strcmp((char *)dir,"Northeast"))
		return NORTHEAST_DIR;
	if (strcmp((char *)dir, "southeast") || strcmp((char *)dir,"Southeast"))
		return SOUTHEAST_DIR;
	if (strcmp((char *)dir, "southwest") || strcmp((char *)dir,"Southwest"))
		return SOUTHWEST_DIR;
	if (strcmp((char *)dir, "northwest") || strcmp((char *)dir,"Northwest"))
		return NORTHWEST_DIR;
	if (strcmp((char *)dir, "return") || strcmp((char *)dir, "Return"))
		return RETURN_ORIGIN_DIR;
	return EXIT_FAILURE;
}

uint8_t *get_dir_string(const int32_t dir)
{
	if (dir == RETURN_ORIGIN_DIR)
		return (uint8_t *)"back to the core.";
	if (dir == NORTH_DIR)
		return (uint8_t *)"north";
	if (dir == SOUTH_DIR)
		return (uint8_t *)"south";
	if (dir == EAST_DIR)
		return (uint8_t *)"east";
	if (dir == WEST_DIR)
		return (uint8_t *)"west";
	if (dir == UP_DIR)
		return (uint8_t *)"up";
	if (dir == DOWN_DIR)
		return (uint8_t *)"down";
	if (dir == NORTHEAST_DIR)
		return (uint8_t *)"northeast";
	if (dir == SOUTHEAST_DIR)
		return (uint8_t *)"southeast";
	if (dir == SOUTHWEST_DIR)
		return (uint8_t *)"southwest";
	if (dir == NORTHWEST_DIR)
		return (uint8_t *)"northwest";
	return (uint8_t *)"EMPTY";
}

static int32_t adjusting_room_exit(const int32_t pnum, const _Bool reverse, const int32_t x, const int32_t y, const int32_t z)
{
	printf("%d %d %d %d\n", pnum, x, y, z);
	if (reverse) 
		printf(".\n");
	// TODO: use newroom 
	/*
	if (strlen((char *)get_player_store(pnum)) == 0 || get_player_store(pnum) == NULL)
		return -1;

	const int32_t direction = get_direction_as_number(get_player_store(pnum));

	struct Map *map = lookup_room(x, y, z, pnum);

	uint8_t *room = sqlite3_mprintf("UPDATE ROOMS SET %Q = %Q, last_modified_by = %Q WHERE xloc = %Q AND yloc = %Q AND zloc = %Q;", 
			get_dir_string(direction), reverse_of_current(direction, reverse), get_player_pname(pnum), (char)x, (char)y, (char)z);
			*/

	//free_room(map);
	// stuff
	return EXIT_SUCCESS;
}
