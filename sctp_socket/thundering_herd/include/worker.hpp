#ifndef _WORKER
#define _WORKER

#include <stdio.h>
#include "IoMultiplex.hpp"


class worker
{
public:
	worker();
	~worker();
	
	void process();
private:
	IoMultiplex io_multi;
};

#endif