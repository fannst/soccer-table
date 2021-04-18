#include "peripherals/Watchdog.hpp"

/// Initializes the watchdog.
void Watchdog::Init (void) noexcept {
  WWDG->CFR = ((0x3 << WWDG_CFR_WDGTB_Pos)		// (PCLK div 4096) / 8
    | (0x7F << WWDG_CFR_W_Pos));							// Window Size
  WWDG->CR = (WWDG_CR_WDGA										// Activate The Watchdog
    | (0x7F << WWDG_CR_T_Pos));								// Set the initial value of T
}

/// Kicks the watchdog.
void Watchdog::Kick (void) noexcept {
  WWDG->CR |= (0x7F << WWDG_CR_T_Pos);
}