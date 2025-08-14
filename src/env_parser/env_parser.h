/********************************************
 * Copyright (c) 2025 Shun/翔海 (@shun4midx) *
 * Project: Shun4miShu                      *
 * File Type: C++ Header file               *
 * File: env_parser.h                       *
 ****************************************** */

// ======== INCLUDE ======== //
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <filesystem>

// ======== FUNCTION PROTOTYPES ======== //
bool contains(const std::vector<std::string>& vec, const std::string& str);
std::unordered_map<std::string, std::vector<std::string>> parseEnvFile(const std::string& path = std::filesystem::path(__FILE__).parent_path().parent_path().parent_path() / ".env");