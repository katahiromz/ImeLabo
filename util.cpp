#include "util.h"
#include "MString.hpp"
#include <cstdio>
#include <cctype>
#include <cstring>
#include <cassert>
#include <algorithm>
using namespace std;

// ANSI文字列からワイド文字列へ。
std::string MZ_AnsiFromWide(LPCWSTR pszWide, INT codepage)
{
    CHAR szAnsi[DEFAULT_BUFLEN];
    WideCharToMultiByte(codepage, 0, pszWide, -1, szAnsi, _countof(szAnsi), NULL, NULL);
    szAnsi[_countof(szAnsi) - 1] = 0; // Avoid buffer overrun
    return szAnsi;
}

// ワイド文字列からANSI文字列へ。
std::wstring MZ_WideFromAnsi(LPCSTR pszAnsi, INT codepage)
{
    WCHAR szWide[DEFAULT_BUFLEN];
    MultiByteToWideChar(codepage, 0, pszAnsi, -1, szWide, _countof(szWide));
    szWide[_countof(szWide) - 1] = 0; // Avoid buffer overrun
    return szWide;
}

// UTF-8文字列からANSI文字列へ。
std::string MZ_AnsiFromUtf8(const char *utf8)
{
    return MZ_AnsiFromWide(MZ_WideFromUtf8(utf8).c_str());
}

// ANSI文字列からUTF-8文字列へ。
std::string MZ_Utf8FromAnsi(const char *ansi)
{
    return MZ_Utf8FromWide(MZ_WideFromAnsi(ansi).c_str());
}

// 文字種変換。
std::wstring MZ_LCMapText(LPCWSTR text, DWORD dwFlags)
{
    WCHAR szBuf[DEFAULT_BUFLEN];
    ::LCMapStringW(LCID_JAPANESE, dwFlags, text, -1, szBuf, _countof(szBuf));
    szBuf[_countof(szBuf) - 1] = 0; // Avoid buffer overrun
    return szBuf;
}

// カタカナからひらがなへ。
std::wstring MZ_HiraFromKata(LPCWSTR pszKatakana)
{
    return MZ_LCMapText(pszKatakana, LCMAP_FULLWIDTH | LCMAP_HIRAGANA);
}

// ひらがなからカタカナへ。
std::wstring MZ_KataFromHira(LPCWSTR pszHiragana)
{
    return MZ_LCMapText(pszHiragana, LCMAP_FULLWIDTH | LCMAP_KATAKANA);
}

// 半角から全角へ。
std::wstring MZ_ZenFromHan(LPCWSTR pszHankaku)
{
    return MZ_LCMapText(pszHankaku, LCMAP_FULLWIDTH);
}

// 全角から半角へ。
std::wstring MZ_HanFromZen(LPCWSTR pszZenkaku)
{
    return MZ_LCMapText(pszZenkaku, LCMAP_HALFWIDTH);
}

// 全角英数から半角へ。
std::wstring MZ_HanFromZenAscii(LPCWSTR pszZenAscii)
{
    std::wstring ret;
    for (size_t i = 0; pszZenAscii[i]; ++i)
    {
        WCHAR ch = pszZenAscii[i];
        if (L'ａ' <= ch && ch <= L'ｚ')
            ch += L'a' - L'ａ';
        else if (L'Ａ' <= ch && ch <= L'Ｚ')
            ch += L'A' - L'Ａ';
        ret += ch;
    }
    return ret;
}

// 読点またはカンマか？
// 注意：変換しやすさのため、ずぼらな判定にしています。
BOOL MZ_IsComma(WCHAR ch)
{
    return ch == L'、' || ch == L'，' || ch == L',' || ch == L'､';
}

// ハイフンまたは長音か？
// 注意：変換しやすさのため、ずぼらな判定にしています。
BOOL MZ_IsHyphen(WCHAR ch)
{
    return ch == L'-' || ch == L'－' || ch == L'ー' || ch == L'ｰ';
}

// ひらがなか？
BOOL MZ_IsHira(WCHAR ch)
{
    if (0x3040 <= ch && ch <= 0x309F)
        return TRUE;
    switch (ch)
    {
    case 0x3095: case 0x3096: case 0x3099: case 0x309A: case 0x309B:
    case 0x309C: case 0x309D: case 0x309E: case 0x30FC:
        return TRUE;
    default:
        return FALSE;
    }
}

