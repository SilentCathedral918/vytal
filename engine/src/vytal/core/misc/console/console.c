#include "console.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "vytal/core/containers/string/string.h"

#if defined(_MSC_VER)
#    include <windows.h>

#    if !defined(ENABLE_VIRTUAL_TERMINAL_PROCESSING)
#        define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#    endif

static HANDLE _out_handle       = INVALID_HANDLE_VALUE;
static DWORD  _console_out_mode = 0;

ConsoleResult misc_console_startup(void) {
    _out_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (_out_handle == INVALID_HANDLE_VALUE)
        return CONSOLE_ERROR_INVALID_HANDLE;

    if (!GetConsoleMode(_out_handle, &_console_out_mode))
        return CONSOLE_ERROR_GET_MODE_FAILED;

    // enable ANSI escape codes
    DWORD out_mode_ = _console_out_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(_out_handle, out_mode_))
        return CONSOLE_ERROR_SET_MODE_FAILED;

    return CONSOLE_SUCCESS;
}

ConsoleResult misc_console_shutdown(void) {
    // restore console to default state
    misc_console_reset();

    // reset the console mode
    if (!SetConsoleMode(_out_handle, _console_out_mode))
        return CONSOLE_ERROR_SET_MODE_FAILED;

    return CONSOLE_SUCCESS;
}

#else
ConsoleResult misc_console_startup(void) {
    // do nothing
    return CONSOLE_SUCCESS;
}

ConsoleResult misc_console_shutdown(void) {
    // restore console to default state
    misc_console_reset();

    return CONSOLE_SUCCESS;
}

#endif

void misc_console_reset(void) {
    fprintf(stdout, "\x1b[0m");
}

void misc_console_set_title(ConstStr title) {
#if defined(_MSC_VER)
    SetConsoleTitleA(title);
#else
    fprintf(stdout, "\x1b]2;%s\x07", title);
#endif
}

// color ---------------------------------------------------------------- //

void misc_console_set_foreground(ConsoleColor color) {
    fprintf(stdout, "\x1b[38;5;%dm", color);
}

void misc_console_set_background(ConsoleColor color) {
    fprintf(stdout, "\x1b[48;5;%dm", color);
}

void misc_console_set_foreground_rgb(Int32 r, Int32 g, Int32 b) {
    fprintf(stdout, "\x1b[38;2;%d;%d;%dm", r, g, b);
}

void misc_console_set_background_rgb(Int32 r, Int32 g, Int32 b) {
    fprintf(stdout, "\x1b[48;2;%d;%d;%dm", r, g, b);
}

// clear ---------------------------------------------------------------- //

enum ClearMode {
    CLEARMODE_FROM_CURSOR_TO_END,
    CLEARMODE_FROM_CURSOR_TO_BEGIN,
    CLEARMODE_ALL
};

void misc_console_clearscr(void) {
    fprintf(stdout, "\x1b[%dJ", CLEARMODE_ALL);
}

void misc_console_clearscr_to_top(void) {
    fprintf(stdout, "\x1b[%dJ", CLEARMODE_FROM_CURSOR_TO_BEGIN);
}

void misc_console_clearscr_to_bottom(void) {
    fprintf(stdout, "\x1b[%dJ", CLEARMODE_FROM_CURSOR_TO_END);
}

void misc_console_clearln(void) {
#if defined(VYTAL_PLATFORM_WINDOWS)
    fprintf(stdout, "\x1b[%dK\r", CLEARMODE_ALL);
#else
    fprintf(stdout, "\x1b[%dK", CLEARMODE_ALL);
#endif
}

void misc_console_clearln_to_left(void) {
    fprintf(stdout, "\x1b[%dK", CLEARMODE_FROM_CURSOR_TO_BEGIN);
}

void misc_console_clearln_to_right(void) {
    fprintf(stdout, "\x1b[%dK", CLEARMODE_FROM_CURSOR_TO_END);
}

// cursor --------------------------------------------------------------- //

void misc_console_move_cursor_up(Int32 positions) {
    fprintf(stdout, "\x1b[%dA", positions);
}

void misc_console_move_cursor_down(Int32 positions) {
    fprintf(stdout, "\x1b[%dB", positions);
}

void misc_console_move_cursor_left(Int32 positions) {
    fprintf(stdout, "\x1b[%dD", positions);
}

void misc_console_move_cursor_right(Int32 positions) {
    fprintf(stdout, "\x1b[%dC", positions);
}

void misc_console_move_cursor_to(Int32 row, Int32 column) {
    fprintf(stdout, "\x1b[%d;%df", row, column);
}

void misc_console_save_cursor(void) {
    fprintf(stdout, "\x1b[s");
}

void misc_console_restore_cursor(void) {
    fprintf(stdout, "\x1b[u");
}

// input ---------------------------------------------------------------- //

ConsoleResult misc_console_readln(String *out_str) {
    Char buffer_[LINE_BUFFER_MAX_SIZE] = {'\0'};
    if (!fgets(buffer_, LINE_BUFFER_MAX_SIZE, stdin)) return CONSOLE_ERROR_READ_FAILED;

    buffer_[strcspn(buffer_, "\n")] = '\0';
    if (container_string_construct(buffer_, out_str) != CONTAINER_SUCCESS)
        return CONSOLE_ERROR_READ_FAILED;

    return CONSOLE_SUCCESS;
}

// output --------------------------------------------------------------- //

void misc_console_write(ConstStr format, ...) {
    VaList va_list_;
    Char   buffer_[LINE_BUFFER_MAX_SIZE] = {'\0'};

    // format writing to buffer
    {
        va_start(va_list_, format);
        vsnprintf(buffer_, LINE_BUFFER_MAX_SIZE, format, va_list_);
        va_end(va_list_);
    }

    fprintf(stdout, "%s", buffer_);
}

void misc_console_writeln(ConstStr format, ...) {
    VaList va_list_;
    Char   buffer_[LINE_BUFFER_MAX_SIZE] = {'\0'};

    // format writing to buffer
    {
        va_start(va_list_, format);
        vsnprintf(buffer_, LINE_BUFFER_MAX_SIZE, format, va_list_);
        va_end(va_list_);
    }

    fprintf(stdout, "%s\n", buffer_);
}

// miscellaneous -------------------------------------------------------- //

void misc_console_bold(void) {
    fprintf(stdout, "\x1b[1m");
}

void misc_console_faint(void) {
    fprintf(stdout, "\x1b[2m");
}

void misc_console_italic(void) {
    fprintf(stdout, "\x1b[3m");
}

void misc_console_underline(void) {
    fprintf(stdout, "\x1b[4m");
}

void misc_console_overline(void) {
    fprintf(stdout, "\x1b[53m");
}

void misc_console_reverse(void) {
    fprintf(stdout, "\x1b[7m");
}

void misc_console_strike(void) {
    fprintf(stdout, "\x1b[9m");
}

void misc_console_conceal(void) {
    fprintf(stdout, "\x1b[8m");
}

void misc_console_reveal(void) {
    fprintf(stdout, "\x1b[28m");
}
