#pragma once

#include "../common.h"

#include "../sqlite/sqlite_callbacks.h"
#include "../sqlite/sqlite_init_dbs.h"
#include "../sqlite/run_sql.h"

#include "coordinates.h"
#include "room_db_record.h"
#include "room_blueprint.h"

#include "../players/player_movement.h"

int32_t adjust_room_flag(struct player_live_record *player);
int32_t adjust_room_desc(struct player_live_record *player);
int32_t adjust_room_name(struct player_live_record *player);

int32_t link_rooms(const int32_t dir, struct room_db_record *existing, struct room_db_record *newroom);
int32_t unlink_rooms(const int32_t dir, struct room_db_record *existing, struct room_db_record *newroom);

struct room_db_record *insert_room(struct room_blueprint rconfig);
int32_t remove_room(struct player_live_record *player);
int32_t remove_players_from_room(const struct coordinates coords, struct room_db_record *room);

struct room_db_record *lookup_room(const struct coordinates coords);
struct room_db_record *lookup_room_by_id(const int32_t id);
int32_t lookup_room_exits(const struct coordinates origin, struct room_db_record *dest_room);
int32_t lookup_room_name_from_coords(struct player_live_record *player, const struct coordinates coords);
int32_t compare_room_owner(struct player_live_record *player, const struct coordinates coords);
int32_t has_exit_for_dir(struct room_db_record *origin, struct room_db_record *dest);
