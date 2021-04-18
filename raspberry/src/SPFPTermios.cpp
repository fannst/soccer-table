#include "SPFPTermios.hpp"

namespace SPFP {
    /// Creates new termios session instance.
    TermiosSession::TermiosSession (void) noexcept:
      Session::Session (nullptr)
    {}

    /// Creates the session for the specified path.
    void TermiosSession::Connect (const char *path, uint32_t baud) {
      struct termios tty;
      
      // Attempts to open the FD.
      if ((m_FD = open (path, O_RDWR | O_NOCTTY | O_SYNC)) < 0) {
        throw std::runtime_error (std::string("Failed to create file pointer: ") + strerror (errno));
      }

      if (tcgetattr (m_FD, &tty) != 0) {
        throw std::runtime_error (std::string("Failed to get attributes: ") + strerror (errno));
      }

      cfsetspeed (&tty, baud);

      tty.c_cflag &= ~(CSIZE | PARENB | PARODD | CSTOPB | CRTSCTS);
      tty.c_cflag |= (CLOCAL | CREAD | CS8);

      tty.c_iflag &= ~(IGNBRK | IXON | IXOFF | IXANY);
      
      tty.c_lflag = 0;
      tty.c_oflag = 0;

      tty.c_cc[VMIN] = 0;
      tty.c_cc[VTIME] = 1;

      if (tcsetattr (m_FD, TCSANOW, &tty) != 0) {
        throw std::runtime_error (std::string("Failed to set attributes: ") + strerror (errno));
      }
    }

    /// Gets called on each direct byte write, to ensure transmission.
		void TermiosSession::EnsureTransmission (void) noexcept {

    }

    /// Polls for RX.
		void TermiosSession::RXPoll (void) {
      int32_t availableBytes;
      ioctl (m_FD, FIONREAD, &availableBytes);

      // Checks if there were any bytes available, if not just return
      //  and do not do anything else.
      if (availableBytes <= 0) {
        return;
      }

      // Allocate memory for the reception of the bytes.
      uint8_t *buffer = new uint8_t[availableBytes];
      if (buffer == nullptr) {
        throw std::runtime_error ("Allocation failed!");
      }

      // Reads all the bytes in the just allocated buffer, after which we
      //  loop over all the available bytes, and call the state machine update.
      if (read (m_FD, buffer, availableBytes) == -1) {
        throw std::runtime_error (std::string ("Read failed: ") + strerror (errno));
      }

      for (int32_t i = 0; i < availableBytes; ++i) {
        UpdateRXStateMachine (buffer[i]);
      }

      // Frees the allocated memory.
      delete buffer;
    }

		/// Polls for TX.
		void TermiosSession::TXPoll (void) noexcept {

    }
}