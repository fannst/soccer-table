/*
	>> Copyright Luke A.C.A. Rieff & Rien Dumore - Project-D ROC Nijmegen
*/

#include "inc.hpp"
#include "spfp.h"

#include "peripherals/Watchdog.hpp"
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
	void OnSPFPPacket (const spfp_packet_t *packet) noexcept;

	/// Gets called when a SPFP System packet.
	void OnSPFPSystemPacket (const spfp_system_pkt_t *packet) noexcept;

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
 * SPFP Overrides
 **************************************************************/

/// Override for the write byte method.
void __spfp_write_byte (uint8_t byte, void *u);

/// Handles an overflwo of the buffer in a state machine.
void __spfp_sm_overflow_handler (spfp_sm_t *sm);

/// Gets called once a valid packet has been received.
void __spfp_sm_packet_handler (spfp_sm_t *sm);

/**************************************************************
 * Functions
 **************************************************************/

extern "C" int main (void) noexcept;
