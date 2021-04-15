/*
	>> Copyright Luke A.C.A. Rieff & Rien Dumore - Project-D ROC Nijmegen
*/

#include "inc.hpp"
#include "spfp.h"

#include "peripherals/Watchdog.hpp"
#include "drivers/Buzzer.hpp"
#include "datatypes/FIFO.hpp"
#include "peripherals/USART.hpp"
#include "spfp_ext.hpp"

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
	void Loop (void) noexcept;

	/// Gets called when a SPFP packet is recieved.
	void OnSPFPPacket (spfp_packet_t *packet) noexcept;

	/// Gets called when a SPFP System packet.
	void OnSPFPSystemPacket (const spfp_system_pkt_t *packet) noexcept;

	/// Gets called on a LRAM packet.
	void OnLRAMPacket (const spfp_lram_pkt_t *packet) noexcept;

	/// Gets called on a LRAM Homing packet.
	void OnLRAMHomePacket (const spfp_lram_home_pkt_t *packet) noexcept;

	inline USART &getUsart1 (void) noexcept
	{ return m_USART1; }

	inline spfp_sm_t *getSPFPSM (void) noexcept
	{ return &m_SPFPStateMachine; }
private:
	/// Initializes the RCC clock sources for peripherals.
	void ResetClockControlInit (void) noexcept;

	/// Initializes the peripherals / drivers.
	void PeripheralsInit (void) noexcept;

	/// Initializes the USART2 and the RS232 peripheral.
	void RS232Init (void) noexcept;

	/// Initializes the status LED's.
	void StatusLEDsInit (void) noexcept;

	Main (void) noexcept;
	~Main (void) = default;

	/* Driver Instances */
	Buzzer m_Buzzer;

	/* Peripheral Driver Instances */
	USART m_USART1;

	/* SPFP Instance Variables */
	uint8_t m_SPFPFifoBuffer[MAIN_SPFP_FIFO_BUFFER_SIZE];
	uint8_t m_SPFPBuffer[MAIN_SPFP_BUFFER_SIZE];
	uint8_t m_SPFPFifoPacketCount;
	FIFO m_SPFPFifo;
	spfp_sm_t m_SPFPStateMachine;

	/* The Singleton Instance */
	static Main INSTANCE;
};

/**************************************************************
 * Functions
 **************************************************************/

extern "C" int main (void) noexcept;
