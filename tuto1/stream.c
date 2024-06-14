/**
 * @brief		This file contains the stream definitions.
 * @file
 */

#include <tuto1.h>

/**
 * @brief		Initializes the specified stream.
 * @param		[in]	stream	The stream to initialize.
 * @param		[in]	src		The source buffer to read.
 * @param		[in]	sz		The source buffer size, in bytes.
 */
extern void
stream_init(stream_t* stream, const void* src, size_t sz)
{
	//---Check arguments---//
	assert( NULL	!= stream);
	assert((NULL	!= src) || (0 == sz));

	//---Init---//
	stream->buf	= src;
	stream->sz	= sz;
}

/**
 * @brief		Reads the specified stream.
 * @param		[in]	stream	The stream to read.
 * @param		[in]	buf		The buffer to fill.
 * @param		[in]	sz		The maximum number of bytes to read.
 * @return		Returns the number of bytes read.
 */
extern size_t
stream_read(stream_t* stream, void* buf, size_t sz)
{
	//---Check arguments---//
	assert( NULL	!= stream);
	assert((NULL	!= buf) || (0 == sz));

	//---Compute the number of bytes to read---//
	const size_t	rd	= sz < stream->sz ? sz : stream->sz;
	memcpy(buf, stream->buf, rd);
	stream->buf	= (const uint8_t*)(stream->buf) + rd;
	stream->sz	-= rd;

	//---Return---//
	return rd;
}
