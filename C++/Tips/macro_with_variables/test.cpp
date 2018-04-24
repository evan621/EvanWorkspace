#include <stdio.h>

#define LOG_1(format, ...) fprintf(stdout, format, ##__VA_ARGS__)
#define LOG_2(format, args...) fprintf(stdout, format, args)

#define ERR_LOG(module) fprintf(stderr, "error, "#module"\n")

int main()
{
	LOG_1("Hello, %d\n", 1);
	
	LOG_2("Hello, %d\n", 2);
	
	LOG_1("Hello world\n");
		
	ERR_LOG("add");
	ERR_LOG(dev=0);

	
	return 0;
}	