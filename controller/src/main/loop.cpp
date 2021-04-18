#include "main.hpp"

/// Gets called over and over again, used for process main loop.
void Main::Loop (void) noexcept {
	Watchdog::Kick ();
}

/// Gets called over and over again, used for polling to replace interrupts.
void Main::Poll (void) noexcept {
  m_SPFPUsartSession.RXPoll ();     // Performs the RX Polling.
  m_SPFPUsartSession.TXPoll ();     // Performs the TX polling.
}
