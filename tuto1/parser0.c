/**
 * @brief		This file contains the parser #0 definitions.
 * @file
 */

#include <tuto1.h>

//---Forward declarations---//
static void	parser0_stmch_read_sz	(parser0_t* parser);

/**
 * @brief		The state machine "parsing complete" state.
 * @param		[in]	parser	The parser.
 */
static void
parser0_stmch_complete(parser0_t* parser)
{
	// do nothing.
}

/**
 * @brief		The state machine "print data" state.
 * @param		[in]	parser	The parser.
 */
static void
parser0_stmch_print_data(parser0_t* parser)
{
	//---Check arguments---//
	assert(NULL	!= parser);

	//---Log---//
	printf("read: \"%.*s\"\n", (int)(parser->data_sz), parser->data);

	//---Set next state---//
	parser->stmch	= parser0_stmch_read_sz;
}

/**
 * @brief		The state machine "read data" state.
 * @param		[in]	parser	The parser.
 */
static void
parser0_stmch_read_data(parser0_t* parser)
{
	//---Check arguments---//
	assert(NULL	!= parser);

	//---Fill the data buffer---//
	const size_t	rd	= stream_read(parser->stream, &(parser->data[parser->data_off]), parser->data_sz - parser->data_off);
	parser->data_off	+= rd;

	//---Set next state---//
	if (0 == rd) {
		parser->stmch	= parser0_stmch_print_data;
	}
}

/**
 * @brief		The state machine "read data size" state.
 * @param		[in]	parser	The parser.
 */
static void
parser0_stmch_read_sz(parser0_t* parser)
{
	//---Check arguments---//
	assert(NULL	!= parser);

	//---Read the data size---//
	uint8_t				data_sz;
	const size_t		rd	= stream_read(parser->stream, &data_sz, sizeof(data_sz));
	parser->data_sz		= data_sz;
	parser->data_off	= 0;

	//---Set next state---//
	parser->stmch	= (0 == rd) ? parser0_stmch_complete : parser0_stmch_read_data;
}

/**
 * @brief		Runs the specified parser.
 * @param		[in]	parser	The parser to run.
 * @return		Returns 0 when the parsing completes.
 */
extern int
parser0_run(parser0_t* parser)
{
	//---Check arguments---//
	assert(NULL	!= parser);

	//---Run the state machine---//
	parser->stmch(parser);

	//---Return---//
	return parser0_stmch_complete == parser->stmch ? 0 : 1;
}

/**
 * @brief		Initializes the specified parser.
 * @param		[in]	parser	The parser to initialize.
 * @param		[in]	stream	The stream to read.
 */
extern void
parser0_init(parser0_t* parser, stream_t* stream)
{
	//---Check arguments---//
	assert(NULL	!= parser);
	assert(NULL	!= stream);

	//---Init---//
	parser->stmch		= parser0_stmch_read_sz;
	parser->stream		= stream;
	parser->data_sz		= 0;
	parser->data_off	= 0;
}
