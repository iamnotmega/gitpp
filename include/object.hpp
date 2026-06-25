#pragma once
#include <string>

std::string hash_object(const std::string& data, const std::string& obj_type, bool write = true);
std::string sha1_to_hex(const unsigned char* hash);