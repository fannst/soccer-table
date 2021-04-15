/*
	>> Copyright Luke A.C.A. Rieff & Rien Dumore - Project-D ROC Nijmegen
*/

#include "inc.hpp"

#pragma once

class FIFO {
public:
	/// Creates a new FIFO instance.
	FIFO (uint8_t *buffer, uint32_t capacity) noexcept;

	/// Reads one byte from the FIFO.
	uint8_t ReadByte (void) noexcept;

	/// Reads N bytes from the FIFO.
	void ReadBytes (uint8_t *buffer, uint32_t n) noexcept;

	/// Writes one byte to the FIFO.
	void WriteByte (uint8_t byte) noexcept;

	/// Writes N bytes to the buffer.
	void WriteBytes (const uint8_t *buffer, uint32_t n) noexcept;

	/// Gets the size of the current FIFO.
	uint32_t getSize () const noexcept;
private:
	uint8_t *m_Buffer;
	uint32_t m_Capacity, m_Size, m_Read, m_Write;
};
