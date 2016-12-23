#pragma once

#define SQLITE_ROOMDB_LOC "dbs/sqlite3_rooms.db"
#define SQLITE_PLAYERDB_LOC "dbs/sqlite3_players.db"
#define ROOM_DB_TYPE -1
#define PLAYER_DB_TYPE -2
#define SALTLEN   50
#define HASHSPACE 100
struct {
    int32_t  x;
    int32_t  y;
    int32_t  z;
    uint8_t hash[HASHSPACE];
} player_tmp;
