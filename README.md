# libcothread

The cothread library provides a way to create execution contexts (CPU registers & stack)
and make it possible for a single thread to pause any of them while resuming another one.
This library aims to be as simpler as possible,
uses the ISO C [setjmp & longjmp](https://www.man7.org/linux/man-pages/man3/setjmp.3.html) functions
to save & restore the CPU registers and some assembly code to initialize the new execution context.

## How to use the library
The [cothread.h](include/cothread/cothread.h) main header defines the `cothread_t` structure
which contains all the data needed to switch from an execution context to another one:
1. To initialize such a structure, the user shall first configure some `cothread_attr_t` attributes
with the `cothread_attr_init` function, and use these attributes in a call to the `cothread_init` one ;
2. Once the `cothread_t` is initialized, the user may call the `cothread_set_user_data` function to store
any data he/she wants, and the `cothread_get_user_data` one to retrieve it
(the library never modifies this user data) ;
3. From this point, the current execution context may be paused and the created one resumed using
the `cothread_yield` function (note that this function may be called many times to get back
to the previous execution context.)

## Examples
The [tuto0](tuto0) is a very simple program that demonstrates how to initialize and use the library.

The [tuto1](tuto1) one is more complex:
Imagine a stream that contains data packets.
Every data packet is composed of an octet that contains the number of data bytes that immediately follows.
This example shows how to parse such a stream using whether the [cothread library](tuto1/parser1.c)
or a standard [state machine](tuto1/parser0.c).
