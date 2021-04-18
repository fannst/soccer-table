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

#include <stdexcept>
#include <iostream>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <err.h>
#include <errno.h>
#include <sys/ioctl.h>

#include "spfp.hpp"

#pragma once

namespace SPFP {
  class TermiosSession : public Session {
  public:
    /// Creates new termios session instance.
    TermiosSession (void) noexcept;

    /// Creates the session for the specified path.
    void Connect (const char *path, uint32_t baud);

    /// Gets called on each direct byte write, to ensure transmission.
		virtual void EnsureTransmission (void) noexcept;

    /// Polls for RX.
		virtual void RXPoll (void);

		/// Polls for TX.
		virtual void TXPoll (void);
  protected:
    int32_t m_FD;
  };
}