#include "main.hpp"

#include "Sysclock.hpp"
#include "Delay.hpp"

Main Main::INSTANCE;

/**************************************************************
 * NVIC Interrupts
 **************************************************************/

/// Gets called on a USART1 RX event.
extern "C" void USART1_IRQHandler (void) noexcept {
	if (USART1->ISR & USART_ISR_RXNE_RXFNE) { // RXNE Event.
		GPIOA->ODR |= (0x1 << 0);

		// Reads the byte from the read FIFO, after which we update the state
		//  machine with the newly received byte.
		uint8_t byte = *reinterpret_cast<volatile uint8_t *>(&USART1->RDR);
		spfp_update(Main::GetInstance ().getSPFPSM (), byte);
		
		GPIOA->ODR &= ~(0x1 << 0);
	}
}

/**************************************************************
 * Main Methods
 **************************************************************/

Main::Main (void) noexcept:
	m_Buzzer(GPIOA, 1),
	m_USART1 (USART1),
	m_SPFPFifoPacketCount (0),
	m_SPFPFifo(m_SPFPFifoBuffer, MAIN_SPFP_FIFO_BUFFER_SIZE)
{
	spfp_sm_init (&m_SPFPStateMachine, m_SPFPBuffer, MAIN_SPFP_BUFFER_SIZE);
}
 
Main &Main::GetInstance (void) noexcept {
	return Main::INSTANCE;
}

void Main::Setup (void) noexcept {
	Main::ResetClockControlInit ();
	Main::StatusLEDsInit ();
	Main::PeripheralsInit ();

	m_Buzzer.Buzz(900, 150);
}

void Main::Loop (void) noexcept {
	if (m_SPFPFifoPacketCount > 0) {
		--m_SPFPFifoPacketCount;

		uint16_t packetLen;
		uint8_t *buffer;

		// Reads the packet length from the FIFO, after which we allocate the required
		//  number of bytes and read the packet from the FIFO.
		m_SPFPFifo.ReadBytes(reinterpret_cast<uint8_t *>(&packetLen), sizeof (uint16_t));
		buffer = reinerpret_cast<uint8_t *>(malloc (packetLen));
		m_SPFPFifo.ReadBytes(buffer, packetLen);

		// Checks the packet, gets the class and performs a switch to indicate
		//  the packet type.
		const spfp_packet_t *packet = reinterpret_cast<spfp_packet_t *>(buffer);
		const spfp_class_pkt_t *class_packet = reinterpret_cast<const spfp_class_pkt_t *>(packet->p);
		switch (class_packet->c) {
			case SPFP_CLASS_SYSTEM:
				OnSPFPSystemPacket (reinterpret_cast<const spfp_system_pkt_t *>(class_packet->p));
				break;

		}

		// Frees the memory allocated for the packet.
		free (buffer);
	}

	// Kicks the watchdog to let it know we're all okay.
	Watchdog::Kick ();
}

/// Gets called when a SPFP packet is recieved.
void Main::OnSPFPPacket (spfp_packet_t *packet) noexcept {
	uint8_t cs = packet->cs;
	packet->cs = 0;
	if (spfp_calc_checksum(packet) != cs) {
		return;
	}

	// Writes the received packet length, after which we write the packet itself
	//  and increment the fifo packet count, so the main loop can process the packet.
	m_SPFPFifo.WriteBytes(reinterpret_cast<const uint8_t *>(&packet->len), sizeof (uint16_t));
	m_SPFPFifo.WriteBytes(reinterpret_cast<const uint8_t *>(packet), packet->len);
	++m_SPFPFifoPacketCount;
}

/// Gets called when a SPFP System packet.
void Main::OnSPFPSystemPacket (const spfp_system_pkt_t *packet) noexcept {\
	switch (packet->op) {
		case SPFP_CONTROL_OP_BEEP:
			m_Buzzer.Buzz (200, 500);
			Delay::Ms(120);
			m_Buzzer.Buzz(200, 400);
			break;
		case SPFP_CONTROL_OP_RESTART:
			NVIC_SystemReset ();
			break;
        default:
            break;
	}
}

