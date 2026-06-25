#pragma once
#include <filesystem>
#include <string>

std::string hash_object(const std::string& data, const std::string& obj_type, bool write = true);
std::string sha1_to_hex(const unsigned char* hash);
std::filesystem::path find_object(const std::string& sha1_prefix);