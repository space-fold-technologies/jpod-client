#ifndef __JC_CORE_COMMANDS_FORMATTER__
#define __JC_CORE_COMMANDS_FORMATTER__

#include <chrono>
#include <iostream>
#include <map>
#include <fmt/format.h>

using namespace std::chrono;
using chrono_days = std::chrono::duration<int, std::ratio_multiply<std::ratio<24>, std::chrono::hours::period>>;

namespace core::commands
{
    constexpr std::size_t kilo_bytes = 1024;
    constexpr std::size_t mega_bytes = kilo_bytes * 1024;
    constexpr std::size_t giga_bytes = mega_bytes * 1024;

    inline std::string format_time_ago(const time_point<system_clock, nanoseconds> &tp)
    {
        auto now = system_clock::now();
        auto duration = now - tp;

        // Extract days, months, and years
        auto days = duration_cast<chrono_days>(duration).count();
        auto months = days / 30;  // Approximate months
        auto years = months / 12; // Approximate years

        // Format the result
        std::string result;
        if (years > 0)
            result += fmt::format("{} years ", years);
        else if (months > 0)
            result += fmt::format("{} months ", months);
        else if (days >= 1)
            result += fmt::format("{} days ", days);
        else
        {
            auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
            auto minutes = seconds / 60;
            auto hours = minutes / 60;
            if (hours > 0)
            {
                result += fmt::format("{} hours ", hours);
            }
            else if (minutes > 0)
            {
                result += fmt::format("{} minutes ", minutes);
            }
            else if (seconds >= 1)
            {
                result += fmt::format("{} seconds ", seconds);
            }
        }
        result += "ago";
        return result;
    }

    inline std::string format_file_size(std::size_t bytes)
    {

        if (bytes <= 0)
        {
            return "-";
        }
        if (bytes < kilo_bytes)
        {
            return fmt::format("{} bytes", bytes);
        }
        else if (bytes < mega_bytes)
        {
            double kb = static_cast<double>(bytes) / kilo_bytes;
            return fmt::format("{:.1f} KB", kb);
        }
        else if (bytes < giga_bytes)
        {
            double mb = static_cast<double>(bytes) / mega_bytes;
            return fmt::format("{:.1f} MB", mb);
        }
        else
        {
            double gb = static_cast<double>(bytes) / giga_bytes;
            return fmt::format("{:.1f} GB", gb);
        }
    }

    inline std::string format_port_mapping(const std::map<std::string, std::string> &port_map)
    {
        std::string mappings;
        for (const auto &entry : port_map)
        {
            mappings.append(fmt::format("{}:{}\n", entry.first, entry.second));
        }
        return mappings;
    }
}

#endif //__JC_CORE_COMMANDS_FORMATTER__