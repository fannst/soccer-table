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

#include <stdint.h>

#pragma once

enum class RingBufferError {
	Underflow, Overflow, Ok, PredictedUnderflow, PredictedOverflow
};

class RingBuffer {
public:
	RingBuffer (uint8_t *buffer, uint32_t capacity) noexcept;
	~RingBuffer (void) = default;

	/// Writes a byte into the ring buffer.
	RingBufferError Write (uint8_t byte) noexcept;

	/// Reads a byte from the ring buffer.
	RingBufferError Read (uint8_t *ret) noexcept;

	/// Writes N bytes to the ring buffer.
	RingBufferError Write (const uint8_t *bytes, uint32_t n) noexcept;

	/// Reads N bytes from the ring buffer.
	RingBufferError Read (uint8_t *ret, uint32_t n) noexcept;

	/// Gets the size of the ring buffer.
	inline uint32_t GetSize () const noexcept {
		return m_Size;
	}

	/// Checks if the ring buffer is empty.
	inline bool IsEmpty () const noexcept {
		return m_Size == 0;
	}

	/// Checks if there is space available.
	inline bool IsFull () const noexcept {
		return m_Size == m_Capacity;
	}
private:
	uint8_t *m_Buffer;
	uint32_t m_Size,		/* Buffer size used of capacity */
			 m_Capacity,	/* Total Buffer Capacity */
			 m_Write,		/* The Write Pointer */
			 m_Read;		/* The Read Pointer */
};

