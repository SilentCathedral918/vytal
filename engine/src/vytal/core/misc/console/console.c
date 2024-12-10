#include "console.h"

#include "vytal/core/misc/string/vtstr.h"

#define CONSOLE_OUTPUT_BUFFER_MAX_SIZE 16384 // 16 KB

#if defined(VT_PLATFORM_WINDOWS)
#    include <Windows.h>

#    if !defined(ENABLE_VIRTUAL_TERMINAL_PROCESSING)
#        define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#    endif

static HANDLE _out_handle       = NULL;
static DWORD  _console_out_mode = 0;

void misc_console_startup(void) {
    _out_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (_out_handle == INVALID_HANDLE_VALUE)
        exit(GetLastError());

    if (!GetConsoleMode(_out_handle, &_console_out_mode))
        exit(GetLastError());

    // set the console mode to enable ANSI escape codes
    DWORD out_mode_ = _console_out_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(_out_handle, out_mode_))
        exit(GetLastError());
}

void misc_console_shutdown(void) {
    // restore console to default state
    misc_console_reset();

    // reset the console mode
    if (!SetConsoleMode(_out_handle, _console_out_mode))
        exit(GetLastError());
}

#else
void misc_console_shutdown(void) {
    // do nothing
    return;
}

void misc_console_shutdown(void) {
    // restore console to default state
    misc_console_reset();
}

#endif

Str misc_console_readln(void) {
    Char buffer_[CONSOLE_OUTPUT_BUFFER_MAX_SIZE] = {'\0'};
    fgets(buffer_, CONSOLE_OUTPUT_BUFFER_MAX_SIZE, stdin);
    fflush(stdin);

    // replace '\n' with '\0'
    {
        ByteSize length     = misc_str_strlen(buffer_);
        buffer_[length - 1] = '\0';
    }

    return misc_str_strdup(buffer_);
}

void misc_console_write(ConstStr format, ...) {
    VaList arg_ls_;
    Char   buffer[CONSOLE_OUTPUT_BUFFER_MAX_SIZE] = {'\0'};

    va_start(arg_ls_, format);
    vsnprintf(buffer, CONSOLE_OUTPUT_BUFFER_MAX_SIZE, format, arg_ls_);
    va_end(arg_ls_);

    fprintf(stdout, "%s", buffer);
}

void misc_console_writeln(ConstStr format, ...) {
    VaList arg_ls_;
    Char   buffer[CONSOLE_OUTPUT_BUFFER_MAX_SIZE] = {'\0'};

    va_start(arg_ls_, format);
    vsnprintf(buffer, CONSOLE_OUTPUT_BUFFER_MAX_SIZE, format, arg_ls_);
    va_end(arg_ls_);

    fprintf(stdout, "%s\n", buffer);
    fflush(stdout);
}
