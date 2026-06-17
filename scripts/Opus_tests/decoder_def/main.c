#include "decoder.h"
#include "parser.h"

int main(int argc, char **argv)
{

	if (parse(argv[1]) != 0)
	{
		printf("Error parsing file. \n");
		return -1;
	}
	if (argc >= 3)
	{
		if (strcmp(argv[2], "custom") == 0)
		{
			// #define CUSTOM_MODES

			if (decode_opus_custom_stream_unknown_size(argv[1]) != 0)
			{
				printf("Decoder error (custom) \n");
				return -1;
			}
			printf("awaaa");
			return 0;
		}
		if (strcmp(argv[2], "custom2") == 0)
		{
			// #define CUSTOM_MODES

			if (decode_opus_custom2_stream_unknown_size(argv[1]) != 0)
			{
				printf("Decoder error (custom) \n");
				return -1;
			}
			printf("awaaa");
			return 0;
		}
		
	}
	if (decode_opus_stream_unknown_size(argv[1]) != 0)
	{
		printf("Decoder error \n");
		return -1;
	}

	return 0;
}