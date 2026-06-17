#include "sbc_decoder.h"
#include "parser.h"

int main(int argc, char **argv)
{
	

	if (parse(argv[1]) != 0)
	{
		printf("Error parsing file. \n");
		return -1;
	}
	
		
	
	if (decode(argv[1]) != 0)
	{
		printf("Decoder error \n");
		return -1;
	}

	return 0;
}