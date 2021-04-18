/*
	>> Copyright Luke A.C.A. Rieff & Rien Dumore - Project-D ROC Nijmegen
*/

#include "inc.hpp"

#include "peripherals/Watchdog.hpp"
#include "drivers/Buzzer.hpp"
#include "peripherals/USART.hpp"

#pragma once

#define MAIN_SPFP_BUFFER_SIZE			(128)
#define MAIN_SPFP_FIFO_BUFFER_SIZE		(1024)

/**************************************************************
 * NVIC Interrupts
 **************************************************************/

/// Gets called on a USART2 RX event.
extern "C" void USART2_IRQHandler (void) noexcept;

/**************************************************************
 * Classes
 **************************************************************/

class Main {
public:
	static Main &GetInstance (void) noexcept;

	void Setup (void) noexcept;
	
	/// Gets called over and over again, used for process main loop.
	void Loop (void) noexcept;

	/// Gets called over and over again, used for polling to replace interrupts.
	void Poll (void) noexcept;

	inline USART &getUsart1 (void) noexcept
	{ return m_SPFPUsart; }
private:
	/// Initializes the RCC clock sources for peripherals.
	void ResetClockControlInit (void) noexcept;

	/// Initializes the peripherals / drivers.
	void PeripheralsInit (void) noexcept;

	/// Initializes the USART1 for SPFP.
	void SPFPUsartInit (void) noexcept;

	/// Initializes the debug USART
	void DebugUsartInit (void) noexcept;

	/// Initializes the status LED's.
	void StatusLEDsInit (void) noexcept;

	Main (void) noexcept;
	~Main (void) = default;

	/* Driver Instances */
	Buzzer m_Buzzer;

	/* Peripheral Driver Instances */
	USART m_SPFPUsart,
				m_DebugUsart;

	/* The Singleton Instance */
	static Main INSTANCE;
};

/**************************************************************
 * Functions
 **************************************************************/

extern "C" int main (void) noexcept;
