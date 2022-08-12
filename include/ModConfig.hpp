#pragma once

#include "config-utils/shared/config-utils.hpp"


DECLARE_CONFIG(ModConfig,
    CONFIG_VALUE(Enabled, bool, "Enabled", false);
    CONFIG_VALUE(Difficulty, int, "Difficulty", 2);

    CONFIG_INIT_FUNCTION(
        CONFIG_INIT_VALUE(Enabled);
        CONFIG_INIT_VALUE(Difficulty);

    )
)