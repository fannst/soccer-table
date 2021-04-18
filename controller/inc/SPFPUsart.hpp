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

#include "spfp.hpp"
#include "peripherals/USART.hpp"

#pragma once

namespace SPFP {
    class USARTSession : public Session {
    public:
        /// Initializes a new USARTSession with the specified USART peripheral.
        USARTSession (USART &usart) noexcept;

        /// Calls the SPFP Usart session initialization code.
        void Init (void) noexcept;

        /// Gets called on each direct byte write, to ensure transmission.
		virtual void EnsureTransmission (void) noexcept;

        /// Polls for RX.
		virtual void RXPoll (void) noexcept;

		/// Polls for TX.
		virtual void TXPoll (void) noexcept;

        uint8_t WRITE_BUFF[256];
        uint8_t READ_BUFF[256];
    protected:
        USART &m_USART;

        bool m_FirstInSequence;
    };
}
