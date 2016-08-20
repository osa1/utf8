#ifndef __UTF8_H
#define __UTF8_H

#include <stdint.h>

// utf8_string invariants:
//
// - buf[len] == 0
// - text_len < len
// - len > 0 (there's always a zero termination in place)
// - cap >= len

typedef struct utf8_string_
{
    /// Text buffer. Always zero-terminated.
    uint8_t*    buf;

    /// Buffer size.
    int         cap;

    /// Size of the used region of the buffer.
    int         len;

    /// Length of the text. (e.g. characters shown when rendered)
    int         text_len;
} utf8_string;

typedef uint32_t utf8_char;
typedef uint8_t  ascii_char;

void utf8_string_init(utf8_string* str);

void utf8_string_free(utf8_string* str);

/// O(n). Extend the buffer if necessary to make room.
void utf8_string_grow(utf8_string* str, int room_needed);

/// O(1). Push a utf8 character to the end of the buffer. Extends the buffer
/// when necessary.
void utf8_string_push_char(utf8_string* str, utf8_char ch);

/// O(1). Try to pop a character. Returns 0 if string is empty.
utf8_char utf8_string_pop_char(utf8_string* str);

/// O(1). Push an ASCII character to the end of the buffer. Extends the buffer
/// when necessary.
void utf8_string_push_ascii_char(utf8_string* str, ascii_char ch);

/// O(n). Push bytes to the end of the buffer. Extends the buffer when
/// necessary. May break invariants if n_chars is not correct. Use with
/// caution.
void utf8_string_push_bytes(utf8_string* str, uint8_t* bytes, int n_bytes, int n_chars);

/// O(1). Returns length of the string in bytes.
int utf8_string_buf_len(utf8_string* str);

/// O(1). Returns length of the text in characters.
int utf8_string_len(utf8_string* str);

/// O(1). Read char at given byte offset. Returns number of bytes read (i.e.
/// width of the utf8_char in bytes).
int utf8_string_char_at_byte_offset(utf8_string* str, int offset, utf8_char* out);

/// O(n). Read char at given offset. Returns number of bytes skipped.
int utf8_string_char_at(utf8_string* str, int char_offset, utf8_char* out);

#endif
