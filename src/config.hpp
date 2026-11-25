#pragma once

#include "config-utils/shared/config-utils.hpp"

// Declare the mod config with the name Config, which determines the get() function name
// This would generate getConfig(), DECLARE_CONFIG(ModCfg) would generate getModCfg()
DECLARE_CONFIG(Config) {
    // Create a config value
    // Parameters: (C++ name, type, title, default value, optional description)
    CONFIG_VALUE(SystemCrash, bool, "System Crash", false, "Replaces game crash with kernel panic");

    // Like with custom types, you can also have regular C++ struct or class contents here
};