// 全角カタカナか？
BOOL MZ_IsZenKata(WCHAR ch)
{
    if (0x30A0 <= ch && ch <= 0x30FF) return TRUE;
    switch (ch)
    {
    case 0x30FD: case 0x30FE: case 0x3099: case 0x309A: case 0x309B:
    case 0x309C: case 0x30FC:
        return TRUE;
    default:
        return FALSE;
    }
}

// 半角カナか？
BOOL MZ_IsHanKata(WCHAR ch)
{
    if (0xFF65 <= ch && ch <= 0xFF9F)
        return TRUE;
    switch (ch)
    {
    case 0xFF61: case 0xFF62: case 0xFF63: case 0xFF64:
        return TRUE;
    default:
        return FALSE;
    }
}

// 漢字か？
BOOL MZ_IsKanji(WCHAR ch)
{
    if (0x4E00 <= ch && ch <= 0x9FFF) return TRUE;
    if (0xF900 <= ch && ch <= 0xFAFF) return TRUE;
    return FALSE;
}

// 教育漢字か？
BOOL MZ_IsEducationKanji(WCHAR ch)
{
    return wcschr(L"一音雨円王火花貝学休九玉空気下月見犬五口校金左三山四子糸字耳七車手十出女小正上森人水生青赤石夕千先川草早足村大男竹虫中町天田土二日入年白八百本名木目文右力立林六引羽雲遠園家何夏科歌画会回海絵外角楽活間顔岩丸記汽帰弓牛魚教強近今兄形京計原言元古戸語午後広交工光公考黄行高合国谷黒細才作算紙矢姉市止時寺自色室社弱首週秋春書少場食新親心図頭数西星声晴雪切船線前組走太多体台弟池地知茶昼長朝鳥直通点店電冬刀当答東道同読内南肉馬買売麦半番父歩風分聞米母方北毎妹万鳴明毛門野夜友曜用来里理話思悪安暗意委医育院員飲運泳駅横央屋温荷開界階館感寒漢岸起期客急級球究去橋業曲局銀苦区具宮君化軽係血決県研湖庫幸向港号根祭皿指始歯死詩仕使次持事式実写者昔酒守主取受習拾集終州重住宿暑所助消昭章商勝乗植身進申深真神世整全想相送息速族他打代対待第題短炭談着注柱調帳追丁定庭笛鉄転都度島等登湯投豆動童農波配倍箱畑発反板坂悲皮鼻美筆氷表病秒品負部福服物平返勉放味命面問薬役由油遊有予葉陽羊洋様落旅両緑流礼列練路和愛案以位胃囲衣印英栄塩億貨課加果芽械改害街覚各関完管官観願喜季旗紀希機器議救泣求給挙協鏡競共極訓軍郡型径景芸結欠建健験固候功好航康告差最菜材昨殺刷札察参散産残氏史試士司治児辞失借種周祝順初唱笑焼松賞象照省信臣清静成席積節折説戦選浅巣争倉束側続卒孫隊帯達単置仲貯兆腸停低底的典伝徒努灯働堂特得毒熱然念敗梅博飯飛費必票標付府不夫無副粉兵別辺変便法包望牧末満未脈民約勇要養浴利陸量良漁料輪類令冷例歴連老労録圧移因永衛営益易液演桜応往恩価過可河仮賀解快確格額慣幹刊眼規基寄技義逆旧久許居境均禁句群経潔険検券件現限減個故護効講構鉱耕厚興混査災採妻際再在罪財雑賛酸支資師志飼枝示似識質謝舎授修述術準序証承招常状条情職織製精性政制勢税責績設接絶舌銭祖素総増像造則測属損貸退態団断築張程提敵適統銅導徳独任燃能破犯版判肥比非備俵評貧富布婦武復複仏編弁保墓豊報防暴夢務迷綿輸余預容率略留領貿律異遺域宇映沿延我灰閣革拡割株簡干看巻危揮机貴疑吸郷胸供筋勤敬系警劇激穴絹憲権厳源己呼誤孝皇后紅鋼降穀刻骨困砂座済裁策冊蚕誌視私詞至姿磁捨射尺若樹宗収衆就従縦縮熟純署処諸除傷障将城蒸針仁推垂寸誠聖盛専宣染泉洗善層装奏操創窓蔵臓尊存宅探担誕暖段値宙忠著潮頂庁賃痛展党討糖届難乳認脳納派肺背俳拝班晩否秘批腹奮陛並閉補暮訪宝棒忘亡枚幕密盟模訳優郵幼翌欲卵乱覧裏臨論朗片", ch) != NULL;
}

