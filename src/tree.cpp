#include "tree.hpp"

#include <cassert>
#include <algorithm>
#include <iterator>

#include "object.hpp"

/* Parses an object into a list of file entries */
std::vector<std::vector<std::string>> read_tree(const std::string& sha1, std::vector<uint8_t> data) {
    /* If hash is not empty, read the object and store its type and raw data */
    if (!sha1.empty()) {
        std::string obj_type;
        std::tie(obj_type, data) = read_object(sha1);
        assert(obj_type == "tree");
    } else if (data.empty()) { /* Throw an error if data is empty/missing */
        throw std::invalid_argument("must specify 'sha1' or 'data'");
    }

    /* Variables for tree parsing state and stored entries */
    size_t i = 0;
    std::vector<std::vector<std::string>> entries;

    /* Parses up to 1000 file entries to prevent infinite loops */
    for (int iter = 0; iter < 1000; ++iter) {
        auto it = std::find(data.begin() + i, data.end(), '\x00'); /* Find next null byte starting from position i */

        /* If no null byte is found, the end of the file entries has been reached */
        if (it == data.end()) {
            break;
        }

        /* Calculate null byte index position */
        size_t end = std::distance(data.begin(), it);

        std::string metadata(data.begin() + i, data.begin() + end); /* Convert raw bytes into a string */
        size_t space_pos = metadata.find(' '); /* Find dividing line between the mode and the path */

        /* Extract file mode and path from metadata variable */
        std::string mode_str = metadata.substr(0, space_pos);
        std::string path = metadata.substr(space_pos + 1);

        /* Convert mode into an integer */
        int mode = std::stoi(mode_str, nullptr, 8);

        /* Extract the 20-byte SHA-1 hash following the null byte */
        std::vector<uint8_t> digest(data.begin() + end + 1, data.begin() + end + 21);

        /* Convert the hash into a hexadecimal string and store it in the entries vector */
        std::string sha1_hex = sha1_to_hex(digest.data());
        entries.push_back({std::to_string(mode), path, sha1_hex});

        /* Move index past null byte and file hash */
        i = end + 1 + 20;
    }
    return entries;
}
