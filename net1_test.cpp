#include <windows.h>
#include <cstdio>
#include <cassert>
#include <string>
#include <vector>
#include "MString.hpp"
#include "util.h"
#include "client.h"

// 使用するURLの一部。
//auto g_url_base = GOOGLE_JAPANESE_CONVERT_URL;
auto g_url_base = MZ_LOCAL_JAPANESE_CONVERT_URL;
//const char *g_url_base = NULL;

// 日本語変換のテスト関数。
static BOOL
MZ_ImeConvertTest(const char *utf8_text, const char *url_base)
{
    printf("utf8_text: '%s'\n", MZ_ConFromUtf8(utf8_text).c_str());
    std::string utf8_result;
    HRESULT hr = MZ_ImeQuery(utf8_result, utf8_text, url_base);
    printf("MZ_ImeQuery: 0x%08lX: '%s'\n", hr, MZ_ConFromUtf8(utf8_result.c_str()).c_str());
    return SUCCEEDED(hr);
}

int main(void)
{
    BOOL ret;
#define DO_TEST(u8_text) do { \
    ret = MZ_ImeConvertTest((u8_text), g_url_base); \
    assert(ret); \
} while (0)

    // 日本語変換のテスト。
    DO_TEST(u8"てすと");
    DO_TEST(u8"かんじ");
    DO_TEST(u8"にほんごにゅうりょく");
    DO_TEST(u8"ちょうさいんがでむきます。");
    DO_TEST(u8"きしゃがきしゃで、きしゃした。");

    return 0;
}
