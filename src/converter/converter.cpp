/********************************************
 * Copyright (c) 2025 Shun/翔海 (@shun4midx) *
 * Project: Shun4miShu                      *
 * File Type: C++ file                      *
 * File: converter.cpp                      *
 ****************************************** */

// ======== INCLUDE ======== //
#include "converter.h"

// ======== PARSING UTILS ======== //
std::vector<std::string> split_utf8(const std::string& str) {
    std::vector<std::string> result;
    for (size_t i = 0; i < str.size();) {
        unsigned char c = str[i];
        size_t len =
            (c & 0x80) == 0x00 ? 1 :
            (c & 0xE0) == 0xC0 ? 2 :
            (c & 0xF0) == 0xE0 ? 3 :
            (c & 0xF8) == 0xF0 ? 4 : 1;
        result.push_back(str.substr(i, len));
        i += len;
    }
    return result;
}

std::vector<std::string> split_zhuyin(const std::string& str) {
    static const std::unordered_set<std::string> DELIMS = {" ", u8"ˊ", u8"ˇ", u8"ˋ", u8"˙", "，", "。", "？", "：", "「", "」", "『", "』"};

    std::vector<std::string> out;
    std::string cur;

    for (size_t i = 0; i < str.size();) {
        unsigned char c = static_cast<unsigned char>(str[i]);
        size_t len =
            (c & 0x80) == 0x00 ? 1 :
            (c & 0xE0) == 0xC0 ? 2 :
            (c & 0xF0) == 0xE0 ? 3 :
            (c & 0xF8) == 0xF0 ? 4 : 1;

        std::string cp = str.substr(i, len);
    
        if (cp == " " || TONES.count(cp)) {
            cur += cp;
            out.push_back(std::move(cur));
            cur.clear();
        } else {
            cur += cp;
        }
        i += len;
    }

    if (!cur.empty()) {
        out.push_back(std::move(cur));
    }

    return out;
}

std::vector<std::string> split_alphanumeric(const std::string& str) {
    std::vector<std::string> words;
    std::stringstream ss(str);
    std::string word;

    while(getline(ss, word, ' ')){
        words.push_back(word);
    }

    return words;
}

std::vector<std::string> split_zhuyin_qwerty(const std::string& str) {
    std::unordered_set<char> delims = {' ', '3', '4', '6', '7'};
    std::vector<std::string> result;

    std::string current;
    for (char c : str) {
        current.push_back(c);
        
        if (delims.count(c)) {
            result.push_back(current);
            current.clear();
        }
    }

    if (!current.empty()) {
        result.push_back(current);
    }

    return result;
}

std::string merge(const std::vector<std::string> vec, const std::string& str) {
    std::string output = "";

    for (int i = 0; i < vec.size(); ++i) {
        output += vec[i];

        if (i + 1 < vec.size()) {
            output += str;
        }
    }

    return output;
}

// ======== CONVERTERS ========= //
std::pair<int, int> getZhuyinPos(const std::string& zhuyin) {
    for (int i = 0; i < 4; ++i) {
        std::vector<std::string> parsed_zhuyin = split_utf8(ZHUYIN_KEYBOARD[i]);

        for (int pos = 0; pos < parsed_zhuyin.size(); ++pos) {
            if (parsed_zhuyin[pos] == zhuyin) {
                return {i, pos};
            }
        }
    }

    return std::pair{-1, -1};
}

std::pair<int, int> getPinyinPos(const char& pinyin) {
    for (int i = 0; i < 4; ++i) {
        size_t pos = QWERTY_KEYBOARD[i].find(pinyin);
        if (pos != std::string::npos) {
            return {i, pos};
        }
    }

    return std::pair{-1, -1};
}

std::vector<std::string> qwerty_to_zhuyin(const std::vector<std::string>& parsed) {
    std::vector<std::string> output;

    for (int i = 0; i < parsed.size(); ++i) {
        std::string zhuyin = "";

        for (char c : parsed[i]) {
            std::pair<int, int> pos = getPinyinPos(c);

            if (pos.first >= 0 && pos.second >= 0) {
                std::vector<std::string> zhuyin_row = split_utf8(ZHUYIN_KEYBOARD[pos.first]);
                zhuyin += zhuyin_row[pos.second];
            } else {
                zhuyin += SYMBOL_ZHUYIN[c];
            }
        }

        output.push_back(zhuyin);
    }

    return output;
}

