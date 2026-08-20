#pragma once

#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
typedef BOOL pipe_bool_t;
typedef DWORD pipe_dword_t;
typedef HANDLE pipe_handle_t;
#else
#include <stdint.h>
typedef int pipe_bool_t;
typedef uint32_t pipe_dword_t;
typedef void *pipe_handle_t;
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#endif

#define PIPE_BUFFER_SIZE 1024

#ifdef __cplusplus
extern "C" {
#endif
	extern char pipe_available;
	extern pipe_handle_t pipe_handle;
	extern char pipe_buffer[PIPE_BUFFER_SIZE];
	extern pipe_dword_t pipe_bytes_read;
	extern pipe_dword_t pipe_bytes_written;

	pipe_bool_t  Pipe_Create(void);
	pipe_dword_t Pipe_AvailableBytes(void);
	pipe_bool_t  Pipe_ConnectToNew(void);
	pipe_bool_t  Pipe_ConnectToExisting(void);
	pipe_bool_t  Pipe_Write(const char* format, ...);
	pipe_bool_t  Pipe_Read(void);
	void Pipe_Close(void);
	pipe_bool_t Pipe_IsConnected(void);
	void Pipe_BeginConnect(void);
#ifdef __cplusplus
}
#endif