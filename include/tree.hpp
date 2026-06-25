#pragma once
#include <cstdint>
#include <string>
#include <vector>

std::vector<std::vector<std::string>> read_tree(const std::string& sha1 = "", std::vector<uint8_t> data = {});
