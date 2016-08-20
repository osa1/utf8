#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utf8.h"

#define INITIAL_CAP 10

void utf8_string_init(utf8_string* str)
{
    str->buf = malloc(INITIAL_CAP);
    memset(str->buf, 0, INITIAL_CAP);
    str->cap = INITIAL_CAP;
    str->len = 0;
    str->text_len = 0;
}

void utf8_string_free(utf8_string* str)
{
    free(str->buf);
}

void utf8_string_grow(utf8_string* str, int room_needed)
{
    assert(str->buf != NULL);
    assert(str->cap >= str->len);
    assert(room_needed >= 0);

    int cap_left = str->cap - str->len;
    if (cap_left >= room_needed)
        return;

    // Otherwise double the size until we have enough room.
    int cap = str->cap;
    while (cap < str->len + room_needed)
        cap *= 2;

    str->buf = realloc(str->buf, cap);
    memset(str->buf + str->cap, 0, cap - str->cap);
    str->cap = cap;
}

void utf8_string_push_char(utf8_string* str, utf8_char ch)
{
    assert(str->buf[str->len] == 0);

    // 0b0xxxxxxx
    if (ch <= 0x7F)
    {
        // Ascii character
        utf8_string_push_ascii_char(str, ch);
    }
    // 110xxxxx 10xxxxxx
    else if (ch >= 0xC080 && ch <= 0xDFBF)
    {
        // 2-byte character
        utf8_string_grow(str, 2);
        int len = str->len;
        str->buf[len    ] = (uint8_t)(ch >> 8);
        str->buf[len + 1] = (uint8_t)(ch     );
        str->buf[len + 2] = 0;
        str->len = len + 2;
        str->text_len += 1;
    }
    // 1110xxxx 10xxxxxx 10xxxxxx
    else if (ch >= 0xE08080 && ch <= 0xEFBFBF)
    {
        // 3-byte character
        utf8_string_grow(str, 3);
        int len = str->len;
        str->buf[len    ] = (uint8_t)(ch >> 16);
        str->buf[len + 1] = (uint8_t)(ch >>  8);
        str->buf[len + 2] = (uint8_t)(ch      );
        str->buf[len + 3] = 0;
        str->len = len + 3;
        str->text_len += 1;
    }
    // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    else if (ch >= 0xF0808080 && ch <= 0xF7BFBFBF)
    {
        // 4-byte character
        utf8_string_grow(str, 4);
        int len = str->len;
        str->buf[len    ] = (uint8_t)(ch >> 24);
        str->buf[len + 1] = (uint8_t)(ch >> 16);
        str->buf[len + 2] = (uint8_t)(ch >>  8);
        str->buf[len + 3] = (uint8_t)(ch      );
        str->buf[len + 4] = 0;
        str->len = len + 4;
        str->text_len += 1;
    }
    else
    {
        fprintf(stderr, "invalid utf8 character: %d\n", ch);
    }
}

utf8_char utf8_string_pop_char(utf8_string* str)
{
    if (str->len == 0)
        return 0;

    int char_len = 1;
    int char_idx = str->len - 1;
    while ((str->buf[char_idx] >> 6) == 2)
    {
        ++char_len;
        --char_idx;
    }

    utf8_char ret;
    int char_bytes = utf8_string_char_at_byte_offset(str, char_idx, &ret);
    assert(char_bytes == char_len);

    str->len -= char_len;
    str->buf[str->len] = 0;
    str->text_len -= 1;

    return ret;
}

void utf8_string_push_byte(utf8_string* str, uint8_t byte)
{
    assert(byte <= 0x7F);
    utf8_string_grow(str, 1);
    str->buf[str->len    ] = byte;
    str->buf[str->len + 1] = 0;
    str->len += 1;
}

void utf8_string_push_ascii_char(utf8_string* str, ascii_char ch)
{
    utf8_string_push_byte(str, ch);
    str->text_len += 1;
}

void utf8_string_push_bytes(utf8_string* str, uint8_t* bytes, int n_bytes, int n_chars)
{
    utf8_string_grow(str, n_bytes);
    // remember that str->buf[str->len] always has null terminator
    memcpy(str->buf + str->len, bytes, n_bytes);
    str->len += n_bytes;
    str->buf[str->len] = 0;
    str->text_len += n_chars;
}

int utf8_string_buf_len(utf8_string* str)
{
    return str->len;
}

int utf8_string_len(utf8_string* str)
{
    return str->text_len;
}

int char_at_byte_offset(uint8_t* buf, int offset, utf8_char* out);

int utf8_string_char_at_byte_offset(utf8_string* str, int offset, utf8_char* out)
{
    assert(offset < str->len);
    return char_at_byte_offset(str->buf, offset, out);
}

int char_at_byte_offset(uint8_t* buf, int offset, utf8_char* out)
{
    utf8_char ch = (utf8_char)(buf[offset]);
    if (ch == 0)
    {
        return 0;
    }
    else if (ch <= 0x007F)
    {
        *out = ch;
        return 1;
    }
    else if (ch >= 0x80 && ch <= 0x07FF)
    {
        ch = (ch << 8) + (utf8_char)(buf[offset + 1]);
        *out = ch;
        return 2;
    }
    else if (ch >= 0x0800 && ch <= 0xFFFF)
    {
        ch = ch << 16;
        ch = ch | ((utf8_char)(buf[offset + 1]) << 8);
        ch = ch | ((utf8_char)(buf[offset + 2]) << 16);
        *out = ch;
        return 3;
    }
    else if (ch >= 0x00010000 && ch <= 0x0010FFFF)
    {
        ch = ch << 24;
        ch = ch | ((utf8_char)(buf[offset + 1]) << 16);
        ch = ch | ((utf8_char)(buf[offset + 2]) << 8);
        ch = ch |  (utf8_char)(buf[offset + 3]);
        *out = ch;
        return 4;
    }

    return 0;
}

int utf8_string_char_at(utf8_string* str, int char_offset, utf8_char* out)
{
    assert(char_offset < str->text_len);
    assert(char_offset >= 0);

    uint8_t* buf = str->buf;
    int byte_offset = 0;

    while (char_offset != 0)
    {
        byte_offset += char_at_byte_offset(buf, byte_offset, out);
        --char_offset;
    }

    return char_at_byte_offset(buf + byte_offset, 0, out);
}
