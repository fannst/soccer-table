#include "datatypes/FIFO.hpp"

/// Creates a new FIFO instance.
FIFO::FIFO (uint8_t *buffer, uint32_t capacity) noexcept:
	m_Buffer (buffer),
	m_Capacity (capacity),
	m_Read (0),
	m_Write (0)
{}

/// Reads one byte from the FIFO.
uint8_t FIFO::ReadByte (void) noexcept {
	if (m_Size == 0) {
		return 0;
	}

	uint8_t byte = m_Buffer[m_Read];
	m_Read = (m_Read + 1) % m_Capacity;

	--m_Size;

	return byte;
}

/// Reads N bytes from the FIFO.
void FIFO::ReadBytes (uint8_t *buffer, uint32_t n) noexcept {
	for (uint32_t i = 0; i < n; ++i) {
		buffer[i] = FIFO::ReadByte ();
	}
}

/// Writes one byte to the FIFO.
void FIFO::WriteByte (uint8_t byte) noexcept {
	if (m_Size == m_Capacity) {
		return;
	}

	m_Buffer[m_Write] = byte;
	m_Write = (m_Write + 1) % m_Capacity;

	++m_Size;
}

/// Writes N bytes to the buffer.
void FIFO::WriteBytes (const uint8_t *buffer, uint32_t n) noexcept {
	if ((m_Size + n) > m_Capacity) {
		return;
	}

	for (uint32_t i = 0; i < n; ++i) {
		FIFO::WriteByte (buffer[i]);
	}
}

/// Gets the size of the current FIFO.
uint32_t FIFO::getSize () const noexcept {
	return m_Size;
}
