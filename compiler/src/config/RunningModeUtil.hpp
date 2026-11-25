#pragma once

#include "config/RunningMode.hpp"
#include <string_view>

namespace vnlc {
    namespace RunningModeUtil {
        [[nodiscard]] RunningMode getRunningMode(std::string_view modeLiteral);
    };
} // namespace vnlc
