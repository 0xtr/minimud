#pragma once

sqlite3 *get_roomdb (void);
void *set_roomdb (sqlite3 *db);
sqlite3 *roomdb;

sqlite3 *get_playerdb (void);
void *set_playerdb (sqlite3 *db);
sqlite3 *playerdb;
