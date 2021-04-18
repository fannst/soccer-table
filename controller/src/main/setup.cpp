#include "main.hpp"

#include "Delay.hpp"

void Main::Setup (void) noexcept {
	Main::ResetClockControlInit ();
	Main::StatusLEDsInit ();
	Main::PeripheralsInit ();

	m_Buzzer.Buzz(900, 150);
	m_Buzzer.Buzz(400, 500);
}

/// Initializes the RCC clock sources for peripherals.
void Main::ResetClockControlInit (void) noexcept {
	RCC->APB2ENR |= (RCC_APB2ENR_USART1EN);
	RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN
		| RCC_AHB1ENR_GPIOBEN
		| RCC_AHB1ENR_GPIOCEN);
	RCC->APB1ENR |= (RCC_APB1ENR_WWDGEN
		| RCC_APB1ENR_USART2EN);
}

/// Initializes the peripherals / drivers.
void Main::PeripheralsInit (void) noexcept {
	// Watchdog::Init ();			// Initializes the WWDG.
	Delay::Init ();

	Main::SPFPUsartInit ();
	Main::DebugUsartInit ();

	m_SPFPUsartSession.Init ();

	m_Buzzer.Init ();
}

/// Initializes the USART1 for SPFP.
void Main::SPFPUsartInit (void) noexcept {
	// Makes A9 and A10 Alternative Function.
	GPIOA->MODER &= ~((0x3 << GPIO_MODER_MODE10_Pos)
			| (0x3 << GPIO_MODER_MODE9_Pos));
	GPIOA->MODER |= ((0x2 << GPIO_MODER_MODE10_Pos)
			| (0x2 << GPIO_MODER_MODE9_Pos));

	// Selects AF7 for A9 and A10.
	GPIOA->AFR[1] |= ((7 << GPIO_AFRH_AFSEL10_Pos)
			| (7 << GPIO_AFRH_AFSEL9_Pos));

	// Initializes the USART1 periperhal.
	m_SPFPUsart.Init(MHZ(APB2_PERIPHERAL), 115200);
}

/// Initializes the debug USART
void Main::DebugUsartInit (void) noexcept {
	// Makes A2 and A3 Alternative Function.
	GPIOA->MODER &= ~((0x3 << GPIO_MODER_MODE2_Pos)
			| (0x3 << GPIO_MODER_MODE3_Pos));
	GPIOA->MODER |= ((0x2 << GPIO_MODER_MODE2_Pos)
			| (0x2 << GPIO_MODER_MODE3_Pos));

	// Selects AF7 for A9 and A10.
	GPIOA->AFR[0] |= ((7 << GPIO_AFRL_AFSEL2_Pos)
			| (7 << GPIO_AFRL_AFSEL3_Pos));

	// Initializes the USART2 peripheral.
	m_DebugUsart.Init (MHZ(APB1_PERIPHERAL), 115200);
}

/// Initializes the status LED's.
void Main::StatusLEDsInit (void) noexcept {
	GPIOA->MODER &= ~(0x3 << GPIO_MODER_MODE0_Pos);
	GPIOA->MODER |= (0x1 << GPIO_MODER_MODE0_Pos);
}