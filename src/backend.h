#ifndef BACKEND_H
#define BACKEND_H

#include <array>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <iomanip>
#include <map>
#include <mutex>
#include <queue>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_set>

#include "Gaming.h"
#include "MCTS.h"

namespace GosFrontline
{

    class UUID
    {
    public:
        /// @brief Generates a random UUID
        UUID() : UUID(generateUUID()) {}

        /// @brief Constructor that Will give a UUID from a given array of 16 bytes
        /// @param uuid_bytes
        explicit UUID(const std::array<uint8_t, 16> &uuid_bytes) : uuid(uuid_bytes) {}

        /// @brief Returns the UUID as a string in the format "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
        /// @return The UUID as a string
        std::string str() const
        {
        }
        /// @return
        std::string str() const
        {
            std::stringstream ss;
            ss << std::hex << std::setfill('0');
            for (size_t i = 0; i < 16; ++i)
            {
                if (i == 4 || i == 6 || i == 8 || i == 10)
                    ss << "-";
                ss << std::setw(2) << static_cast<int>(uuid[i]);
            }
            return ss.str();
        }

    private:
        /// @brief Underlying UUID
        std::array<uint8_t, 16> uuid;

        struct UUIDHash
        {
            size_t operator()(const std::array<uint8_t, 16> &uuid) const
            {
                std::hash<uint64_t> hasher;
                uint64_t high = *reinterpret_cast<const uint64_t *>(uuid.data());
                uint64_t low = *reinterpret_cast<const uint64_t *>(uuid.data() + 8);
                return hasher(high) ^ hasher(low);
            }
        };

        /// @brief UUID generator. Checks against redundancy.
        static std::array<uint8_t, 16> generateUUID()
        {
            static std::random_device rd;
            static std::mt19937_64 gen(rd());
            static std::uniform_int_distribution<uint8_t> dis(0, 255);
            static std::mutex mtx;
            static std::unordered_set<std::array<uint8_t, 16>, UUIDHash> used_uuids;

            std::lock_guard<std::mutex> lock(mtx); // Ensure thread safety

            std::array<uint8_t, 16> uuid;
            do
            {
                for (auto &byte : uuid)
                {
                    byte = dis(gen);
                }
                // Set the version (4 - random) and variant (2 - RFC 4122)
                uuid[6] = (uuid[6] & 0x0F) | 0x40; // Version 4
                uuid[8] = (uuid[8] & 0x3F) | 0x80; // Variant 2
            } while (!used_uuids.insert(uuid).second);

            return uuid;
        }
    };

    class Request
    {
        UUID uuid;
        std::string action;
        
    };

} // namespace GosFrontline

#endif // BACKEND_H