std::vector<std::string> zhuyin_to_qwerty(const std::vector<std::string>& parsed) {
    std::vector<std::string> output;

    for (int i = 0; i < parsed.size(); ++i) {
        std::string qwerty = "";

        std::vector<std::string> parsed_zhuyin = split_utf8(parsed[i]);

        for (std::string& zhuyin : parsed_zhuyin) {
            std::pair<int, int> pos = getZhuyinPos(zhuyin);

            if (pos.first >= 0 && pos.second >= 0) {
                qwerty += QWERTY_KEYBOARD[pos.first][pos.second];
            } else {
                if (zhuyin == " ") {
                    qwerty += ' ';
                } else if (ZHUYIN_SYMBOLS.find(zhuyin) != ZHUYIN_SYMBOLS.end()) {
                    qwerty += ZHUYIN_SYMBOLS[zhuyin];
                }
            }
        }

        output.push_back(qwerty);
    }

    return output;
}

std::vector<std::string> pinyin_to_zhuyin(const std::vector<std::string>& parsed) {
    // Build reverse lookup once
    static std::unordered_map<std::string, std::string> PINYIN_TO_ZHUYIN_INITIALS;
    static std::unordered_map<std::string, std::string> PINYIN_TO_ZHUYIN_FINALS;
    static std::unordered_map<std::string, std::string> SPECIAL_PINYIN_TO_ZHUYIN;
    static std::unordered_map<int, std::string> TONE_SYMBOLS = {
        {1, " "}, {2, u8"ˊ"}, {3, u8"ˇ"}, {4, u8"ˋ"}, {0, u8"˙"}
    };

    if (PINYIN_TO_ZHUYIN_INITIALS.empty()) {
        for (auto &kv : ZHUYIN_BEGINNING_CONSONANTS) {
            PINYIN_TO_ZHUYIN_INITIALS[kv.second] = kv.first;
        }

        for (auto &kv : ZHUYIN_BEGINNING_VOWELS) {
            PINYIN_TO_ZHUYIN_FINALS[kv.second] = kv.first;
        }
        
        for (auto &kv : ZHUYIN_VOWELS) {
            PINYIN_TO_ZHUYIN_FINALS[kv.second] = kv.first;
        }
        
        for (auto &kv : ZHUYIN_ENDING_CONSONANTS) {
            PINYIN_TO_ZHUYIN_FINALS[kv.second] = kv.first;
        }

        for (auto &kv : ZHUYIN_ONLY_CONSONANTS) {
            SPECIAL_PINYIN_TO_ZHUYIN[kv.second] = kv.first;
        }

        PINYIN_TO_ZHUYIN_FINALS["v"] = "ㄩ";
        PINYIN_TO_ZHUYIN_FINALS["vn"] = u8"ㄩㄣ";
        PINYIN_TO_ZHUYIN_FINALS["ve"] = u8"ㄩㄝ";
        PINYIN_TO_ZHUYIN_FINALS["van"] = u8"ㄩㄢ";
    }

    std::vector<std::string> output;

    for (int i = 0; i < parsed.size(); ++i) {
        std::string syl = parsed[i];

        if (syl.empty()) {
            continue;
        }

        // Extract tone number
        int tone = 0;
        if (isdigit(syl.back())) {
            tone = syl.back() - '0';
            syl.pop_back();
        }

        // Lowercase
        std::string lower;
        for (char c : syl) lower += std::tolower(static_cast<unsigned char>(c));

        std::string zhuyin = "";

        // Full consonant matching
        auto it_sp = SPECIAL_PINYIN_TO_ZHUYIN.find(lower);
        if (it_sp != SPECIAL_PINYIN_TO_ZHUYIN.end()) {
            output.push_back(it_sp->second + TONE_SYMBOLS[tone]);
            continue;
        }

        auto it_full = PINYIN_TO_ZHUYIN_FINALS.find(lower);
        if (it_full != PINYIN_TO_ZHUYIN_FINALS.end()) { 
            output.push_back(it_full->second + TONE_SYMBOLS[tone]); 
            continue; 
        }

        // Try longest matching initial
        std::string matched_init_py = "";
        std::string matched_init_zy = "";
        for (int len = std::min((int)lower.size(), 2); len >= 1; --len) {
            std::string try_init = lower.substr(0, len);
            auto it = PINYIN_TO_ZHUYIN_INITIALS.find(try_init);
            if (it != PINYIN_TO_ZHUYIN_INITIALS.end()) {
                matched_init_py = try_init;
                matched_init_zy = it->second;
                lower.erase(0, len);
                break;
            }
        }
        zhuyin += matched_init_zy;

        auto is_jqxy = [](const std::string& s){
            return s == "j" || s == "q" || s == "x" || s == "y";
        };
    
        std::string matched_final = "";
    
        if (is_jqxy(matched_init_py) && !lower.empty()) {
            // Special: j/q/x + iong => ㄩㄥ
            if (lower.rfind("uan", 0) == 0) {
                lower.replace(0, 3, "van");
            } else if (lower.rfind("ue",  0) == 0) {
                lower.replace(0, 2, "ve");
            } else if (lower.rfind("un",  0) == 0) {
                lower.replace(0, 2, "vn");
            } else if (lower[0] == 'u') {
                lower[0] = 'v';
            }
        }

        // Try longest matching final
        for (int len = std::min((int)lower.size(), 4); len >= 1; --len) {
            std::string try_final = lower.substr(0, len);
            if (PINYIN_TO_ZHUYIN_FINALS.count(try_final)) {
                matched_final = PINYIN_TO_ZHUYIN_FINALS[try_final];
                lower.erase(0, len);
                break;
            }
        }
        zhuyin += matched_final;

        // Other symbols
        if (matched_init_zy.empty() && matched_final.empty()) {
            output.push_back(parsed[i]);
            continue;
        }

        // Add tone mark
        zhuyin += TONE_SYMBOLS[tone];

        output.push_back(zhuyin);
    }

    return output;
}