/// Gets called on a LRAM packet.
void Main::OnLRAMPacket (const spfp_lram_pkt_t *packet) noexcept {
    switch (packet->op) {
        case SPFP_LRAM_OP_HOME:
            OnLRAMHomePacket (reinterpret_cast<const spfp_lram_home_pkt_t *>(packet->p));
            break;
        default:t
            break;
    }
}

/// Gets called on a LRAM Homing packet.
void Home::OnLRAMHomePacket (const spfp_lram_home_pkt_t *packet) noexcept {
    do {
        packet = spfp_lram_home_next (packet);
    } while (packet != nullptr);
}

/// Initializes the RCC clock sources for peripherals.
void Main::ResetClockControlInit (void) noexcept {
	RCC->APB2ENR |= (RCC_APB2ENR_USART1EN);
	RCC->AHB4ENR |= (RCC_AHB4ENR_GPIOAEN);
	RCC->APB3ENR |= (RCC_APB3ENR_WWDG1EN);
}

/// Initializes the peripherals / drivers.
void Main::PeripheralsInit (void) noexcept {
	Watchdog::Init ();			// Initializes the WWDG1.
	Delay::Init ();				// Initializes the delay functions.
	Main::RS232Init ();			// Initializes the RS232 USART.

	m_Buzzer.Init ();
}

/// Initializes the USART2 and the RS232 peripheral.
void Main::RS232Init (void) noexcept {
	// Makes A9 and A10 Alternative Function.
	GPIOA->MODER &= ~((0x3 << GPIO_MODER_MODE10_Pos)
			| (0x3 << GPIO_MODER_MODE9_Pos));
	GPIOA->MODER |= ((0x2 << GPIO_MODER_MODE10_Pos)
			| (0x2 << GPIO_MODER_MODE9_Pos));

	// Selects AF7 for A9 and A10.
	GPIOA->AFR[1] |= ((7 << GPIO_AFRH_AFSEL10_Pos)
			| (7 << GPIO_AFRH_AFSEL9_Pos));

	// Initializes the USARt1 periperhal.
	m_USART1.Init((120 * 1000000), 9600);

	// Enables the USART1 interrupt in the NVIC.
	NVIC_EnableIRQ (USART1_IRQn);

	// Enables the USART1 RXNE Interrupt.
	m_USART1.EnableRXNEInterrupt ();
}

/// Initializes the status LED's.
void Main::StatusLEDsInit (void) noexcept {
	GPIOA->MODER &= ~(0x3 << GPIO_MODER_MODE0_Pos);
	GPIOA->MODER |= (0x1 << GPIO_MODER_MODE0_Pos);
}

/**************************************************************
 * SPFP Overrides
 **************************************************************/

extern "C" {
	/// Override for the write byte method.
	void __spfp_write_byte (uint8_t byte, void *u) {
		reinterpret_cast<USART *>(u)->Write(byte);
	}

	/// Handles an overflwo of the buffer in a state machine.
	void __spfp_sm_overflow_handler (spfp_sm_t *sm) {
		// Overflows are not accepted, and if one happens
		//  the system is failing, so reset.
		NVIC_SystemReset ();
	}

	/// Gets called once a valid packet has been received.
	void __spfp_sm_packet_handler (spfp_sm_t *sm) {
		Main::GetInstance ().OnSPFPPacket (reinterpret_cast<spfp_packet_t *>(sm->b));
	}
}

/**************************************************************
 * Functions
 **************************************************************/

extern "C" int main (void) noexcept {
	Main &main = Main::GetInstance ();

	// Initializes the system clock to 480 MHZ, and calls the
	//  setup code, after which we will enter a infinite event loop.
	
	Sysclock::Init ();
	main.Setup ();

	for (;;) {
		main.Loop ();
	}

	return 0;
}
