#include "epollfd_storage.h"

static int32_t epollfd;
int32_t get_epollfd(void)
{
	return epollfd;
}

void set_epollfd(const int32_t fd)
{
	epollfd = fd;
}
