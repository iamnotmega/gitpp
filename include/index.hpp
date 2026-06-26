#pragma once

#include <array>
#include <string>
#include <cstdint>
#include <vector>

/* Represents a tracked file stored in the index */
struct IndexEntry {
    uint32_t ctime_s{}; /* Change time (seconds since the Unix epoch) */
    uint32_t ctime_n{}; /* Change time (nanoseconds) */
    uint32_t mtime_s{}; /* Last modification time (seconds since the Unix epoch) */
    uint32_t mtime_n{}; /* Last modification time (nanoseconds) */
    uint32_t dev{}; /* ID of the storage device storing the file */
    uint32_t ino{}; /* Unique ID of a file on a filesystem */
    uint32_t mode{}; /* File mode */
    uint32_t uid{}; /* User ID of file owner */
    uint32_t gid{}; /* Group ID of file owner */
    uint32_t size{}; /* File size in bytes */
    std::array<unsigned char, 20> sha1{}; /* SHA-1 hash of the file's contents */
    uint16_t flags{}; /* Index metadata flags */
    std::string path{}; /* File path relative to the repository root */
};

std::vector<IndexEntry> read_index();