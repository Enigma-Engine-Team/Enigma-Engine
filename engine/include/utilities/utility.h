#pragma once
#include <cstdint>
#include <random>

#include "rttr/registration_friend.h"

class UUID
{
public:
    UUID();
    UUID(uint64_t high, uint64_t low);
    static std::string ToString(uint64_t high, uint64_t low);
    std::string ToString() const;
    static UUID ToUUID(const std::string& str);
    //return true if is equal to the other UUID
    bool IsEqual(const UUID& other) const { return high == other.high && low == other.low; }

private:
    RTTR_REGISTRATION_FRIEND
    uint64_t high;
    uint64_t low;
    std::string id;
};
