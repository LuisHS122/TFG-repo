#include "lc3_decoder.h"
#include "parser.h"

int main(int argc, char **argv)
{
	static lc3_decoder_t decoder;
	lc3_decoder_mem_16k_t decoder_mem;
	decoder = lc3_setup_decoder(10000,16000,0,malloc(lc3_decoder_size(10000,16000)));
	if(decoder == NULL){
    printf("NULL DECODER \n");
    return -1;
}

	if (parse(argv[1]) != 0)
	{
		printf("Error parsing file. \n");
		return -1;
	}
	
		
	
	if (decode(decoder,argv[1]) != 0)
	{
		printf("Decoder error \n");
		return -1;
	}

	return 0;
}