// 常用漢字か？
BOOL MZ_IsCommonUseKanji(WCHAR ch)
{
    return wcschr(L"亜哀挨愛曖悪握圧扱宛嵐安案暗以衣位囲医依委威為畏胃尉異移萎偉椅彙意違維慰遺緯域育一壱逸茨芋引印因咽姻員院淫陰飲隠韻右宇羽雨唄鬱畝浦運雲永泳英映栄営詠影鋭衛易疫益液駅悦越謁閲円延沿炎怨宴媛援園煙猿遠鉛塩演縁艶汚王凹央応往押旺欧殴桜翁奥横岡屋億憶臆虞乙俺卸音恩温穏下化火加可仮何花佳価果河苛科架夏家荷華菓貨渦過嫁暇禍靴寡歌箇稼課蚊牙瓦我画芽賀雅餓介回灰会快戒改怪拐悔海界皆械絵開階塊楷解潰壊懐諧貝外劾害崖涯街慨蓋該概骸垣柿各角拡革格核殻郭覚較隔閣確獲嚇穫学岳楽額顎掛潟括活喝渇割葛滑褐轄且株釜鎌刈干刊甘汗缶完肝官冠巻看陥乾勘患貫寒喚堪換敢棺款間閑勧寛幹感漢慣管関歓監緩憾還館環簡観韓艦鑑丸含岸岩玩眼頑顔願企伎危机気岐希忌汽奇祈季紀軌既記起飢鬼帰基寄規亀喜幾揮期棋貴棄毀旗器畿輝機騎技宜偽欺義疑儀戯擬犠議菊吉喫詰却客脚逆虐九久及弓丘旧休吸朽臼求究泣急級糾宮救球給嗅窮牛去巨居拒拠挙虚許距魚御漁凶共叫狂京享供協況峡挟狭恐恭胸脅強教郷境橋矯鏡競響驚仰暁業凝曲局極玉巾斤均近金菌勤琴筋僅禁緊錦謹襟吟銀区句苦駆具惧愚空偶遇隅串屈掘窟熊繰君訓勲薫軍郡群兄刑形系径茎係型契計恵啓掲渓経蛍敬景軽傾携継詣慶憬稽憩警鶏芸迎鯨隙劇撃激桁欠穴血決結傑潔月犬件見券肩建研県倹兼剣拳軒健険圏堅検嫌献絹遣権憲賢謙鍵繭顕験懸元幻玄言弦限原現舷減源厳己戸古呼固股虎孤弧故枯個庫湖雇誇鼓錮顧五互午呉後娯悟碁語誤護口工公勾孔功巧広甲交光向后好江考行坑孝抗攻更効幸拘肯侯厚恒洪皇紅荒郊香候校耕航貢降高康控梗黄喉慌港硬絞項溝鉱構綱酵稿興衡鋼講購乞号合拷剛傲豪克告谷刻国黒穀酷獄骨駒込頃今困昆恨根婚混痕紺魂墾懇左佐沙査砂唆差詐鎖座挫才再災妻采砕宰栽彩採済祭斎細菜最裁債催塞歳載際埼在材剤財罪崎作削昨柵索策酢搾錯咲冊札刷刹拶殺察撮擦雑皿三山参桟蚕惨産傘散算酸賛残斬暫士子支止氏仕史司四市矢旨死糸至伺志私使刺始姉枝祉肢姿思指施師恣紙脂視紫詞歯嗣試詩資飼誌雌摯賜諮示字寺次耳自似児事侍治持時滋慈辞磁餌璽鹿式識軸七叱失室疾執湿嫉漆質実芝写社車舎者射捨赦斜煮遮謝邪蛇尺借酌釈爵若弱寂手主守朱取狩首殊珠酒腫種趣寿受呪授需儒樹収囚州舟秀周宗拾秋臭修袖終羞習週就衆集愁酬醜蹴襲十汁充住柔重従渋銃獣縦叔祝宿淑粛縮塾熟出述術俊春瞬旬巡盾准殉純循順準潤遵処初所書庶暑署緒諸女如助序叙徐除小升少召匠床抄肖尚招承昇松沼昭宵将消症祥称笑唱商渉章紹訟勝掌晶焼焦硝粧詔証象傷奨照詳彰障憧衝賞償礁鐘上丈冗条状乗城浄剰常情場畳蒸縄壌嬢錠譲醸色拭食植殖飾触嘱織職辱尻心申伸臣芯身辛侵信津神唇娠振浸真針深紳進森診寝慎新審震薪親人刃仁尽迅甚陣尋腎須図水吹垂炊帥粋衰推酔遂睡穂随髄枢崇数据杉裾寸瀬是井世正生成西声制姓征性青斉政星牲省凄逝清盛婿晴勢聖誠精製誓静請整醒税夕斥石赤昔析席脊隻惜戚責跡積績籍切折拙窃接設雪摂節説舌絶千川仙占先宣専泉浅洗染扇栓旋船戦煎羨腺詮践箋銭潜線遷選薦繊鮮全前善然禅漸膳繕狙阻祖租素措粗組疎訴塑遡礎双壮早争走奏相荘草送倉捜挿桑巣掃曹曽爽窓創喪痩葬装僧想層総遭槽踪操燥霜騒藻造像増憎蔵贈臓即束足促則息捉速側測俗族属賊続卒率存村孫尊損遜他多汰打妥唾堕惰駄太対体耐待怠胎退帯泰堆袋逮替貸隊滞態戴大代台第題滝宅択沢卓拓託濯諾濁但達脱奪棚誰丹旦担単炭胆探淡短嘆端綻誕鍛団男段断弾暖談壇地池知値恥致遅痴稚置緻竹畜逐蓄築秩窒茶着嫡中仲虫沖宙忠抽注昼柱衷酎鋳駐著貯丁弔庁兆町長挑帳張彫眺釣頂鳥朝貼超腸跳徴嘲潮澄調聴懲直勅捗沈珍朕陳賃鎮追椎墜通痛塚漬坪爪鶴低呈廷弟定底抵邸亭貞帝訂庭逓停偵堤提程艇締諦泥的笛摘滴適敵溺迭哲鉄徹撤天典店点展添転填田伝殿電斗吐妬徒途都渡塗賭土奴努度怒刀冬灯当投豆東到逃倒凍唐島桃討透党悼盗陶塔搭棟湯痘登答等筒統稲踏糖頭謄藤闘騰同洞胴動堂童道働銅導瞳峠匿特得督徳篤毒独読栃凸突届屯豚頓貪鈍曇丼那奈内梨謎鍋南軟難二尼弐匂肉虹日入乳尿任妊忍認寧熱年念捻粘燃悩納能脳農濃把波派破覇馬婆罵拝杯背肺俳配排敗廃輩売倍梅培陪媒買賠白伯拍泊迫剥舶博薄麦漠縛爆箱箸畑肌八鉢発髪伐抜罰閥反半氾犯帆汎伴判坂阪板版班畔般販斑飯搬煩頒範繁藩晩番蛮盤比皮妃否批彼披肥非卑飛疲秘被悲扉費碑罷避尾眉美備微鼻膝肘匹必泌筆姫百氷表俵票評漂標苗秒病描猫品浜貧賓頻敏瓶不夫父付布扶府怖阜附訃負赴浮婦符富普腐敷膚賦譜侮武部舞封風伏服副幅復福腹複覆払沸仏物粉紛雰噴墳憤奮分文聞丙平兵併並柄陛閉塀幣弊蔽餅米壁璧癖別蔑片辺返変偏遍編弁便勉歩保哺捕補舗母募墓慕暮簿方包芳邦奉宝抱放法泡胞俸倣峰砲崩訪報蜂豊飽褒縫亡乏忙坊妨忘防房肪某冒剖紡望傍帽棒貿貌暴膨謀頬北木朴牧睦僕墨撲没勃堀本奔翻凡盆麻摩磨魔毎妹枚昧埋幕膜枕又末抹万満慢漫未味魅岬密蜜脈妙民眠矛務無夢霧娘名命明迷冥盟銘鳴滅免面綿麺茂模毛妄盲耗猛網目黙門紋問冶夜野弥厄役約訳薬躍闇由油喩愉諭輸癒唯友有勇幽悠郵湧猶裕遊雄誘憂融優与予余誉預幼用羊妖洋要容庸揚揺葉陽溶腰様瘍踊窯養擁謡曜抑沃浴欲翌翼拉裸羅来雷頼絡落酪辣乱卵覧濫藍欄吏利里理痢裏履璃離陸立律慄略柳流留竜粒隆硫侶旅虜慮了両良料涼猟陵量僚領寮療瞭糧力緑林厘倫輪隣臨瑠涙累塁類令礼冷励戻例鈴零霊隷齢麗暦歴列劣烈裂恋連廉練錬呂炉賂路露老労弄郎朗浪廊楼漏籠六録麓論和話賄脇惑枠湾腕", ch) != NULL;
}

