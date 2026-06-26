#pragma once
#include <filesystem>
#include <string>
#include <vector>

std::string hash_object(const std::string& data, const std::string& obj_type, bool write = true);
std::string sha1_to_hex(const unsigned char* hash);
std::filesystem::path find_object(const std::string& sha1_prefix);
std::pair<std::string, std::vector<uint8_t>> read_object(const std::string& sha1_prefix);
void cat_file(const std::string& mode, const std::string& sha1_prefix);