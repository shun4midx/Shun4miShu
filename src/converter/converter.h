/********************************************
 * Copyright (c) 2025 Shun/翔海 (@shun4midx) *
 * Project: Shun4miShu                      *
 * File Type: C++ Header file               *
 * File: converter.h                        *
 ****************************************** */

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <sstream>
#include <vector>

// ======== KEYBOARDS ======== //
static std::string ZHUYIN_KEYBOARD[4] = {
    "ㄅㄉˇˋㄓˊ˙ㄚㄞㄢㄦ",
    "ㄆㄊㄍㄐㄔㄗㄧㄛㄟㄣ",
    "ㄇㄋㄎㄑㄕㄘㄨㄜㄠㄤ",
    "ㄈㄌㄏㄒㄖㄙㄩㄝㄡㄥ"
};

static std::unordered_map<std::string, char> ZHUYIN_SYMBOLS = {
    {"，", '<'},
    {"。", '>'},
    {"？", '?'},
    {"：", ':'},
    {"「", '['},
    {"」", ']'},
    {"！", '!'},
    {"『", '{'},
    {"』", '}'}
};

static std::unordered_map<char, std::string> SYMBOL_ZHUYIN = {
    {'<', "，"},
    {'>', "。"},
    {'?', "？"},
    {':', "："},
    {'[', "「"},
    {']', "」"},
    {'!', "！"},
    {'{', "『"},
    {'}', "』"}
};

static std::string QWERTY_KEYBOARD[4] = {
    "1234567890-",
    "qwertyuiop",
    "asdfghjkl;",
    "zxcvbnm,./"
};

static std::unordered_map<std::string, char> PINYIN_SYMBOLS = {
    {"，", ','},
    {"。", '.'},
    {"？", '?'},
    {"：", ':'},
    {"「", '['},
    {"」", ']'},
    {"！", '!'},
    {"『", '{'},
    {"』", '}'}
};

static std::unordered_map<std::string, char> CHINESE_SYMBOLS = {
    {"，", ','},
    {"。", '.'},
    {"？", '?'},
    {"：", ':'},
    {"「", '\''},
    {"」", '\''},
    {"！", '!'},
    {"『", '"'},
    {"』", '"'}
};

// ======== INPUTS ======== //
static std::unordered_map<std::string, int> TONES = {
    {"ˇ", 3}, {"ˋ", 4}, {"ˊ", 2}, {"˙", 0}
};

static std::unordered_map<std::string, std::string> ZHUYIN_BEGINNING_CONSONANTS = {
    {"ㄅ", "b"}, {"ㄆ", "p"}, {"ㄇ", "m"}, {"ㄈ", "f"}, {"ㄉ", "d"}, {"ㄊ", "t"}, {"ㄋ", "n"}, {"ㄌ", "l"}, {"ㄍ", "g"}, {"ㄎ", "k"},
    {"ㄏ", "h"}, {"ㄐ", "j"}, {"ㄑ", "q"}, {"ㄒ", "x"}, {"ㄓ", "zh"}, {"ㄔ", "ch"}, {"ㄕ", "sh"}, {"ㄖ", "r"}, {"ㄗ", "z"}, {"ㄘ", "c"},
    {"ㄙ", "s"}
};

static std::unordered_map<std::string, std::string> ZHUYIN_ONLY_CONSONANTS = {
    {"ㄓ", "zhi"}, {"ㄔ", "chi"}, {"ㄕ", "shi"}, {"ㄖ", "ri"}, {"ㄗ", "zi"}, {"ㄘ", "ci"}, {"ㄙ", "si"}, {"ㄣ", "en"}, {"ㄦ", "er"}
};

static std::unordered_map<std::string, std::string> ZHUYIN_ENDING_CONSONANTS = {
    {"ㄣ", "n"}, {"ㄦ", "r"}
};

static std::unordered_map<std::string, std::string> ZHUYIN_VOWELS = {
    // Singular
    {"ㄧ", "i"}, {"ㄨ", "u"}, {"ㄩ", "v"}, {"ㄚ", "a"}, {"ㄛ", "o"}, {"ㄜ", "e"}, {"ㄞ", "ai"}, {"ㄟ", "ei"}, {"ㄠ", "ao"},
    {"ㄡ", "ou"}, {"ㄢ", "an"}, {"ㄤ", "ang"}, {"ㄥ", "eng"},
    
    // Combinations
    {"ㄨㄥ", "ong"}, {"ㄧㄚ", "ia"}, {"ㄧㄠ", "iao"}, {"ㄧㄝ", "ie"}, {"ㄧㄡ", "iu"}, {"ㄧㄢ", "ian"}, {"ㄧㄣ", "in"}, {"ㄧㄤ", "iang"}, {"ㄧㄥ", "ing"}, {"ㄩㄥ", "iong"}, 
    {"ㄨㄚ", "ua"}, {"ㄨㄛ", "uo"}, {"ㄨㄞ", "uai"}, {"ㄨㄟ", "ui"}, {"ㄨㄢ", "uan"}, {"ㄨㄣ", "un"}, {"ㄨㄤ", "uang"}, {"ㄩㄝ", "ue"}, {"ㄩㄢ", "uan"}
};

static std::unordered_map<std::string, std::string> ZHUYIN_BEGINNING_VOWELS = {
    {"ㄧ", "yi"}, {"ㄨ", "wu"}, {"ㄩ", "yu"}, {"ㄚ", "a"}, {"ㄛ", "o"}, {"ㄜ", "e"}, {"ㄞ", "ai"}, {"ㄟ", "ei"}, {"ㄠ", "ao"}, {"ㄡ", "ou"}, {"ㄢ", "an"}, {"ㄤ", "ang"}, {"ㄥ", "eng"},
    {"ㄨㄥ", "weng"}, {"ㄧㄚ", "ya"}, {"ㄧㄠ", "yao"}, {"ㄧㄝ", "ye"}, {"ㄧㄡ", "you"}, {"ㄧㄢ", "yan"}, {"ㄧㄣ", "yin"}, {"ㄧㄤ", "yang"}, {"ㄧㄥ", "ying"}, {"ㄩㄥ", "yong"}, 
    {"ㄨㄚ", "wa"}, {"ㄨㄛ", "wo"}, {"ㄨㄞ", "wai"}, {"ㄨㄟ", "wei"}, {"ㄨㄢ", "wan"}, {"ㄨㄣ", "wen"}, {"ㄨㄤ", "wang"}, {"ㄩㄝ", "yue"}, {"ㄩㄢ", "yuan"}
};

// ======== PARSING UTILS ======== //
std::vector<std::string> split_utf8(const std::string& str);
std::vector<std::string> split_zhuyin(const std::string& str);
std::vector<std::string> split_alphanumeric(const std::string& str);
std::string merge(const std::vector<std::string> vec, const std::string& str);

// ======== CONVERTERS ======== //
std::pair<int, int> getZhuyinPos(const std::string& zhuyin);
std::pair<int, int> getPinyinPos(const char& pinyin);

std::vector<std::string> qwerty_to_zhuyin(const std::vector<std::string>& parsed);
std::vector<std::string> zhuyin_to_qwerty(const std::vector<std::string>& parsed);

std::vector<std::string> pinyin_to_zhuyin(const std::vector<std::string>& parsed);
std::vector<std::string> zhuyin_to_pinyin(const std::vector<std::string>& parsed);