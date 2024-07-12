#pragma once

#include "nlohmann/json.hpp"
using json = nlohmann::json;

// TODO: 辞書セットを取り扱うのに必要な宣言・定義などを追加して下さい。

// 変換に必要な初期化を行う関数。
BOOL MZ_LoadDictSet(void);

// MZ_LoadDictSet関数で読み込んだデータを解放する関数。
void MZ_UnloadDictSet(void);

// 変換前文字列をかな漢字変換する。
std::wstring MZ_ImeConvert(const std::wstring& text, json *pjson = NULL);

// 辞書読み込みの自動化。
struct CMZDictSet
{
    BOOL m_bLoaded;
    CMZDictSet()
    {
        m_bLoaded = FALSE;
    }
    ~CMZDictSet()
    {
        Unload();
    }
    BOOL Load()
    {
        if (!m_bLoaded)
            m_bLoaded = MZ_LoadDictSet();
        if (!m_bLoaded)
            printf("CMZDictSet::Init failed\n");
        return m_bLoaded;
    }
    void Unload()
    {
        if (m_bLoaded)
        {
            MZ_UnloadDictSet();
            m_bLoaded = FALSE;
        }
    }
};

struct MZ_RESULT_CLAUSE
{
    std::wstring pre;
    std::vector<std::wstring> post;
};

// IME変換前に必要な事前変換。
HRESULT MZ_PreImeConvert(std::string& utf8_result, std::vector<std::string> utf8_clauses);
// IME変換の本体。
HRESULT MZ_ImeConvertBody(json& root, const std::string& utf8_result);