// 全角ASCIIか？
BOOL MZ_IsZenAscii(WCHAR ch)
{
    return (0xFF00 <= ch && ch <= 0xFFEF);
}

// 濁音処理。
WCHAR MZ_ConvertDakuon(WCHAR ch0, WCHAR ch1)
{
    switch (MAKELONG(ch0, ch1))
    {
#define DEFINE_DAKUON(ch0, ch1, result) case MAKELONG(ch0, ch1): return result;
#include "data/dakuon.h"
#undef DEFINE_DAKUON
    default:
        return 0;
    }
}

// ローマ字からひらがなへ。
std::wstring
MZ_HiraFromRomaji(LPCWSTR pszRomaji, BOOL bFinishN)
{
    static BOOL s_bSorted = FALSE;
    struct KEY_VALUE { LPCWSTR key, value; };
    static KEY_VALUE s_data[] =
    {
#define DEFINE_ROMAJI(key, value) { key, value },
#include "data/romaji.h"
    };
    if (!s_bSorted)
    {
        std::sort(std::begin(s_data), std::end(s_data), [&](const KEY_VALUE& x, const KEY_VALUE& y) {
            if (lstrlenW(x.key) > lstrlenW(y.key))
                return true;
            if (lstrcmpiW(x.key, y.key) > 0)
                return true;
            return false;
        });
        s_bSorted = TRUE;
    }

    // 半角英数にする。
    std::wstring text = MZ_LCMapText(pszRomaji, LCMAP_HALFWIDTH | LCMAP_LOWERCASE);

    // ローマ字変換を行う。
    for (size_t ich = 0; ich < text.size(); ++ich)
    {
        for (auto& pair : s_data)
        {
            INT cchPart = lstrlenW(pair.key);
            if (cchPart > INT(text.size() - ich))
                continue;
            if (memcmp(pair.key, &text[ich], cchPart * sizeof(WCHAR)) == 0)
            {
                text.replace(ich, cchPart, pair.value);
                ich += lstrlenW(pair.value) - cchPart - 1;
                break;
            }
        }
    }

    // 'n'の特別扱い。
    if (bFinishN)
    {
        for (auto& ch : text)
        {
            if (ch == L'n' || ch == L'N')
                ch = L'ん';
        }
    }
    else
    {
        size_t ich = 0, len = text.size();
        for (auto& ch : text)
        {
            if (ich + 1 != len)
            {
                if (ch == L'n' || ch == L'N')
                    ch = L'ん';
            }
            ++ich;
        }
    }

    return text;
}

