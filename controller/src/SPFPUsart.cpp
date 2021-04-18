#include "SPFPUsart.hpp"

namespace SPFP {
  /// Initializes a new USARTSession with the specified USART peripheral.
  USARTSession::USARTSession (USART &usart) noexcept:
    Session::Session (nullptr),
    m_USART (usart),
    m_FirstInSequence (true)
  {}

  /// Polls for RX.
  void USARTSession::RXPoll (void) noexcept {
    if (m_USART.IsRXNE ()) { // Data available in USART->DR
      UpdateRXStateMachine (m_USART.Read ());
    }
  }

  /// Calls the SPFP Usart session initialization code.
  void USARTSession::Init (void) noexcept {
    WriteReadyPacket (WRITE_BUFF);
  }

  /// Gets called on each direct byte write, to ensure transmission.
  void USARTSession::EnsureTransmission (void) noexcept {
    if (m_TXBuffer.IsEmpty () && !m_FirstInSequence) {
      m_FirstInSequence = true;
    }
  }

  /// Polls for TX.
  void USARTSession::TXPoll (void) noexcept {
    if (m_USART.IsTXE () || m_FirstInSequence) { // Data can be written to USART->DR

      // Sets first in sequence to false, to prevent future non-required writes.
      m_FirstInSequence = false;
      
      // Checks if the TX Ring Buffer is empty, if so nothing
      //  should be written so return.
      if (m_TXBuffer.IsEmpty ()) {
        // return;
      }

      // Read a single byte from the TX Ring Buffer, after which
      //  we write it to the usart, and skip the polling for TXNE.
      uint8_t byte;
      if (m_TXBuffer.Read (&byte) != RingBufferError::Ok) {
        // TODO: Write error handlig via serial debugger.
        // return;
      }

      m_USART.WriteDangerous (byte);
    }
  }
}
