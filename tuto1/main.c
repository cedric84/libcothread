/**
 * @brief		This file contains an example of application that uses the cothread library.
 * @file
 */

#include <tuto1.h>

/**
 * @brief		The application entry point.
 * @param		[in]	argc	The number of arguments.
 * @param		[in]	argv	The arguments values.
 * @return		Returns zero on success.
 */
extern int
main(int argc, char* argv[])
{
	//---Definitions---//
	int	ret	= EXIT_FAILURE;

	//---Log---//
	printf("%s started\n", __func__);

	//---Create a stream to parser---//
	static const char	src[]	= "\x05" "Hello" "\x07" "world !";
	stream_t			stream;
	stream_init(&stream, src, sizeof(src) - 1);

	//---Check the number of arguments---//
	if (2 != argc) {
		fprintf(stderr, "invalid number of arguments\n");
	} else if (0 == strcmp(argv[1], "0")) {
		//---Create a parser---//
		static parser0_t	parser0;
		parser0_init(&parser0, &stream);

		//---Run the parser---//
		while (0 != parser0_run(&parser0)) {
			printf("do background stuff...\n");
		}

		//---Update the return code---//
		ret	= EXIT_SUCCESS;
	} else if (0 == strcmp(argv[1], "1")) {
		//---Create a parser---//
		static parser1_t	parser1;
		parser1_init(&parser1, &stream);

		//---Run the parser---//
		while (0 != parser1_run(&parser1)) {
			printf("do background stuff...\n");
		}

		//---Update the return code---//
		ret	= EXIT_SUCCESS;
	} else {
		fprintf(stderr, "parser #%s is not supported\n", argv[1]);
	}

	//---Return---//
	printf("%s terminated\n", __func__);
	return ret;
}
