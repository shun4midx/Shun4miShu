/********************************************
 * Copyright (c) 2025 Shun/翔海 (@shun4midx) *
 * Project: Shun4miShu                      *
 * File Type: C++ file                      *
 * File: typist.cpp                         *
 ****************************************** */

// ======= INCLUDE ======== //
#include "typist.h"

// ======= TESTING MAIN ======== //
void initMaps() {
    if (init_maps) {
        return;
    }
    
    std::ifstream in(std::filesystem::path(__FILE__).parent_path().parent_path() / "tsi.json");

    std::string json((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();

    int i = 0;
    while (true) {
        // Find key
        size_t key_start = json.find('"', i);
        if (key_start == std::string::npos) {
            break;
        }

        size_t key_end = json.find('"', key_start + 1);
        if (key_end == std::string::npos) {
            break;
        }

        std::string key = json.substr(key_start + 1, key_end - key_start - 1);

        // Find opening bracket of array
        size_t arr_start = json.find('[', key_end);
        if (arr_start == std::string::npos) {
            break;
        }

        size_t arr_end = json.find(']', arr_start);
        if (arr_end == std::string::npos) {
            break;
        }

        std::vector<std::string> values;
        size_t pos = arr_start + 1;
        
        while (true) {
            size_t val_start = json.find('"', pos);
            if (val_start == std::string::npos || val_start > arr_end) {
                break;
            }

            size_t val_end = json.find('"', val_start + 1);
            if (val_end == std::string::npos || val_end > arr_end) {
                break;
            }

            std::string val = json.substr(val_start + 1, val_end - val_start - 1);
            values.push_back(val);
            hanzi_to_zhuyin[val].push_back(key);
            pos = val_end + 1;
        }

        zhuyin_to_hanzi[key] = values;
        i = arr_end + 1;
    }
    init_maps = true;
} 

std::vector<std::string> zhuyinToHanzi(const std::string& z) {
    initMaps();

    std::vector<std::string> split_vec = split_utf8(z);

    if (split_vec.size() == 0) {
        return {};
    }

    if (split_vec[split_vec.size() - 1] == " ") {
        split_vec.pop_back();
        return zhuyin_to_hanzi[merge(split_vec, "")];
    } else {
        if (zhuyin_to_hanzi.find(z) != zhuyin_to_hanzi.end()) {
            return zhuyin_to_hanzi[z];
        } else {
            return {};
        }
    }
}

std::vector<std::string> hanziToZhuyin(const std::string& h) {
    initMaps();
    if (hanzi_to_zhuyin.find(h) != hanzi_to_zhuyin.end()) {
        return hanzi_to_zhuyin[h];
    } else {
        return {};
    }
}