std::vector<std::string> zhuyin_to_pinyin(const std::vector<std::string>& parsed) {
    std::vector<std::string> output;

    for (int i = 0; i < parsed.size(); ++i) {
        std::vector<std::string> parsed_zhuyin = split_utf8(parsed[i]);

        // If it's a single symbol or not found in any Zhuyin maps, output directly
        if (parsed_zhuyin.empty() || 
           (!ZHUYIN_BEGINNING_CONSONANTS.count(parsed_zhuyin[0]) &&
            !ZHUYIN_BEGINNING_VOWELS.count(parsed_zhuyin[0]) &&
            !ZHUYIN_VOWELS.count(parsed_zhuyin[0]) &&
            !ZHUYIN_ENDING_CONSONANTS.count(parsed_zhuyin[0]) &&
            !TONES.count(parsed_zhuyin[0]))) {
            output.push_back(parsed[i]);
            continue;
        }

        int tone = 1;

        if (TONES.find(parsed_zhuyin[parsed_zhuyin.size() - 1]) != TONES.end()) {
            tone = TONES[parsed_zhuyin[parsed_zhuyin.size() - 1]];
            parsed_zhuyin.pop_back(); // Remove tone
        }

        // Convert main
        std::string pinyin = "";

        if (parsed_zhuyin.size() == 1) { // Single vowel or "er"
            if (ZHUYIN_ONLY_CONSONANTS.find(parsed_zhuyin[0]) != ZHUYIN_ONLY_CONSONANTS.end()) {
                pinyin = ZHUYIN_ONLY_CONSONANTS[parsed_zhuyin[0]];
            } else {
                pinyin += ZHUYIN_BEGINNING_VOWELS[parsed_zhuyin[0]];
            }
        } else if (parsed_zhuyin.size() == 2) { // CV or V or VC
            if (ZHUYIN_BEGINNING_CONSONANTS.find(parsed_zhuyin[0]) != ZHUYIN_BEGINNING_CONSONANTS.end()) { // CV
                pinyin += ZHUYIN_BEGINNING_CONSONANTS[parsed_zhuyin[0]];
                pinyin += ZHUYIN_VOWELS[parsed_zhuyin[1]];
            } else if (ZHUYIN_ENDING_CONSONANTS.find(parsed_zhuyin[1]) != ZHUYIN_ENDING_CONSONANTS.end() && parsed_zhuyin[0] != "ㄨ") { // VC
                pinyin += ZHUYIN_BEGINNING_VOWELS[parsed_zhuyin[0]];
                pinyin += ZHUYIN_ENDING_CONSONANTS[parsed_zhuyin[1]];
            } else { // V
                pinyin = ZHUYIN_BEGINNING_VOWELS[parsed_zhuyin[0] + parsed_zhuyin[1]];
            }
        } else if (parsed_zhuyin.size() == 3) { // CV or CVC
            if (ZHUYIN_ENDING_CONSONANTS.find(parsed_zhuyin[2]) != ZHUYIN_ENDING_CONSONANTS.end()) { // CVC
                pinyin += ZHUYIN_BEGINNING_CONSONANTS[parsed_zhuyin[0]];

                if (parsed_zhuyin[1] == "ㄩ" && parsed_zhuyin[2] == "ㄣ") { // vn exception
                    pinyin += "un";
                } else {
                    pinyin += ZHUYIN_VOWELS[parsed_zhuyin[1]];
                    pinyin += ZHUYIN_ENDING_CONSONANTS[parsed_zhuyin[2]];
                }
            } else { // CV
                pinyin += ZHUYIN_BEGINNING_CONSONANTS[parsed_zhuyin[0]];
                pinyin += ZHUYIN_VOWELS[parsed_zhuyin[1] + parsed_zhuyin[2]];
            }
        }

        // Add tone
        if (tone != 0 && pinyin != "") {
            pinyin += std::to_string(tone);
        }

        // Push
        output.push_back(pinyin);
    }

    return output;
}