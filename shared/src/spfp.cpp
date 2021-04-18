#include "spfp.hpp"

namespace SPFP {
	/// Initiates a new session with specified user data.
	Session::Session (void *udata) noexcept:
		m_RXBuffer (m_RXBufferBin, SPFP_SESSION_RX_BUFFER_SIZE),
		m_TXBuffer (m_TXBufferBin, SPFP_SESSION_TX_BUFFER_SIZE),
		m_RXBufferFrameCount (0),
		m_TempRXBufferLevel (0),
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
	uint16_t __oc16_do (const uint8_t *buffer, uint16_t size) noexcept {
		uint16_t cs = 0x0000;

		// Performs the cehcksum calculation of the even part of the checksum
		//  we will skip the (possible) last byte, and truncate it with zero
		//  later in the process.
		uint16_t n = (size % 2 == 0) ? size : size - 1;
		for (uint16_t i = 0; i < n; i += 2) {
			cs = __oc16_add (cs, *reinterpret_cast<const uint16_t *>(buffer));
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

	/// Generates and inserts an checksum into the current frame.
	void AssignChecksum (frame_t *frame) {
		frame->cs = 0x0000;
		frame->cs = __oc16_do (reinterpret_cast<const uint8_t *>(frame), frame->len);
	}

	/// Gets and validates the checksum from the current frame.
	bool ValidateChecksum (const frame_t *frame) {
		return (__oc16_do (reinterpret_cast<const uint8_t *>(frame), frame->len) == 0xFFFF);
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
		EnsureTransmission ();
		if (m_TXBuffer.Write (byte) != RingBufferError::Ok) {
			// TODO: Add debug overflow error
		}
	}

	/// Reads an frame from the session.
	bool Session::Read (frame_t *frame) noexcept {
		if (m_RXBufferFrameCount == 0) {
			return false;
		}

		m_RXBuffer.Read (reinterpret_cast<uint8_t *>(frame), sizeof (frame_t));
		m_RXBuffer.Read (reinterpret_cast<uint8_t *>(frame->p), frame->len - sizeof (frame_t));

		--m_RXBufferFrameCount;

		return true;
	}

	/// Gets called once the state machine has detected new frame.
	void Session::OnStateMachineFrame (void) noexcept {
		// Gets the frame, and calculates the checksum, so we can determine
		//  to request a retransmission for the current frame (if it had a checksum error).
		const frame_t *frame = reinterpret_cast<const frame_t *>(m_TempRXBuffer);
		if (!ValidateChecksum (frame)) {
			// Sends the NACK byte to indicate a invalid checksum, and the device
			//  will initiate a retransmission.
			WriteByteDirect (static_cast<uint8_t>(Flag::Nack));
			
			// TODO: Add debug message for invalid checksum.
			return;
		}

		// Writes the received frame to the RX Buffer, if it does not return Ok
		//  there is either a predicted overflow, or direct overflow.
		if (m_RXBuffer.Write (m_TempRXBuffer, m_TempRXBufferLevel) != RingBufferError::Ok) {
			// TODO: Handle Overflow
			return;
		}

		// Adds one to the packet count, so main can read and handle
		//  the received packet.
		++m_RXBufferFrameCount;

		// Sends the acknowledgement flag, which tells the other device
		//  that the checksum was okay, and we've got all the data.
		WriteByteDirect (static_cast<uint8_t>(Flag::Ack));
	}

	/// Updates the receiving state machine.
	void Session::UpdateRXStateMachine (uint8_t byte) noexcept {
		// Checks if the next byte is being escaped, if not check for any
		//  control bytes, else just set escape next to false, and store
		//  the received byte.
		if (!m_EscapeNext) {
			switch (byte) {
				/*
					START Flag - Reset State Machine, Receive ...
				*/
				case static_cast<uint8_t>(Flag::Start):
					m_TempRXBufferLevel = 0;			// Reset RX Buffer level.
					m_ReceivingFrame = true;			// Set receiving frame to true.
					return;
				/*
					END Flag - Handle Received Frame.
				*/
				case static_cast<uint8_t>(Flag::End):
					// Checks if we're currentl receiving a frame, if not
					//  something is goign wrong, report error.
					if (m_ReceivingFrame == false) {
						// TODO: Report Invalid END condition.
						return;
					}

					// Calls the on packet method, which will copy the received
					//  frame into the reception FIFO.
					OnStateMachineFrame ();

					// Sets receiving frame to false, since we're done with the
					//  current frame.
					m_ReceivingFrame = false;

					return;
				/*
					ESC Flag - Escape Next Byte.
				*/
				case static_cast<uint8_t>(Flag::Escape):
					m_EscapeNext = true;
					return;
				/*
					Handles other BS bytes.
				*/
				default:
					break;
			}
		} else {
			m_EscapeNext = false;
		}

		// Check if we're currenty receiving any frame, if so append it
		//  to the current reception buffer.
		if (m_ReceivingFrame) {
			if ((m_TempRXBufferLevel + 1) > SPFP_SESSION_RX_BUFFER_SIZE) {
				// TODO: Debug Overflow Error handler.
				return;
			}

			m_TempRXBuffer[m_TempRXBufferLevel++] = byte;
		}
	}

	/// Sends the RX, TX ready packet.
	void Session::WriteReadyPacket (uint8_t *buffer) noexcept {
		frame_t *frame = reinterpret_cast<frame_t *>(buffer);
		frame->len = sizeof (frame_t) + sizeof (packet_t);

		packet_t *packet = reinterpret_cast<packet_t *>(frame->p);
		packet->proto = static_cast<uint8_t>(DefaultProto::Control);
		packet->ptype = static_cast<uint8_t>(CtrlProtoType::Ready_RX_TX);

		AssignChecksum (frame);
		Write (frame);
	}
}
