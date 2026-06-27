#include "index.hpp"

#include <fstream>
#include <iostream>
#include <vector>
#include <openssl/sha.h>
#include <set>
#include <algorithm>
#include <unordered_map>

#include "main.hpp"
#include "object.hpp"

/* Read the index file and return an index entry */
std::vector<IndexEntry> read_index() {
    std::string data; /* Placeholder for index file contents */

    try {
        std::ifstream file(".gitpp/index", std::ios::binary); /* Read index file */

        data = std::string( /* Store contents of index file in memory */
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        );
    }
    catch (const std::ios_base::failure&) { /* Return empty IndexEntry if reading/opening index file fails */
        return {};
    }

    std::array<unsigned char, SHA_DIGEST_LENGTH> digest{}; /* Array for storing digest SHA-1 hash */

    SHA1( /* Compute hash for digest */
        reinterpret_cast<const unsigned char*>(data.data()), /* Treat file data as raw bytes */
        data.size() - 20, /* Exclude last 20 bytes of index file data */
        digest.data() /* Store result in digest variable */
    );

    if (!std::equal( /* If stored hash (within index file) and computed hash don't match, throw a runtime error */
        digest.begin(),
        digest.end(),
        reinterpret_cast<const unsigned char*>(data.data() + data.size() - SHA_DIGEST_LENGTH)
        )) {
        throw std::runtime_error("invalid index checksum");
    }

    const auto* ptr = reinterpret_cast<const unsigned char*>(data.data()); /* Treat file data as raw bytes */

    std::string signature(ptr, ptr + 4); /* Convert first 4 bytes into a string */

    uint32_t version =
    (ptr[4] << 24) | (ptr[5] << 16) | (ptr[6] << 8) | ptr[7]; /* Read big-endian format version */

    uint32_t num_entries =
        (ptr[8] << 24) | (ptr[9] << 16) | (ptr[10] << 8) | ptr[11]; /* Read number of index entries */

    /* Check for a valid signature and version, throw an error if invalid */
    if (signature != "DIRC") {
        throw std::runtime_error(std::string("invalid index signature ") + signature);
    }

    if (version != 2) {
        throw std::runtime_error("unknown index version " + std::to_string(version));
    }

    /* Extract the entry data section from index file (excluding header and checksum) */
    std::string entry_data = data.substr(12, data.size() - 12 -20);

    std::vector<IndexEntry> entries; /* Parsed index entries */
    int i = 0;

    /* Iterate over index entries */
    while (i + 62 < entry_data.length()) {
        int fields_end = i + 62; /* End of entry header */

        const auto* p = reinterpret_cast<const unsigned char*>(entry_data.data() + i); /* Pointer to start of current index entry */

        /* Index entry fields */
        uint32_t ctime_s = (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
        uint32_t ctime_n = (p[4] << 24) | (p[5] << 16) | (p[6] << 8) | p[7];

        uint32_t mtime_s = (p[8] << 24) | (p[9] << 16) | (p[10] << 8) | p[11];
        uint32_t mtime_n = (p[12] << 24) | (p[13] << 16) | (p[14] << 8) | p[15];

        uint32_t dev  = (p[16] << 24) | (p[17] << 16) | (p[18] << 8) | p[19];
        uint32_t ino  = (p[20] << 24) | (p[21] << 16) | (p[22] << 8) | p[23];
        uint32_t mode = (p[24] << 24) | (p[25] << 16) | (p[26] << 8) | p[27];
        uint32_t uid  = (p[28] << 24) | (p[29] << 16) | (p[30] << 8) | p[31];
        uint32_t gid  = (p[32] << 24) | (p[33] << 16) | (p[34] << 8) | p[35];
        uint32_t size = (p[36] << 24) | (p[37] << 16) | (p[38] << 8) | p[39];

        /* SHA-1 hash of file contents (20 bytes) */
        std::array<unsigned char, 20> sha1{};
        std::copy(p + 40, p + 60, sha1.begin());

        /* Flags (index metadata) */
        uint16_t flags = (p[60] << 8) | p[61];

        /* Find end of file path */
        size_t path_end = entry_data.find('\0', fields_end);

        /* Extract file path from entry */
        std::string path = entry_data.substr(fields_end, path_end - fields_end);

        /* Create a new IndexEntry with parsed data */
        IndexEntry entry{
            ctime_s,
            ctime_n,
            mtime_s,
            mtime_n,
            dev,
            ino,
            mode,
            uid,
            gid,
            size,
            sha1,
            flags,
            path
        };

        /* Store parsed entry */
        entries.push_back(entry);

        /* Compute total size of index entry, aligned to 8 bytes */
        size_t entry_len = ((62 + path.size() + 8) / 8) * 8;

        i += entry_len;

        /* Check if all entries were parsed, otherwise throw an error */
        if (entries.size() != num_entries) {
            throw std::runtime_error("index entry count mismatch");
        }
    }
    return entries;
}

/* Print list of files in index and optionally their metadata */
void ls_files(const bool details) {
    for (const auto& entry : read_index()) { /* Loop through every entry in the index */
        if (details) { /* Print extra metadata if "details" is true */
            int stage = (entry.flags >> 12) & 0x3; /* Extract stage from the entry's flags */
            /* Print file mode (octal), SHA-1 hash, stage and file path */
            std::cout << std::setw(6) << std::setfill('0')
          << std::oct << entry.mode
          << ' '
          << sha1_to_hex(entry.sha1.data())
          << ' '
          << std::dec << stage
          << '\t'
          << entry.path
          << '\n';
        } else { /* Otherwise print just the path of the index entry */
            std::cout << entry.path << '\n';
        }
    }
}

/* Get status of working copy and return lists of changed, new and deleted files */
Status get_status() {
    /* Create a new empty set for paths */
    std::set<std::string> paths;

    /* Loop through every directory starting from current working directory */
    for (auto& entry : std::filesystem::recursive_directory_iterator(".")) {
        /* Skip .gitpp folder's contents */
        if (entry.path().string().find(".gitpp") != std::string::npos) {
            continue;
        }
        auto path = entry.path().string(); /* Convert current file/folder path into a string */
        std::replace(path.begin(), path.end(), '\\', '/'); /* Replace '\' with '/' */

        /* If the path starts with './' then remove the prefix */
        if (path.starts_with("./")) {
            path = path.substr(2);
        }
        paths.insert(path); /* Add path to set if it doesn't exist already */
    }

    /* Lookup table for index entries using a file path */
    std::pmr::unordered_map<std::string, IndexEntry> entries_by_path;

    /* Add each index entry to a path -> entry lookup table */
    for (const auto& e : read_index()) {
        entries_by_path[e.path] = e;
    }

    /* Set of file paths extracted from the index entries */
    std::set<std::string> entry_paths;

    /* Extract all file paths from index entries into a set */
    for (const auto& [path, _] : entries_by_path) {
        entry_paths.insert(path);
    }

    /* Empty set for changed files */
    std::set<std::string> changed;

    /* Loop through all file paths in current directory */
    for (const auto& p : paths) {
        /* Only consider files that are tracked in the index */
        if (entry_paths.find(p) != entry_paths.end()) {
            /* Compare current file hash with stored index hash */
            if (hash_object(read_file(p), "blob", false) != sha1_to_hex(entries_by_path[p].sha1.data())) {
                /* Mark file as changed if file content differs from index */
                changed.insert(p);
            }
        }
    }

    /* Declare sets for new and deleted files */
    std::set<std::string> new_files;
    std::set<std::string> deleted;

    /* Insert paths that are not in the index but in the current directory (new files) */
    std::set_difference(paths.begin(), paths.end(), entry_paths.begin(), entry_paths.end(), std::inserter(new_files, new_files.begin()));

    /* Insert paths that are in the index but not in the current directory (deleted files) */
    std::set_difference(entry_paths.begin(), entry_paths.end(), paths.begin(), paths.end(), std::inserter(deleted, deleted.begin()));

    /* Return the changed, new_files and deleted sets */
    return Status{
        std::move(changed),
        std::move(new_files),
        std::move(deleted)
    };
}

/* Show status of working copy */
void status() {
    Status status_result = get_status(); /* Get status of working copy using get_status() */
    /* Save changed, new and deleted files into their own variables */
    std::set<std::string> changed = status_result.changed;
    std::set<std::string> new_files = status_result.new_files;
    std::set<std::string> deleted = status_result.deleted;

    /* Print out working copy status */
    if (!changed.empty()) { /* Changed files */
        std::cout << "changed files:\n";
        for (const auto& path : changed) { /* Loop through all paths in changed files set */
            std::cout << "   " << path << "\n";
        }
    }
    if (!new_files.empty()) {
        std::cout << "new files:\n";
        for (const auto& path : new_files) {
            std::cout << "   " << path << "\n";
        }
    }
    if (!deleted.empty()) {
        std::cout << "deleted files:\n";
        for (const auto& path : deleted) {
            std::cout << "   " << path << "\n";
        }
    }
}