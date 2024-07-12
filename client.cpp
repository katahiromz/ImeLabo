#include "util.h"
#include "MString.hpp"
#include "client.h"
#include "convert.h"
#include <cstdio>
#include <cctype>
#include <cstring>
#include <cassert>
#include <algorithm>
#include <urlmon.h>
#pragma comment(lib, "urlmon.lib")

// HTTPリクエスト。
HRESULT
MZ_RequestHttp(std::string& result, LPCSTR encoded_url)
{
    result.clear();

    IStream *pStream = NULL;
    HRESULT hr = URLOpenBlockingStreamA(NULL, encoded_url, &pStream, 0, NULL);
    if (FAILED(hr))
        return hr;

    CHAR szBuff[DEFAULT_BUFLEN];
    ULONG cbRead;
    for (;;)
    {
        hr = pStream->Read(szBuff, _countof(szBuff), &cbRead);
        if (FAILED(hr) || !cbRead)
            break;
        result.append(szBuff, cbRead);
    }

    pStream->Release();
    return S_OK;
}

// IME変換用のURLを構築する。
HRESULT MZ_BuildImeUrl(std::string& url, const char *utf8_raw, const char *url_base)
{
    std::string encoded = MZ_UrlEncode(utf8_raw);
    if (encoded.empty())
        return E_OUTOFMEMORY;

    url = url_base;
    url += encoded;
    return S_OK;
}

// IME変換の問合せ。
HRESULT
MZ_ImeQuery(std::string& utf8_result, const char *utf8_raw, const char *url_base)
{
    utf8_result.clear();

    if (url_base) // Network IME?
    {
        // IME変換用のURLを構築する。
        std::string url;
        HRESULT hr = MZ_BuildImeUrl(url, utf8_raw, url_base);
        if (FAILED(hr))
            return hr;

        // See also: https://www.google.co.jp/ime/cgiapi.html
        return MZ_RequestHttp(utf8_result, url.c_str());
    }
    else // Local IME?
    {
        std::wstring wide = MZ_WideFromUtf8(utf8_raw);
        std::wstring result = MZ_ImeConvert(wide);
        utf8_result = MZ_Utf8FromWide(result.c_str());
        return S_OK;
    }
}
