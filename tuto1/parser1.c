/**
 * @brief		This file contains the parser #1 definitions.
 * @file
 */

#include <tuto1.h>

#define PARSER1_AGAIN	1	///< @brief	This value is returned by the parser1 cothread to request another call.
#define PARSER1_DONE	2	///< @brief	This value is returned by the parser1 cothread to signal the completion.

/**
 * @brief		Reads the stream of the specified parser.
 * @param		[in]	parser	The parser to read the stream of.
 * @param		[in]	buf		The buffer to fill.
 * @param		[in]	sz		The maximum number of bytes to read.
 * @return		Returns the number of bytes read.
 */
static size_t
parser1_read(parser1_t* parser, void* buf, size_t sz)
{
	//---Check arguments---//
	assert( NULL	!= parser);
	assert((NULL	!= buf) || (0 == sz));

	//---Request a reading---//
	parser->wptr	= buf;
	parser->wptr_sz	= sz;
	cothread_yield(&(parser->cothread), PARSER1_AGAIN);

	//---Return---//
	return sz - parser->wptr_sz;
}

/**
 * @brief		The parser entry point.
 * @param		[in]	cothread	The cothread.
 * @param		[in]	user_val	Any user value (except zero) received from the other endpoint.
 * @return		Returns any user value (except zero) to send to the other endpoint.
 */
static int
parser1_main(cothread_t* cothread, int user_val)
{
	//---Check arguments---//
	assert(NULL	!= cothread);

	//---Read the data size---//
	parser1_t*	parser1	= cothread_get_user_data(cothread);
	for (uint8_t data_sz; 0 != parser1_read(parser1, &data_sz, sizeof(data_sz)); ) {
		//---Read the data---//
		char	data[32];
		parser1_read(parser1, data, data_sz);

		//---Log---//
		printf("read: \"%.*s\"\n", (int)data_sz, data);
	}

	//---Return---//
	return PARSER1_DONE;
}

/**
 * @brief		Runs the specified parser.
 * @param		[in]	parser	The parser to run.
 * @return		Returns 0 when the parsing completes.
 */
extern int
parser1_run(parser1_t* parser)
{
	//---Definitions---//
	int		ret	= 1;

	//---Check arguments---//
	assert(NULL	!= parser);

	//---Is a reading request pending ?---//
	if ((NULL != parser->wptr) && (0 != parser->wptr_sz)) {
		//---Read---//
		const size_t	rd	= stream_read(parser->stream, parser->wptr, parser->wptr_sz);
		if (0 == rd) {				// is the end-of-file reached ?
			parser->wptr	= NULL;	// stop the reading request.
		} else {
			parser->wptr	+= rd;
			parser->wptr_sz	-= rd;
		}
	} else {
		//---Run the main---//
		ret	= (PARSER1_DONE == cothread_yield(&(parser->cothread), 1234)) ? 0 : 1;
	}

	//---Return---//
	return ret;
}

/**
 * @brief		Initializes the specified parser.
 * @param		[in]	parser	The parser to initialize.
 * @param		[in]	stream	The stream to read.
 */
extern void
parser1_init(parser1_t* parser, stream_t* stream)
{
	//---Check arguments---//
	assert(NULL	!= parser);
	assert(NULL	!= stream);

	//---Init---//
	parser->stream	= stream;
	parser->wptr	= NULL;
	parser->wptr_sz	= 0;
	//
	cothread_attr_t	attr;
	cothread_attr_init(&attr, parser->stack, sizeof(parser->stack), parser1_main);
	cothread_init(&(parser->cothread), &attr);
	cothread_set_user_data(&(parser->cothread), parser);
}
