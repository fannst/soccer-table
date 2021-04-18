#include "debug.hpp"

/// Gets the string name for a debug word.
const char *debugWordToString (DebugWord word) noexcept {
  switch (word) {
    case DebugWord::Startup:
      return "Startup";
    case DebugWord::StartupClockReady:
      return "Startup, Clock Ready";
    case DebugWord::StartupDone:
      return "Startup, all done";
    case DebugWord::StartupPeripheralsReady:
      return "Startup, peripherals ready";
    default:
      return "Invalid";
  }
}
