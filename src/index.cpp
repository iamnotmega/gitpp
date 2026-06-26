#include "index.hpp"

#include <fstream>
#include <iostream>
#include <vector>
#include <openssl/sha.h>

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
