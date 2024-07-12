#pragma once

#include <vector>
#include <string>

// IME変換に使用するURLの一部。MZ_ImeQuery関数で使う。
#define GOOGLE_JAPANESE_CONVERT_URL \
    "http://www.google.com/transliterate?langpair=ja-Hira|ja&text="
#define MZ_LOCAL_JAPANESE_CONVERT_URL \
    "http://localhost:" DEFAULT_PORT "?langpair=ja-Hira|ja&text="

// HTTPリクエスト。
HRESULT MZ_RequestHttp(std::string& result, LPCSTR encoded_url);

// IME変換の問合せ。
HRESULT MZ_ImeQuery(std::string& utf8_result, const char *utf8_raw, const char *url_base = NULL);
