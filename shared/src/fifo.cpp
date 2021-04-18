#include "fifo.hpp"


RingBuffer::RingBuffer (uint8_t *buffer, uint32_t capacity) noexcept:
	m_Buffer (buffer), m_Size (0), m_Capacity (0), m_Write (0), m_Read (0)
{}

/// Writes a byte into the ring buffer.
RingBufferError RingBuffer::Write (uint8_t byte) noexcept {
	if (m_Size == m_Capacity) {
		return RingBufferError::Overflow;
	}

	m_Buffer[m_Write] = byte;
	m_Write = (m_Write + 1) % m_Capacity;

	++m_Size;

	return RingBufferError::Ok;
}

/// Reads a byte from the ring buffer.
RingBufferError RingBuffer::Read (uint8_t *ret) noexcept {
	if (m_Size == 0) {
		return RingBufferError::Underflow;
	}

	*ret = m_Buffer[m_Read];
	m_Read = (m_Read + 1) % m_Capacity;

	--m_Size;

	return RingBufferError::Ok;
}

/// Writes N bytes to the ring buffer.
RingBufferError RingBuffer::Write (const uint8_t *bytes, uint32_t n) noexcept {
	if ((m_Size + n) > m_Capacity) {
		return RingBufferError::PredictedOverflow;
	}

	for (uint32_t i = 0; i < n; ++i) {
		if (Write (bytes[i]) == RingBufferError::Overflow) {
			return RingBufferError::Overflow;
		}
	}

	return RingBufferError::Ok;
}

/// Reads N bytes from the ring buffer.
RingBufferError RingBuffer::Read (uint8_t *ret, uint32_t n) noexcept {
	if (n > m_Size) {
		return RingBufferError::PredictedUnderflow;
	}

	for (uint32_t i = 0; i < n; ++i) {
		if (Read (&ret[i]) == RingBufferError::Underflow) {
			return RingBufferError::Underflow;
		}
	}

	return RingBufferError::Ok;
}

