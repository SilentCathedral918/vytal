#pragma once

#include "vytal/defines/core/mem.h"
#include "vytal/defines/core/types.h"
#include "vytal/defines/shared.h"

#include <stdio.h>

VT_API void misc_console_startup(void);
VT_API void misc_console_shutdown(void);

VT_API VT_INLINE void misc_console_reset(void) { fprintf(stdout, "\x1b[0m"); }

// -------------------------------- color -------------------------------- //

VT_API VT_INLINE void misc_console_setforeground(Int32 code) { fprintf(stdout, "\x1b[38;5;%dm", code); }
VT_API VT_INLINE void misc_console_setbackground(Int32 code) { fprintf(stdout, "\x1b[48;5;%dm", code); }
VT_API VT_INLINE void misc_console_setforeground_rgb(Int32 r, Int32 g, Int32 b) { fprintf(stdout, "\x1b[38;2;%d;%d;%dm", r, g, b); }
VT_API VT_INLINE void misc_console_setbackground_rgb(Int32 r, Int32 g, Int32 b) { fprintf(stdout, "\x1b[48;2;%d;%d;%dm", r, g, b); }

// -------------------------------- clear -------------------------------- //

enum ClearMode { CLEARMODE_FROM_CURSOR_TO_END, CLEARMODE_FROM_CURSOR_TO_BEGIN, CLEARMODE_ALL };

VT_API VT_INLINE void misc_console_clearscr(void) { fprintf(stdout, "\x1b[%dJ", CLEARMODE_ALL); }
VT_API VT_INLINE void misc_console_clearscr_totop(void) { fprintf(stdout, "\x1b[%dJ", CLEARMODE_FROM_CURSOR_TO_BEGIN); }
VT_API VT_INLINE void misc_console_clearscr_tobottom(void) { fprintf(stdout, "\x1b[%dJ", CLEARMODE_FROM_CURSOR_TO_END); }
VT_API VT_INLINE void misc_console_clearln(void) { fprintf(stdout, "\x1b[%dK", CLEARMODE_ALL); }
VT_API VT_INLINE void misc_console_clearln_toleft(void) { fprintf(stdout, "\x1b[%dK", CLEARMODE_FROM_CURSOR_TO_BEGIN); }
VT_API VT_INLINE void misc_console_clearln_toright(void) { fprintf(stdout, "\x1b[%dK", CLEARMODE_FROM_CURSOR_TO_END); }

// -------------------------------- cursor -------------------------------- //

VT_API VT_INLINE void misc_console_movecursorup(Int32 positions) { fprintf(stdout, "\x1b[%dA", positions); }
VT_API VT_INLINE void misc_console_movecursordown(Int32 positions) { fprintf(stdout, "\x1b[%dB", positions); }
VT_API VT_INLINE void misc_console_movecursorleft(Int32 positions) { fprintf(stdout, "\x1b[%dD", positions); }
VT_API VT_INLINE void misc_console_movecursorright(Int32 positions) { fprintf(stdout, "\x1b[%dC", positions); }
VT_API VT_INLINE void misc_console_movecursorto(Int32 row, Int32 column) { fprintf(stdout, "\x1b[%d;%df", row, column); }

// -------------------------------- input -------------------------------- //

VT_API Str misc_console_readln(void);

// -------------------------------- output -------------------------------- //

VT_API void misc_console_write(ConstStr format, ...);
VT_API void misc_console_writeln(ConstStr format, ...);

// -------------------------------- misc. -------------------------------- //

VT_API VT_INLINE void misc_console_bold(void) { fprintf(stdout, "\x1b[1m"); }
VT_API VT_INLINE void misc_console_faint(void) { fprintf(stdout, "\x1b[2m"); }
VT_API VT_INLINE void misc_console_italic(void) { fprintf(stdout, "\x1b[2m"); }
VT_API VT_INLINE void misc_console_underline(void) { fprintf(stdout, "\x1b[4m"); }
VT_API VT_INLINE void misc_console_overline(void) { fprintf(stdout, "\x1b[53m"); }
VT_API VT_INLINE void misc_console_reverse(void) { fprintf(stdout, "\x1b[7m"); }
VT_API VT_INLINE void misc_console_strike(void) { fprintf(stdout, "\x1b[9m"); }
VT_API VT_INLINE void misc_console_conceal(void) { fprintf(stdout, "\x1b[8m"); }
VT_API VT_INLINE void misc_console_reveal(void) { fprintf(stdout, "\x1b[28m"); }
