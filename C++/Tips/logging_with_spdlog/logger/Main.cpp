#include "Logger.hpp"
#include <unistd.h>


int main()
{
	auto count = 100;
	
	while(count--)
	{
		LOG_INFO_MSG("Info loggs 1, %d\n", count);

		LOG_ERROR_MSG("Error loggs, %d\n", count);

		usleep(100000);
	}

}

