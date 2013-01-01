#include <stdio.h>

#include "Bot.h"

#define PROGRAM "vox"
#define VERSION 0.1


int main(int argc, const char *argv[])
{
	printf("%s - Version %.1f\n", PROGRAM, VERSION);
	
	if (argc < 6) { // Update to 6 when accepting all arguments.
		printf("USAGE: %s [server] [account] [password] [classic_key] [expansion_key]\n", argv[0]);
		return 0;
	}
	
	/*for (int x=0; x<argc; x++) {
		printf("[%u] : %s\n", x, argv[x]);
	}*/
	
	Bot Apoc(argv[1], argv[2], argv[3], argv[4], argv[5]);
	//Apoc.printStatus();
	Apoc.run();
	
	return 0;
}