static inline int MZ_HexToInt(unsigned char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

// URLエンコード。
std::string MZ_UrlEncode(const char *str)
{
    char *encoded = (char *)std::malloc(strlen(str) * 3 + 1);
    assert(encoded);
    if (!encoded)
        return "";

    char *pch;
    for (pch = encoded; *str; ++str)
    {
        BYTE ch = *str;
        if (isalnum(ch) || ch == '-' || ch == '_' || ch == '.' || ch == '~')
            *pch++ = ch;
        else
            pch += sprintf(pch, "%%%02X", ch);
    }

    *pch = 0;
    std::string ret = encoded;
    std::free(encoded);
    return ret;
}

// URLデコード。
std::string MZ_UrlDecode(const char *str)
{
    char *decoded = (char*)std::malloc(strlen(str) + 1);
    assert(decoded);
    if (!decoded)
        return "";

    char *pch;
    for (pch = decoded; *str; ++str)
    {
        if (str[0] == '%' && isxdigit((BYTE)str[1]) && isxdigit((BYTE)str[2]))
        {
            int high = MZ_HexToInt(str[1]);
            int low = MZ_HexToInt(str[2]);
            assert(high != -1 && low != -1);
            *pch++ = (char)(high * 16 + low);
            str += 3 - 1;
            continue;
        }
        else if (*str == '+')
        {
            *pch++ = ' ';
        }
        else
        {
            *pch++ = *str;
        }
    }

    *pch = 0;
    std::string ret = decoded;
    std::free(decoded);
    return ret;
}
