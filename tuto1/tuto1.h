/**
 * @brief		This file contains the example declarations.
 * @file
 */

#ifndef __TUTO1_H__
#define __TUTO1_H__

#include <cothread/cothread.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

//---Forward declarations---//
typedef struct _stream_t	stream_t;
typedef struct _parser0_t	parser0_t;
typedef struct _parser1_t	parser1_t;

struct _stream_t
{
	const void*	buf;	///< @brief	Points the next byte to read.
	size_t		sz;		///< @brief	The number of bytes that remains to read.
};

struct _parser0_t
{
	void		(*stmch)	(parser0_t* parser);	///< @brief	The state machine.

	stream_t*	stream;		///< @brief	The stream to read.
	size_t		data_sz;	///< @brief	The data size, in bytes.
	size_t		data_off;	///< @brief	The index of the next data byte to read.
	char		data[32];	///< @brief	The data buffer.
};

struct _parser1_t
{
	stream_t*			stream;		///< @brief	The stream to read.
	uint8_t*			wptr;		///< @brief	The buffer to fill with the data read, NULL if there is no reading request pending.
	size_t				wptr_sz;	///< @brief	The number of bytes to read.

	cothread_stack_t	stack[1 * 1024 * 1024 / sizeof(cothread_stack_t)];	///< @brief	The callee stack.
	cothread_t			cothread;	///< @brief	The cothread.
};

#ifdef __cplusplus
extern "C" {
#endif

extern void			stream_init		(stream_t* stream, const void* src, size_t sz);
extern size_t		stream_read		(stream_t* stream, void* buf, size_t sz);

extern int			parser0_run		(parser0_t* parser);
extern void			parser0_init	(parser0_t* parser, stream_t* stream);

extern int			parser1_run		(parser1_t* parser);
extern void			parser1_init	(parser1_t* parser, stream_t* stream);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __TUTO1_H__ */
