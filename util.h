#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <cstdlib>

// 配列の長さ。
#ifndef _countof
    #define _countof(array) (sizeof(array) / sizeof(array[0]))
#endif

// 日本語の言語IDなど。
#define LANGID_JAPANESE MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT)
#define LCID_JAPANESE MAKELCID(LANGID_JAPANESE, SORT_DEFAULT)

// デフォルトのポート番号の文字列。
#define DEFAULT_PORT "8081"

// デフォルトのバッファの長さ。
#define DEFAULT_BUFLEN (2 * 1024) /* 2 KB */

// 文字コード変換。
std::string MZ_AnsiFromWide(LPCWSTR pszWide, INT codepage = CP_ACP);
std::wstring MZ_WideFromAnsi(LPCSTR pszAnsi, INT codepage = CP_ACP);
std::string MZ_AnsiFromUtf8(const char *utf8);
std::string MZ_Utf8FromAnsi(const char *ansi);
#define MZ_Utf8FromWide(pszWide) MZ_AnsiFromWide((pszWide), CP_UTF8)
#define MZ_WideFromUtf8(pszUtf8) MZ_WideFromAnsi((pszUtf8), CP_UTF8)

// 文字種変換。
std::wstring MZ_LCMapText(LPCWSTR text, DWORD dwFlags);

// ひらがな・カタカナ変換。
std::wstring MZ_HiraFromKata(LPCWSTR pszKatakana);
std::wstring MZ_KataFromHira(LPCWSTR pszHiragana);

// 半角・全角変換。
std::wstring MZ_ZenFromHan(LPCWSTR pszHankaku);
std::wstring MZ_HanFromZen(LPCWSTR pszZenkaku);

// 全角英数から半角へ。
std::wstring MZ_HanFromZenAscii(LPCWSTR pszZenAscii);

// ローマ字からひらがなへ。
std::wstring MZ_HiraFromRomaji(LPCWSTR pszRomaji, BOOL bFinishN = TRUE);

// 読点またはカンマか？
// 注意：変換しやすさのため、ずぼらな判定にしています。
BOOL MZ_IsComma(WCHAR ch);

// ハイフンまたは長音か？
// 注意：変換しやすさのため、ずぼらな判定にしています。
BOOL MZ_IsHyphen(WCHAR ch);

// ひらがなか？
BOOL MZ_IsHira(WCHAR ch);

// 全角カタカナか？
BOOL MZ_IsZenKata(WCHAR ch);
// 半角カナか？
BOOL MZ_IsHanKata(WCHAR ch);
// 漢字か？
BOOL MZ_IsKanji(WCHAR ch);
// 教育漢字か？
BOOL MZ_IsEducationKanji(WCHAR ch);
// 常用漢字か？
BOOL MZ_IsCommonUseKanji(WCHAR ch);
// 全角ASCIIか？
BOOL MZ_IsZenAscii(WCHAR ch);

// 濁音処理。
WCHAR MZ_ConvertDakuon(WCHAR ch0, WCHAR ch1);

// URLエンコード＆デコード。
std::string MZ_UrlEncode(const char *str);
std::string MZ_UrlDecode(const char *str);
