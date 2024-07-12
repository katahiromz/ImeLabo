#include <windows.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>
#include "MString.hpp"
#include "util.h"
#include "client.h"
#include "convert.h"

BOOL g_bDotIsMaru = FALSE;
BOOL g_bCommaIsTen = FALSE;

// 変換に必要な初期化を行う関数。
BOOL MZ_LoadDictSet(void)
{
    // TODO: 変換に必要な辞書などを読み込む。
    return TRUE;
}

// MZ_LoadDictSet関数で読み込んだデータを解放する関数。
void MZ_UnloadDictSet(void)
{
    // TODO: MZ_LoadDictSet関数で読み込んだデータを解放する。
}

// IME変換前に必要な変換。
HRESULT
MZ_PreImeConvert(std::string& utf8_result, std::vector<std::string> utf8_clauses)
{
    utf8_result.clear();
    for (auto& clause : utf8_clauses)
    {
        if (g_bDotIsMaru)
            mstr_replace_all(clause, u8".", u8"。");
        else
            mstr_replace_all(clause, u8".", u8"．");

        if (g_bCommaIsTen)
            mstr_replace_all(clause, u8",", u8"、");
        else
            mstr_replace_all(clause, u8",", u8"，");

        if (utf8_result.size())
            utf8_result += u8","; // See also: https://www.google.co.jp/ime/cgiapi.html

        utf8_result += clause;
    }

    if (utf8_result.empty())
        return E_FAIL;

    return S_OK;
}

// IME変換の本体。
// TODO: 「Google CGI API for Japanese Input」互換のJSONを返すようにして下さい。
HRESULT
MZ_RealImeConvert(json& root, const std::string& utf8_input)
{
    root.clear();

#ifdef GOOGLE_INSIDE
    std::string utf8_result;
    MZ_ImeQuery(utf8_result, utf8_input.c_str(), GOOGLE_JAPANESE_CONVERT_URL);
    try
    {
        root = json::parse(utf8_result.c_str());
    }
    catch (...)
    {
        root.clear();
        return E_FAIL;
    }
#else
    // TODO: 辞書セットを使って適切に変換するように実装して下さい。
    json clause, trail;
    trail.push_back(utf8_input);
    clause.push_back(utf8_input);
    clause.push_back(trail);
    root.push_back(clause);
#endif

    return S_OK;
}

// 変換前文字列をかな漢字変換する。
std::wstring
MZ_ImeConvert(const std::wstring& text, json *pjson)
{
    printf("DoConvertText('%s')\n", MZ_AnsiFromWide(text.c_str()).c_str());

    std::wstring input = text;
    mstr_trim(input, L" \t\r\n　");

    if (input.empty())
        return L""; // 失敗。

    // JSON は UTF-8 文字列を使う。
    std::string utf8_input = MZ_Utf8FromWide(input.c_str());

    // TODO: 複数文節。
    std::vector<std::string> utf8_clauses;
    utf8_clauses.push_back(utf8_input);

    // IME変換前に必要な事前変換。
    if (S_OK != MZ_PreImeConvert(utf8_input, utf8_clauses))
        return L""; // 失敗。

    json root;
    if (S_OK != MZ_RealImeConvert(root, utf8_input))
        return L""; // 失敗。

    if (pjson)
        *pjson = root;

    std::wstring wide_result;
    try
    {
        wide_result = MZ_WideFromUtf8(root.dump().c_str());
    }
    catch (...)
    {
        wide_result.clear(); // 失敗。
    }

    return wide_result;
}
