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

struct command *is_movement_cmd(const uint8_t *cmd, struct command *info)
{
	loop_over_cmds(movement_commands, MOVEMENT);
	return info;
}

struct command *is_system_cmd(const uint8_t *cmd, struct command *info)
{
	loop_over_cmds(system_commands, SYSTEM_ACTION);
	return info;
}

struct command *is_room_cmd(const uint8_t *cmd, struct command *info)
{
	loop_over_cmds(room_commands, ROOM_CHANGE);
	return info;
}

struct command *is_travel_cmd(const uint8_t *cmd, struct command *info)
{
	loop_over_cmds(travel_commands, TRAVEL_ACTION);
	return info;
}

struct command *is_info_cmd(const uint8_t *cmd, struct command *info)
{
	loop_over_cmds(info_commands, INFO_REQUEST);
	return info;
}

struct command *get_command_info(const uint8_t *cmd)
{
	struct command *info = (struct command *)malloc(sizeof(struct command));

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
	if (is_dir(dir, DIR_NORTH))
		return (uint8_t *)"north";
	if (is_dir(dir, DIR_SOUTH))
		return (uint8_t *)"south";
	if (is_dir(dir, DIR_EAST))
		return (uint8_t *)"east";
	if (is_dir(dir, DIR_WEST))
		return (uint8_t *)"west";
	if (is_dir(dir, DIR_UP))
		return (uint8_t *)"up";
	if (is_dir(dir, DIR_DOWN))
		return (uint8_t *)"down";
	if (is_dir(dir, DIR_NORTHEAST))
		return (uint8_t *)"northeast";
	if (is_dir(dir, DIR_SOUTHEAST))
		return (uint8_t *)"southeast";
	if (is_dir(dir, DIR_SOUTHWEST))
		return (uint8_t *)"southwest";
	if (is_dir(dir, DIR_NORTHWEST))
		return (uint8_t *)"northwest";
	return (uint8_t *)"EMPTY";
}

_Bool is_dir(const int32_t dir, const int32_t check)
{
	if (dir == check || dir == (check + 1))
		return true;
	return false;
}

uint8_t *get_opposite_str(const int32_t dir)
{
	if (is_dir(dir, DIR_NORTH))
		return (uint8_t *)"south";
	if (is_dir(dir, DIR_SOUTH))
		return (uint8_t *)"north";
	if (is_dir(dir, DIR_EAST))
		return (uint8_t *)"west";
	if (is_dir(dir, DIR_WEST))
		return (uint8_t *)"east";
	if (is_dir(dir, DIR_UP))
		return (uint8_t *)"down";
	if (is_dir(dir, DIR_DOWN))
		return (uint8_t *)"up";
	if (is_dir(dir, DIR_NORTHEAST))
		return (uint8_t *)"southwest";
	if (is_dir(dir, DIR_SOUTHEAST))
		return (uint8_t *)"northwest";
	if (is_dir(dir, DIR_SOUTHWEST))
		return (uint8_t *)"northeast";
	if (is_dir(dir, DIR_NORTHWEST))
		return (uint8_t *)"southeast";
	return NULL;
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

struct command *get_empty_command(void)
{
	struct command *info = (struct command *)malloc(sizeof(struct command));

	info->type = COMMAND_NOT;
	info->subtype = COMMAND_NOT;

	return info;
}
