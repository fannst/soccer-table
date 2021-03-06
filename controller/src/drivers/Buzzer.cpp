#include "drivers/Buzzer.hpp"
#include "Delay.hpp"

Buzzer::Buzzer (GPIO_TypeDef *gpio, uint8_t pin) noexcept:
	m_GPIO (gpio), m_Pin(pin)
{}

void Buzzer::Init (void) noexcept {
	m_GPIO->MODER &= ~(0x3 << (static_cast<uint32_t>(m_Pin) * 2));
	m_GPIO->MODER |= (0x1 << (static_cast<uint32_t>(m_Pin) * 2));

	m_GPIO->ODR &= ~(0x1 << static_cast<uint32_t>(m_Pin));
}

void Buzzer::Buzz (uint16_t pulses, uint16_t delay) noexcept {
	for (; pulses > 0; --pulses) {
		m_GPIO->ODR |= (0x1 << static_cast<uint32_t>(m_Pin));
		Delay::Us(delay);
		m_GPIO->ODR &= ~(0x1 << static_cast<uint32_t>(m_Pin));
		Delay::Us(delay);
	}
}

