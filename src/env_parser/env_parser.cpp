
/********************************************
 * Copyright (c) 2025 Shun/翔海 (@shun4midx) *
 * Project: Shun4miShu                      *
 * File Type: C++ file                      *
 * File: env_parser.cpp                     *
 ****************************************** */

// ======== INCLUDE ======== //
#include "env_parser.h"

// ======== FUNCTION IMPLEMENTATION ======== //
bool contains(const std::vector<std::string>& vec, const std::string& str) {
    return std::find(vec.begin(), vec.end(), str) != vec.end();
}

std::unordered_map<std::string, std::vector<std::string>> parseEnvFile(const std::string& path) {
    std::unordered_map<std::string, std::vector<std::string>> env;
    std::ifstream infile(path);
    std::string line;

    while (std::getline(infile, line)) {
        // Skip empty lines and lines starting with #
        if (line.empty() || line[0] == '#') {
            continue;
        }

        size_t eq_pos = line.find('=');
        if (eq_pos != std::string::npos) {
            std::string key = line.substr(0, eq_pos);
            std::string val = line.substr(eq_pos + 1);

            // Remove potential quotes
            if (!val.empty() && val.front() == '"' && val.back() == '"') {
                val = val.substr(1, val.size() - 2);
            }
            
            std::vector<std::string> parts;
            std::stringstream ss(val);
            std::string item;

            while (std::getline(ss, item, ',')) {
                parts.push_back(item);
            }

            env[key] = parts;
        }
    }

    return env;
}