#include "object.hpp"

#include <filesystem>
#include <iomanip>
#include <vector>
#include <sstream>
#include <openssl/sha.h>
#include <fstream>
#include <zlib.h>

/* Convert a SHA1 hash into hexadecimal */
std::string sha1_to_hex(const unsigned char* hash) {
    std::stringstream ss;
    for (int i = 0; i < 20; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

/* Calculate a SHA1 cryptographic hash for an object */
std::string hash_object(const std::string& data, const std::string& obj_type, bool write) {
    /* Create header string */
    std::string header_str = obj_type + " " + std::to_string(data.size());

    /* Convert to byte array */
    std::vector<uint8_t> header_bytes(header_str.begin(), header_str.end());

    /* Combine the header with provided data */
    std::vector<uint8_t> full_data = header_bytes;
    full_data.push_back(0x00); /* Null byte */
    full_data.insert(full_data.end(), data.begin(), data.end());

    /* Calculate SHA1 hash for data */
    unsigned char full_data_hash[SHA_DIGEST_LENGTH];
    SHA1(full_data.data(), full_data.size(), full_data_hash);

    /* Convert hash to hexadecimal */
    std::string sha1_hex = sha1_to_hex(full_data_hash);

    if (write) {
        std::filesystem::path path = std::filesystem::path(".gitpp") / "objects"
            / sha1_hex.substr(0, 2) / sha1_hex.substr(2); /* Directory structure for objects */
        if (!std::filesystem::exists(path)) { /* Create directories for the object if they don't exist */
            std::filesystem::create_directories(path.parent_path());

            /* Compress data using zlib */
            uLongf compressed_size = compressBound(full_data.size());
            std::vector<uint8_t> compressed_data(compressed_size);

            int result = compress(
                compressed_data.data(),
                &compressed_size,
                full_data.data(),
                full_data.size()
            );

            /* If compression is successful, write the compressed data to the object file itself */
            if (result == Z_OK) {
                std::ofstream(path, std::ios::binary).write(reinterpret_cast<const char*>(compressed_data.data()), compressed_size);
            }
        }
    }
    return sha1_hex;
}

/* Find an object using a partial SHA1 hash prefix */
std::filesystem::path find_object(const std::string &sha1_prefix) {
    if (sha1_prefix.length() < 2) { /* Throw error if prefix is too short */
        throw std::invalid_argument("hash prefix must be 2 or more characters");
    }
    std::filesystem::path obj_dir = std::filesystem::path(".gitpp") / "objects" / sha1_prefix.substr(0, 2); /* Directory structure for objects */
    std::string rest = sha1_prefix.substr(2); /* Store the rest of the hash */

    std::vector<std::string> objects; /* Vector for found filenames */

    /* Loop through every file in the target directory */
    for (const auto& entry : std::filesystem::directory_iterator(obj_dir)) {
        /* Extract plain filename */
        std::string name = entry.path().filename().string();

        /* If filename starts with hash prefix, add it to objects vector */
        if (name.rfind(rest, 0) == 0) {
            objects.push_back(name);
        }
    }

        /* Throw error if objects vector is empty */
        if (objects.empty()) {
            throw std::invalid_argument("object '" + sha1_prefix + "' not found");
        }

        /* Throw error if there are multiple objects with matching prefixes */
        if (objects.size() >= 2) {
            throw std::runtime_error("multiple objects (" + std::to_string(objects.size()) + ") with prefix '" + sha1_prefix + "'");
        }
        return obj_dir / objects[0];
    }