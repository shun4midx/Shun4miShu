/********************************************
 * Copyright (c) 2025 Shun/翔海 (@shun4midx) *
 * Project: Shun4miShu                      *
 * File Type: C++ Header file               *
 * File: typist.h                           *
 ****************************************** */

#pragma once

// ======== INCLUDE ======== //
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <vector>
#include <filesystem>

#include "../converter/converter.h"

// ======= STATIC VARIABLES ======== //
static bool init_maps = false;
static std::unordered_map<std::string, std::vector<std::string>> zhuyin_to_hanzi;
static std::unordered_map<std::string, std::vector<std::string>> hanzi_to_zhuyin;

// static std::unordered_map<char, std::vector<std::string>> ZHUYIN_KEYBOARD_SYMBOLS = {
//     {'<', {"，", "、", "《", "〈", "＜"}},
//     {'>', {"。", "⋯⋯", "・", "》", "〉", "＞"}},
//     {'?', {"？", "/", "／", "÷"}},
//     {':', {"：", "；"}},
//     {'[', {"「", "『", "【", "〖", "〔", "［", "｛"}},
//     {']', {"」", "』", "】", "〗", "〕", "］", "｝"}},
//     {'!', {"！"}},
//     {'{', {"『", "「", "【", "〖", "〔", "［", "｛"}},
//     {'}', {"』", "」", "】", "〗", "〕", "］", "｝"}}
// };

// ======== FUNCTION PROTOTYPES ======= //
void initMaps();

std::vector<std::string> zhuyinToHanzi(const std::string& z);
std::vector<std::string> hanziToZhuyin(const std::string& h);