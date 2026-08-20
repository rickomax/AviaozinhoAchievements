#include "Pipe.h"

char pipe_available;

#ifdef _WIN32

HANDLE pipe_handle = INVALID_HANDLE_VALUE;
char pipe_buffer[PIPE_BUFFER_SIZE];
DWORD pipe_bytes_read;
DWORD pipe_bytes_written;

OVERLAPPED g_ov;
HANDLE g_event = NULL;

BOOL Pipe_Create(void) {
    pipe_handle = CreateNamedPipeA(
        PIPE_NAME,
        PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        PIPE_BUFFER_SIZE,
        PIPE_BUFFER_SIZE,
        0,
        NULL
    );
    if (pipe_handle == INVALID_HANDLE_VALUE) return FALSE;
    g_event = CreateEvent(NULL, TRUE, FALSE, NULL);
    ZeroMemory(&g_ov, sizeof(g_ov));
    g_ov.hEvent = g_event;
    return TRUE;
}

DWORD Pipe_AvailableBytes(void)
{
    if (pipe_handle == INVALID_HANDLE_VALUE) {
        return 0;
    }
    DWORD bytesAvailable = 0;
    if (!PeekNamedPipe(pipe_handle, NULL, 0, NULL, &bytesAvailable, NULL)) {
        return 0;
    }
    return bytesAvailable;
}

BOOL Pipe_ConnectToNew(void) {
    return ConnectNamedPipe(pipe_handle, NULL);
}

BOOL Pipe_ConnectToExisting(void)
{
    for (;;)
    {
        pipe_handle = CreateFileA(
            PIPE_NAME,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            0,
            NULL
        );
        if (pipe_handle != INVALID_HANDLE_VALUE)
        {
            DWORD mode = PIPE_READMODE_MESSAGE | PIPE_WAIT;
            SetNamedPipeHandleState(pipe_handle, &mode, NULL, NULL);
            return TRUE;
        }
        DWORD err = GetLastError();
        if (err == ERROR_PIPE_BUSY)
        {
            if (!WaitNamedPipeA(PIPE_NAME, 5000)) 
            {
                return FALSE;
            }
        }
        else if (err == ERROR_FILE_NOT_FOUND)
        {
            return FALSE;
        }
    }
}
BOOL Pipe_Write(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int newByteCount = vsnprintf(pipe_buffer, PIPE_BUFFER_SIZE, format, args);
    va_end(args);
    return WriteFile(pipe_handle, pipe_buffer, newByteCount, &pipe_bytes_written, NULL);
}

BOOL Pipe_Read(void) {
    char result = ReadFile(pipe_handle, pipe_buffer, PIPE_BUFFER_SIZE - 1, &pipe_bytes_read, NULL);
    pipe_buffer[pipe_bytes_read] = '\0';
    return result;
}

void Pipe_Close(void) {
    if (pipe_handle != INVALID_HANDLE_VALUE) {
        FlushFileBuffers(pipe_handle);
        CloseHandle(pipe_handle);
        pipe_handle = INVALID_HANDLE_VALUE;
    }
    if (g_event != NULL) {
        CloseHandle(g_event);
        g_event = NULL;
        g_ov.hEvent = NULL;
    }
}

void Pipe_BeginConnect(void) {
    ResetEvent(g_event);
    BOOL ok = ConnectNamedPipe(pipe_handle, &g_ov);
    if (!ok) {
        DWORD err = GetLastError();
        if (err == ERROR_PIPE_CONNECTED) {
            SetEvent(g_event);
        }
    }
}

BOOL Pipe_IsConnected(void) {
    if (WaitForSingleObject(g_event, 0) == WAIT_OBJECT_0) {
        return TRUE;
    }
    return FALSE;
}

#else /* !_WIN32 */

/* No launcher IPC on this mac or steam yet; achievements/stats calls are no ops. */

pipe_handle_t pipe_handle = NULL;
char pipe_buffer[PIPE_BUFFER_SIZE];
pipe_dword_t pipe_bytes_read;
pipe_dword_t pipe_bytes_written;

pipe_bool_t Pipe_Create(void) { return FALSE; }
pipe_dword_t Pipe_AvailableBytes(void) { return 0; }
pipe_bool_t Pipe_ConnectToNew(void) { return FALSE; }
pipe_bool_t Pipe_ConnectToExisting(void) { return FALSE; }
pipe_bool_t Pipe_Write(const char *format, ...) { (void)format; return FALSE; }
pipe_bool_t Pipe_Read(void) { return FALSE; }
void Pipe_Close(void) {}
pipe_bool_t Pipe_IsConnected(void) { return FALSE; }
void Pipe_BeginConnect(void) {}

#endif /* _WIN32 */
