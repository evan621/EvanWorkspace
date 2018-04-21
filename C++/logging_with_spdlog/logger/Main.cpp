#include "Logger.hpp"


int main()
{
	LOG_INFO_MSG("Hello world 1\n");

	
	LOG_INFO_MSG("Hello world 2\n");


	LOG_INFO_MSG("Hello world 3\n");

	

	
	LOG_INFO_DBG("Hello world 4\n");

	LOG_ENABLE_DBG();
	LOG_INFO_DBG("Hello world 5\n");
}



