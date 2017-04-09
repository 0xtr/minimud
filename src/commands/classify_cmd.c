#include "classify_cmd.h"

#define arr_size(x) sizeof(x) / sizeof(x[0])
#define loop_over_cmds(x, y) \
	for (size_t i = 0; i < (sizeof(x) / sizeof(x[0])); ++i) {\
		if (!(strlen(x[i]) == strlen((char *)cmd)))\
			continue;\
		if (memcmp(x[i], cmd, strlen(x[i])) == 0) {\
			info->type = y;\
			info->subtype = i;\
			break;\
		}\
	}
#define return_if_matched \
	if (info->type != COMMAND_NOT)\
		return info;

const char * const movement_commands[] = {
	"north", "n",
	"east", "e",
	"south","s",
	"west","w",
	"up","u",
	"down","d",
	"northeast", "ne", 
	"southeast", "se", 
	"southwest", "sw",
	"northwest","nw"
};
const char * const system_commands[] = {
	"say",
	"quit"
};
const char * const info_commands[] = {
	"look", "l",
	"commands",
	"players",
	"map"
};
const char * const room_commands[] = {
	"mkroom",
	"rmroom",
	"setrname",
	"setrdesc",
	"setrexit",
	"setrflag"
};

const char * const travel_commands[] = {
	"goto",
	"swap"
};

struct Command *is_movement_cmd(const uint8_t *cmd, struct Command *info)
{
	loop_over_cmds(movement_commands, MOVEMENT);
	return info;
}

struct Command *is_system_cmd(const uint8_t *cmd, struct Command *info)
{
	loop_over_cmds(system_commands, SYSTEM_ACTION);
	return info;
}

struct Command *is_room_cmd(const uint8_t *cmd, struct Command *info)
{
	loop_over_cmds(room_commands, ROOM_CHANGE);
	return info;
}

struct Command *is_travel_cmd(const uint8_t *cmd, struct Command *info)
{
	loop_over_cmds(travel_commands, TRAVEL_ACTION);
	return info;
}

struct Command *is_info_cmd(const uint8_t *cmd, struct Command *info)
{
	loop_over_cmds(info_commands, INFO_REQUEST);
	return info;
}

struct Command *get_command_info(const uint8_t *cmd)
{
	struct Command *info = (struct Command *)malloc(sizeof(struct Command));
	info->type = COMMAND_NOT;
	info->subtype = COMMAND_NOT;

	info = is_system_cmd(cmd, info);
	return_if_matched;
	info = is_movement_cmd(cmd, info);
	return_if_matched;
	info = is_room_cmd(cmd, info);
	return_if_matched;
	info = is_info_cmd(cmd, info);
	return_if_matched;
	info = is_travel_cmd(cmd, info);

	return info;
}

uint8_t *get_movement_str(const int32_t dir)
{
	if (dir == MOVEMENT_NORTH)
		return (uint8_t *)"north";
	if (dir == MOVEMENT_SOUTH)
		return (uint8_t *)"south";
	if (dir == MOVEMENT_EAST)
		return (uint8_t *)"east";
	if (dir == MOVEMENT_WEST)
		return (uint8_t *)"west";
	if (dir == MOVEMENT_UP)
		return (uint8_t *)"up";
	if (dir == MOVEMENT_DOWN)
		return (uint8_t *)"down";
	if (dir == MOVEMENT_NORTHEAST)
		return (uint8_t *)"northeast";
	if (dir == MOVEMENT_SOUTHEAST)
		return (uint8_t *)"southeast";
	if (dir == MOVEMENT_SOUTHWEST)
		return (uint8_t *)"southwest";
	if (dir == MOVEMENT_NORTHWEST)
		return (uint8_t *)"northwest";
	return (uint8_t *)"EMPTY";
}

size_t get_num_of_available_cmds(void)
{
	return arr_size(movement_commands) 
		+ arr_size(system_commands)
		+ arr_size(info_commands)
		+ arr_size(travel_commands)
		+ arr_size(room_commands);
}

uint8_t *get_command(const size_t num)
{
	const size_t move_index = arr_size(movement_commands) - 1;
	const size_t syst_index = move_index + arr_size(system_commands);
	const size_t info_index = syst_index + arr_size(info_commands);
	const size_t room_index = info_index + arr_size(room_commands);
	const size_t trav_index = room_index + arr_size(travel_commands);

	for (size_t i = 0; i != get_num_of_available_cmds(); ++i) {
		if (i != num)
			continue;
		if (i <= move_index)
			return (uint8_t *)movement_commands[i];
		if (i <= syst_index)
			return (uint8_t *)system_commands[i-move_index-1];
		if (i <= info_index)
			return (uint8_t *)info_commands[i-syst_index-1];
		if (i <= room_index)
			return (uint8_t *)room_commands[i-info_index-1];
		if (i <= trav_index)
			return (uint8_t *)travel_commands[i-room_index-1];
	}

	return NULL;
}
