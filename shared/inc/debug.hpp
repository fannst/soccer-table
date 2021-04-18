/*

Copyright 2021 Luke A.C.A. Rieff

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/

#include <stdint.h>

#pragma once

#define DEBUG_START_BYTE          (0x77)
#define DEBUG_END_BYTE            (0x78)
#define DEBUG_ESCAPE_BYTE         (0x79)

/**************************************************************
 * Data Types
 **************************************************************/

enum class DebugWord {
  /* Startup Shit */
  Startup = 0, StartupClockReady, StartupPeripheralsReady, StartupDone,
  /* Stepper Shit */
  StepperHome, StepperMove, StepperEnable, StepperDisable
};

typedef struct __attribute__ (( packed )) {
  uint32_t    word;       /* The Debug Word */
  uint8_t     p[0];       /* The Debug Packet Payload */
} debug_frame_t;

/**************************************************************
 * X86 Only
 **************************************************************/

#ifndef __EMBED

/// Gets the string name for a debug word.
const char *debugWordToString (DebugWord word) noexcept;

#endif

/**************************************************************
 * ARM Only
 **************************************************************/

#ifdef __EMBED

class DebugSession {
public:
  virtual void Write (const debug_frame_t *frame) noexcept;
private:

};

#endif