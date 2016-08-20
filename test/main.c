#include <stdio.h>

#include "utf8.h"

void test1();
void test2();
void test3();
void test4();

static int failed = 0;

int main()
{
    test1();
    test2();
    test3();
    test4();

    return failed;
}

#define ANSI_COLOR_RED      "\x1b[31m"
#define ANSI_COLOR_BOLD_RED "\x1b[1;31m"
#define ANSI_COLOR_RESET    "\x1b[0m"

#define ASSERT_MSG(cnd, args...) \
    if (!(cnd)) { \
        failed = 1; \
        fprintf(stderr, ANSI_COLOR_BOLD_RED); \
        fprintf(stderr, __FILE__ ":%d\n\t", __LINE__); \
        fprintf(stderr, args); \
        fprintf(stderr, "\n"); \
        fprintf(stderr, ANSI_COLOR_RESET); \
    }

#define ASSERT_(cnd) \
    if (!(cnd)) { \
        failed = 1; \
        fprintf(stderr, ANSI_COLOR_BOLD_RED); \
        fprintf(stderr, "Assertion failed at %s:%d\n", __FILE__, __LINE__); \
        fprintf(stderr, ANSI_COLOR_RESET); \
    }

void test1()
{
    printf("test1\n");

    utf8_string str;
    utf8_string_init(&str);
    utf8_string_push_char(&str, 'a');
    utf8_string_push_char(&str, 'b');
    ASSERT_(utf8_string_len(&str) == 2);
    ASSERT_(utf8_string_pop_char(&str) == 'b');
    ASSERT_(utf8_string_len(&str) == 1);
    ASSERT_(utf8_string_pop_char(&str) == 'a');
    ASSERT_(utf8_string_len(&str) == 0);
    utf8_string_free(&str);
}

void test2()
{
    printf("test2\n");

    utf8_string str;
    utf8_string_init(&str);
    utf8_string_push_char(&str, 'a');
    utf8_string_push_char(&str, 'b');

    utf8_char ch;
    utf8_string_char_at(&str, 0, &ch);
    ASSERT_(ch == 'a');
    utf8_string_char_at(&str, 1, &ch);
    ASSERT_(ch == 'b');
    utf8_string_free(&str);
}

void test3()
{
    printf("test3\n");

    utf8_string str;
    utf8_string_init(&str);

    {
        uint8_t buf[1000];
        FILE* strings_file = fopen("test/strings", "r");
        int len = fread((void*)buf, 1, 1000, strings_file);
        fclose(strings_file);
        utf8_string_push_bytes(&str, (void*)buf, len, 14);
    }

    ASSERT_(utf8_string_len(&str) == 14);
    ASSERT_(utf8_string_pop_char(&str) == '\n');
    ASSERT_(utf8_string_len(&str) == 13);
    ASSERT_(utf8_string_pop_char(&str) == 'İ');
    ASSERT_(utf8_string_len(&str) == 12);
    ASSERT_(utf8_string_pop_char(&str) == 'Ş');
    ASSERT_(utf8_string_pop_char(&str) == 'Ü');
    ASSERT_(utf8_string_pop_char(&str) == 'Ğ');
    ASSERT_(utf8_string_pop_char(&str) == 'Ç');
    ASSERT_(utf8_string_pop_char(&str) == 'Ö');
    ASSERT_(utf8_string_pop_char(&str) == '\n');
    ASSERT_(utf8_string_len(&str) == 6);
    ASSERT_(utf8_string_pop_char(&str) == 'ı');
    ASSERT_(utf8_string_pop_char(&str) == 'ş');
    ASSERT_(utf8_string_pop_char(&str) == 'ü');
    ASSERT_(utf8_string_pop_char(&str) == 'ğ');
    ASSERT_(utf8_string_pop_char(&str) == 'ç');
    ASSERT_(utf8_string_pop_char(&str) == 'ö');
    ASSERT_(utf8_string_len(&str) == 0);

    utf8_string_free(&str);
}

void test4()
{
    printf("test4\n");

    utf8_string str;
    utf8_string_init(&str);

    utf8_string_push_char(&str, 'ı');
    utf8_string_push_char(&str, 'İ');
    ASSERT_(utf8_string_len(&str) == 2);

    utf8_char ch;
    utf8_string_char_at(&str, 0, &ch);
    ASSERT_(ch == 'ı');
    utf8_string_char_at(&str, 1, &ch);
    ASSERT_(ch == 'İ');

    utf8_string_free(&str);
}
