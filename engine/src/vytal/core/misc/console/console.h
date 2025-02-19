#pragma once

#include "vytal/defines/core/console.h"
#include "vytal/defines/core/containers.h"
#include "vytal/defines/shared.h"

VYTAL_API ConsoleResult misc_console_startup(void);
VYTAL_API ConsoleResult misc_console_shutdown(void);

VYTAL_API void misc_console_reset(void);

VYTAL_API void misc_console_set_title(ConstStr title);

// color ---------------------------------------------------------------- //

VYTAL_API void misc_console_set_foreground(ConsoleColor color);
VYTAL_API void misc_console_set_background(ConsoleColor color);
VYTAL_API void misc_console_set_foreground_rgb(Int32 r, Int32 g, Int32 b);
VYTAL_API void misc_console_set_background_rgb(Int32 r, Int32 g, Int32 b);

// clear ---------------------------------------------------------------- //

VYTAL_API void misc_console_clearscr(void);
VYTAL_API void misc_console_clearscr_to_top(void);
VYTAL_API void misc_console_clearscr_to_bottom(void);
VYTAL_API void misc_console_clearln(void);
VYTAL_API void misc_console_clearln_to_left(void);
VYTAL_API void misc_console_clearln_to_right(void);

// cursor --------------------------------------------------------------- //

VYTAL_API void misc_console_move_cursor_up(Int32 positions);
VYTAL_API void misc_console_move_cursor_down(Int32 positions);
VYTAL_API void misc_console_move_cursor_left(Int32 positions);
VYTAL_API void misc_console_move_cursor_right(Int32 positions);
VYTAL_API void misc_console_move_cursor_to(Int32 row, Int32 column);
VYTAL_API void misc_console_save_cursor(void);
VYTAL_API void misc_console_restore_cursor(void);

// input ---------------------------------------------------------------- //

VYTAL_API ConsoleResult misc_console_readln(String *out_str);

// output --------------------------------------------------------------- //

VYTAL_API void misc_console_write(ConstStr format, ...);
VYTAL_API void misc_console_writeln(ConstStr format, ...);

// miscellaneous -------------------------------------------------------- //

VYTAL_API void misc_console_bold(void);
VYTAL_API void misc_console_faint(void);
VYTAL_API void misc_console_italic(void);
VYTAL_API void misc_console_underline(void);
VYTAL_API void misc_console_overline(void);
VYTAL_API void misc_console_reverse(void);
VYTAL_API void misc_console_strike(void);
VYTAL_API void misc_console_conceal(void);
VYTAL_API void misc_console_reveal(void);
