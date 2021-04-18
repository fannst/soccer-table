#include "main.hpp"

#include "Sysclock.hpp"

Main Main::INSTANCE;

/**************************************************************
 * Main Methods
 **************************************************************/

Main::Main (void) noexcept:
	m_Buzzer(GPIOA, 1),
	m_SPFPUsart (USART1),
	m_DebugUsart (USART2)
{
}
 
Main &Main::GetInstance (void) noexcept {
	return Main::INSTANCE;
}

/**************************************************************
 * Functions
 **************************************************************/

extern "C" int main (void) noexcept {
	Main &main = Main::GetInstance ();

	Sysclock::Init ();
	main.Setup ();

	for (;;) {
		main.Loop ();
	}

	return 0;
}
