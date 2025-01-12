#ifndef UTILITIES_H
#define UTILITIES_H

#include <regex>
#include <cctype>

namespace GosFrontline
{
    std::string sanitize_filename(const std::string &filename)
    {
        std::regex invalid_chars_regex(R"([\x00-\x1F/\\:*?"<>|])");
        std::string sanitized = std::regex_replace(filename, invalid_chars_regex, "_");

        for (auto &c : sanitized)
        {
            if (std::isspace(static_cast<unsigned char>(c)))
            {
                c = '_';
            }
        }
        return sanitized;
    }
} // namespace GosFrontline

#endif