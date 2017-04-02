#pragma once

#include "../common.h"

struct Command *is_movement_cmd(const uint8_t *cmd, struct Command *info);
struct Command *is_system_cmd(const uint8_t *cmd, struct Command *info);
struct Command *is_room_cmd(const uint8_t *cmd, struct Command *info);
struct Command *is_info_cmd(const uint8_t *cmd, struct Command *info);
struct Command *is_travel_cmd(const uint8_t *cmd, struct Command *info);

struct Command *get_command_info(const uint8_t *cmd);
size_t get_num_of_available_cmds(void);
uint8_t *get_command(const size_t num);
uint8_t *get_movement_str(const int32_t dir);

enum MOVEMENT {
	MOVEMENT_NORTH,
	MOVEMENT_N,
	MOVEMENT_EAST,
	MOVEMENT_E,
	MOVEMENT_SOUTH,
	MOVEMENT_S,
	MOVEMENT_WEST,
	MOVEMENT_W,
	MOVEMENT_UP,
	MOVEMENT_U,
	MOVEMENT_DOWN,
	MOVEMENT_D,
	MOVEMENT_NORTHEAST,
	MOVEMENT_NE,
	MOVEMENT_SOUTHEAST,
	MOVEMENT_SE,
	MOVEMENT_SOUTHWEST,
	MOVEMENT_SW,
	MOVEMENT_NORTHWEST,
	MOVEMENT_NW,
	MOVEMENT_NOT
};

enum COMMAND_TYPES {
	MOVEMENT,
	ROOM_CHANGE,
	SYSTEM_ACTION,
	TRAVEL_ACTION,
	INFO_REQUEST,
	COMMAND_NOT = -1
};

enum SYSTEM_ACTION {
	SYS_SAY,
	SYS_QUIT,
	SYS_NOT
};

enum ROOM_CHANGE {
	ROOM_MK,
	ROOM_RM,
	ROOM_SET_NAME,
	ROOM_SET_DESC,
	ROOM_SET_FLAG,
	ROOM_SET_EXIT,
	ROOM_SET_NOT
};

enum INFO_REQUEST {
	INFO_ROOM,
	INFO_ROOM2,
	INFO_COMMANDS,
	INFO_PLAYERS,
	INFO_MAP,
	INFO_NOT
};

enum TRAVEL_ACTION {
	TRAVEL_GOTO,
	TRAVEL_SWAP,
	TRAVEL_NOT
};

struct Command {
	enum COMMAND_TYPES type;
	int32_t subtype;
};

