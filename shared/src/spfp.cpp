#include "spfp.hpp"

namespace SPFP {
	/// Initiates a new session with specified user data.
	Session::Session (void *udata) noexcept:
		m_RXBuffer (m_RXBufferBin, SPFP_SESSION_RX_BUFFER_SIZE),
		m_TXBuffer (m_TXBufferBin, SPFP_SESSION_TX_BUFFER_SIZE),
		m_RXBufferFrameCount (0),
		m_UserData (udata)
	{}

	/// Adds 2 bytes to the OC16 checksum.
	uint16_t __oc16_add (uint16_t cs, uint16_t val) noexcept {
		if ((static_cast<uint32_t>(cs) + static_cast<uint32_t>(val)) >= 0xFFFF) {
			++cs;
		}

		cs += val;

		return cs;
	}

	/// Calculates an OC16 checksum.
	uint16_t __oc16_do (uint8_t *buffer, uint16_t size) noexcept {
		uint16_t cs = 0x0000;

		// Performs the cehcksum calculation of the even part of the checksum
		//  we will skip the (possible) last byte, and truncate it with zero
		//  later in the process.
		uint16_t n = (size % 2 == 0) ? size : size - 1;
		for (uint16_t i = 0; i < n; i += 2) {
			cs = __oc16_add (cs, *reinterpret_cast<uint16_t *>(buffer));
			buffer += 2;
		}

		// Checks if N is not equal to the size, if so we know truncation is
		//  required to make clean checksum.
		if (n != size) {
			uint16_t halfword = (static_cast<uint16_t>(*buffer) << 8);
			cs = __oc16_add (cs, halfword);
		}

		// Idk why, but the IP protocol performs an invert operation.
		return ~cs;
	}

	/// Writes an frame to the session.
	void Session::Write (const frame_t *frame) noexcept {
		// Writes the start byte.
		WriteByteDirect (static_cast<uint8_t>(Flag::Start));

		// Loops over each individual byte of the frame, writing them
		//  and escaping if required.
		const uint8_t *p = reinterpret_cast<const uint8_t *>(frame);
		for (uint16_t i = 0; i < frame->len; ++i) {
			WriteByte (p[i]);
		}

		// Sends the end flag, and sets the current state to awaiting response
		//  this will make us wait for a ACK, NACK or timeout.
		WriteByteDirect (static_cast<uint8_t>(Flag::End));
	}

	/// Writes an byte to the session.
	void Session::WriteByte (uint8_t byte) noexcept {
		// Checks first if the current byte is a control byte, if so we will
		//  sne da escape flag first.
		if (byte > static_cast<uint8_t>(Flag::__BEG) && byte < static_cast<uint8_t>(Flag::__END)) {
			WriteByteDirect (static_cast<uint8_t>(Flag::Escape));
		}

		// Writes the actual data byte.
		WriteByteDirect (byte);
	}
	
	/// Writes a byte direct to the other device (without escaping).
	void Session::WriteByteDirect (uint8_t byte) noexcept {
		// Stays in loop as long as there is overflow.
		while (m_RXBuffer.Write (byte) == RingBufferError::Overflow);
	}

	/// Reads an frame from the session.
	bool Session::Read (frame_t *frame) noexcept {
		if (m_RXBufferFrameCount == 0) {
			return false;
		}

		uint8_t *p = reinterpret_cast<uint8_t *>(frame);
		m_RXBuffer.Read (p, sizeof (frame_t));

		return true;
	}
}
