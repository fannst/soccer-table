/*
	>> Copyright Luke A.C.A. Rieff & Rien Dumore - Project-D ROC Nijmegen
*/


#include "inc.hpp"

#pragma once

class Buzzer {
public:
	/// Creates a new buzzer instance.
	Buzzer (GPIO_TypeDef *gpio, uint8_t pin) noexcept;

	/// Initializes the buzzer driver for specified pin / GPIO.
	void Init (void) noexcept;

	/// Buzzes the buzzer LOL.
	void Buzz (uint16_t pulses, uint16_t delay) noexcept;
private:
	GPIO_TypeDef *m_GPIO;
	uint8_t m_Pin;
};

