#include "PlayerDBRecord.h"

struct PlayerDBRecord *get_player_db_struct(void)
{
	return (struct PlayerDBRecord *)calloc(sizeof(struct PlayerDBRecord), sizeof(struct PlayerDBRecord));
}
