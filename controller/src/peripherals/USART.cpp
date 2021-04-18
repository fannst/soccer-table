#include "peripherals/USART.hpp"

/**************************************************
 * USART Master Class
 **************************************************/

/// Creates a new USART instance.
USART::USART (USART_TypeDef *usart) noexcept:
	m_USART (usart)
{}

// Initializes the USART peripheral with the specified baud rate.
void USART::Init (uint32_t clk, uint32_t baud) {
	// Calculates and sets the baud rate.
	m_USART->BRR = (((2 * clk) + baud) / (2 * baud));

	// Configures the USART peripheral further.
	m_USART->CR1 = USART_CR1_TE		// Transmit Enable
		| USART_CR1_RE				// Receive Enable
		| USART_CR1_UE;				// USART Enable (EN)
}

/// Writes an single char to the USART peripheral and skips polling.
void USART::WriteDangerous (uint8_t c) const noexcept {
	*reinterpret_cast<volatile uint8_t *>(&m_USART->DR) = c;
}

/// Writes an single char to the USART peripheral.
void USART::Write (uint8_t c) const {
	*reinterpret_cast<volatile uint8_t *>(&m_USART->DR) = c;
	while (!(m_USART->SR & USART_SR_TXE));
}

/// Writes N bytes to the USART peripheral.
void USART::Write (const uint8_t *s, uint16_t n) const {
	for (uint16_t i = 0; i < n; ++i) {
		USART::Write(s[i]);
	}
}

/// Writes an string to the USART peripheral.
void USART::Write (const char *s) const {
	while (*s != '\0') {
		USART::Write(*s++);
	}
}

/// Reads an byte from the USART peripheral.
uint8_t USART::Read (void) const {
	while (!(m_USART->SR & USART_SR_RXNE));
	return *reinterpret_cast<volatile uint8_t *>(&m_USART->DR);
}
