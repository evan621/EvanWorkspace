 #include <sys/eventfd.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>             /* Definition of uint64_t */
#include <poll.h>

#define handle_error(msg) \
   do { perror(msg); exit(EXIT_FAILURE); } while (0)

int
main(int argc, char *argv[])
{
   int efd, j;
   uint64_t u;
   ssize_t s;

   if (argc < 2) {
	   fprintf(stderr, "Usage: %s <num>...\n", argv[0]);
	   exit(EXIT_FAILURE);
   }

   efd = eventfd(0, 0); //EFD_SEMAPHORE  EFD_NONBLOCK
   if (efd == -1)
	   handle_error("eventfd");

   switch (fork()) {
   case 0:
	   for (j = 1; j < argc; j++) {
		   printf("Child writing %s to efd\n", argv[j]);
		   u = strtoull(argv[j], NULL, 0);
				   /* strtoull() allows various bases */
		   s = write(efd, &u, sizeof(uint64_t));
		   if (s != sizeof(uint64_t))
			   handle_error("write");
	   }
	   printf("Child completed write loop\n");

	   exit(EXIT_SUCCESS);

   default:
		sleep(1);

		printf("Parent about to read\n");

		
		while (1)
		{
			struct pollfd item;
			item.fd = efd;
			item.events = POLLIN;
			item.revents = 0;
			
			int ret = poll(&item, 1, -1);
			if (ret == -1)
			{
				printf("error");
				exit(EXIT_SUCCESS);
			}
			else if(item.revents & POLLIN > 0)
			{
				printf("polling something");
				s = read(efd, &u, sizeof(uint64_t));
						
				printf("Parent read %llu (0x%llx) from efd\n",
						(unsigned long long) u, (unsigned long long) u);
			}
			else if(ret == 0)
			{
				printf("timeout\n");
				exit(EXIT_SUCCESS);
			}
		}

			   
		exit(EXIT_SUCCESS);

   case -1:
	   handle_error("fork");
   }
}