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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "fifo.hpp"

#pragma once

#define SPFP_SESSION_RX_BUFFER_SIZE 		(256)
#define SPFP_SESSION_TX_BUFFER_SIZE			(256)

namespace SPFP {
	/*
		## SPFP Protocol Example ##

		1. Master Initiating Transfer:
			[0x1 (START), 0x87 (DATA, CS), 0x3 (ESCAPE), 0x01 (DATA, CS), 0x88 (DATA, LEN), 0x00 (DATA, LEN), ... Escaped Payload, 0x2 (END)]
		
		2. Master Receiving ACK:
			[0x4 (ACK)]

		3. Master Receiving NACK:
			[0x5 (NACK)]
	*/

	enum class Flag {
		__BEG	= 0x0,
		Start 	= 0x1,			/* Start of frame */
		End 	= 0x2,				/* End of frame */
		Escape	= 0x3,			/* Escapes next byte */
		Ack		= 0x4,				/* Checksum OK, Packet Received */
		Nack	= 0x5,				/* Checksum Error, Retransmit */
		__END	= 0x6,
		__TIMEO = 0x7
	};

	enum class DefaultProto {
		Control = 0					/* SPFP Control Frames */
	};

	enum class CtrlProtoType {
		Ready_RX_TX	= 0,		/* SPFP Ready for TX and RX */
		Ready_RX_Only = 1,	/* SPFP Ready for RX only */
		Ready_TX_Only = 2,	/* SPFP Ready for TX only */
	};

	typedef struct __attribute__ (( packed )) {
		uint16_t 	cs;				/* Packet Checksum */
		uint16_t 	len;			/* Packet Length */
		uint8_t		p[0];			/* Packet Payload */
	} frame_t;

	typedef struct __attribute__ (( packed )) {
		uint8_t		proto;		/* Higher Level Protocol */
		uint8_t		ptype;		/* Higher Level Protocol, Packet Type */
		uint8_t		p[0];			/* Packet Payload */
	} packet_t;

	class Session {
	public:
		/// Initiates a new session with specified user data.
		Session (void *udata) noexcept;

		/// Writes an frame to the session.
		virtual void Write (const frame_t *frame) noexcept;

		/// Writes an byte to the session (with escaping).
		virtual void WriteByte (uint8_t byte) noexcept;

		/// Writes a byte direct to the other device (without escaping).
		virtual void WriteByteDirect (uint8_t byte) noexcept;

		/// Reads an frame from the session.
		virtual bool Read (frame_t *frame) noexcept;

		/// Polls for RX, will be implemented by child class.
		virtual void RXPoll (void) = 0;

		/// Polls for TX, will be implemented by child class.
		virtual void TXPoll (void) = 0;

		/// Gets called on each direct byte write, to ensure transmission.
		virtual void EnsureTransmission (void) noexcept = 0;

		/// Gets called once the state machine has detected new frame.
		virtual void OnStateMachineFrame (void) noexcept;

		/// Updates the receiving state machine.
		virtual void UpdateRXStateMachine (uint8_t byte) noexcept;

		/// Sends the RX, TX ready packet.
		void WriteReadyPacket (uint8_t *buffer) noexcept;
	protected:
		/* Buffer Crap */
		uint8_t m_RXBufferBin[SPFP_SESSION_RX_BUFFER_SIZE], 
						m_TXBufferBin[SPFP_SESSION_TX_BUFFER_SIZE],
						m_TempRXBuffer[SPFP_SESSION_RX_BUFFER_SIZE];
		RingBuffer m_RXBuffer, m_TXBuffer;
		uint32_t m_RXBufferFrameCount,
						 m_TempRXBufferLevel;

		/* Reception State */
		unsigned m_EscapeNext : 1;				/* Next Byte Escaped */
		unsigned m_ReceivingFrame : 1;		/* Currently Receiving Frame */

		/* User Data */
		void *m_UserData;
	};

	/// Adds 2 bytes to the OC16 checksum.
	uint16_t __oc16_add (uint16_t cs, uint16_t val) noexcept;

	/// Calculates an OC16 checksum.
	uint16_t __oc16_do (const uint8_t *buffer, uint16_t size) noexcept;

	/// Generates and inserts an checksum into the current frame.
	void AssignChecksum (frame_t *frame);

	/// Gets and validates the checksum from the current frame.
	bool ValidateChecksum (const frame_t *frame);
}
