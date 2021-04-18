/*
	>> Copyright Luke A.C.A. Rieff & Rien Dumore - Project-D ROC Nijmegen
*/

#include "inc.hpp"
#include "Sysclock.hpp"

#pragma once

/**************************************************
 * Class Definitions
 **************************************************/

class USART {
public:

	/// Creates a new USART instance.
	USART (USART_TypeDef *usart) noexcept;

	// Initializes the USART peripheral with the specified baud rate.
	virtual void Init (uint32_t clk, uint32_t baud);

	/// Writes an single char to the USART peripheral and skips polling.
	virtual void WriteDangerous (uint8_t c) const noexcept;

	/// Writes an single char to the USART peripheral.
	virtual void Write (uint8_t c) const;

	/// Writes N bytes to the USART peripheral.
	virtual void Write (const uint8_t *s, uint16_t n) const;

	/// Writes an string to the USART peripheral.
	virtual void Write (const char *s) const;

	/// Reads an byte from the USART peripheral.
	virtual uint8_t Read (void) const;

	/// Enables the RXNE interrupt.
	inline void EnableRXNEInterrupt (void) noexcept {
		m_USART->CR1 |= USART_CR1_RXNEIE;
	}

	/// Checks the RXNE Flag.
	inline bool IsRXNE (void) noexcept {
		return m_USART->SR & USART_SR_RXNE;
	}

	/// Checks the TXE Flag.
	inline bool IsTXE (void) noexcept {
		return m_USART->SR & USART_SR_TXE;
	}
protected:
	USART_TypeDef *m_USART;
};
