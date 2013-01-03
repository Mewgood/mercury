#include <stdio.h>

#include "Bot.h"

#define PROGRAM "particle"
#define VERSION 0.1


int main(int argc, const char *argv[])
{
	printf("%s - Version %.1f\n", PROGRAM, VERSION);
	
	if (argc < 6) { // Update to 6 when accepting all arguments.
		printf("USAGE: %s [server] [realm] [account] [password] [character] [classic_key] [expansion_key] [exe_info] [key_owner]\n", argv[0]);
		return 0;
	}
	
	Bot Apoc(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], argv[8], argv[9]);
	//Apoc.printStatus();
	Apoc.run();
	
	return 0;
}
