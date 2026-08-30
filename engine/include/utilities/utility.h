#pragma once
#include <cstdint>
#include <random>

#include "../../../out/build/x64-release/_deps/rttr-src/src/rttr/registration_friend.h"

class UUID
{
public:
    UUID();
    UUID(uint64_t high, uint64_t low);
    static std::string ToString(uint64_t high, uint64_t low);
    std::string ToString() const;
    static UUID ToUUID(const std::string& str);
    bool IsEqual(const UUID& other) const { return high == other.high && low == other.low; }

private:
    RTTR_REGISTRATION_FRIEND
    uint64_t high;
    uint64_t low;
    std::string id;
};
