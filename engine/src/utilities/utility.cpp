#include "utilities\utility.h"
#include <iomanip>

#include "scripting/script_base.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<UUID>("UUID")
         .property("id", &UUID::id);
}

UUID::UUID()
{
    static std::random_device rd;
    static std::mt19937_64 generator(rd());

    high = generator();
    low = generator();
    id = ToString(high, low);
}

UUID::UUID(uint64_t high, uint64_t low)
{
    this->high = high;
    this->low = low;
    id = ToString(high, low);
}

std::string UUID::ToString(uint64_t high, uint64_t low)
{
    std::stringstream ss;
    ss << std::hex << std::setiosflags(std::ios::uppercase) << std::setw(16) << std::setfill('0') << high
       << std::setiosflags(std::ios::uppercase) << std::setw(16) << std::setfill('0') << low;
    return ss.str();
}

std::string UUID::ToString() const
{
    return ToString(high, low);
}

UUID UUID::ToUUID(const std::string& str)
{
    if (str.length() != 32)
    {
        throw std::invalid_argument("Invalid UUID string length");
    }

    uint64_t newHigh = std::stoull(str.substr(0, 16), nullptr, 16);
    uint64_t newLow = std::stoull(str.substr(16, 16), nullptr, 16);

    return UUID(newHigh, newLow);
}
