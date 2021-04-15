#include "Sysclock.hpp"

void Sysclock::Init (void) noexcept {
	//
	// Configures flash
	//

	// Sets 2 flash wait states due to the BUS frequency of 120 Mhz

	FLASH->ACR &= FLASH_ACR_LATENCY;
	FLASH->ACR |= FLASH_ACR_LATENCY_2WS;

	//
	// Configures the actual clock
	//

	// Enables HSE
	RCC->CR |= RCC_CR_HSEON;
	while (!(RCC->CR & RCC_CR_HSERDY));

	// Sets the PLLSRC to HSE
	RCC->PLLCKSELR &= ~RCC_PLLCKSELR_PLLSRC;
	RCC->PLLCKSELR |= RCC_PLLCKSELR_PLLSRC_HSE;

	// Configure PLL1
	RCC->PLLCKSELR &= ~RCC_PLLCKSELR_DIVM1;				// Clear DIVM1
	RCC->PLLCKSELR |= (2 << RCC_PLLCKSELR_DIVM1_Pos);	// DIVM1: /2

	RCC->PLL1DIVR &= ~RCC_PLL1DIVR_N1;					// Clear DIVN1
	RCC->PLL1DIVR |= (119 << RCC_PLL1DIVR_N1_Pos);		// DIVN1: 120 ( 119 because 0x000 is 1 )

	RCC->PLL1DIVR &= ~RCC_PLL1DIVR_P1;					// Clear DIVP1
	RCC->PLL1DIVR |= (1 << RCC_PLL1DIVR_P1_Pos);		// DIVP1: 2, 0b1 = CLK/2

	// Enable PLL1Q Output ( For RNG ), and set DIV to 10, to get 48Mhz
	RCC->PLLCFGR |= RCC_PLLCFGR_DIVQ1EN;
	RCC->PLL1DIVR &= ~RCC_PLL1DIVR_Q1;
	RCC->PLL1DIVR |= (9 << RCC_PLL1DIVR_R1_Pos);		// 9 because 0b00 = 1

	// Configures HPRE
	RCC->D1CFGR &= RCC_D1CFGR_HPRE;						// Clear HPRE
	RCC->D1CFGR |= RCC_D1CFGR_HPRE_DIV2;				// HPRE: /2

	// Enable PLL1
	RCC->CR |= RCC_CR_PLL1ON;
	while (!(RCC->CR & RCC_CR_PLL1RDY));

	// Select PLL1 as main clock source
	RCC->CFGR |= RCC_CFGR_SW_PLL1;
	while ((RCC->CFGR & RCC_CFGR_SW_Msk) != RCC_CFGR_SW_PLL1);

	//
	// Configures backup domain
	//

	// Grant access to backup domain
	PWR->CR1 |= PWR_CR1_DBP;

	// Checks if backup domain reset is required
	if ((RCC->BDCR & RCC_BDCR_RTCSEL) >> RCC_BDCR_RTCSEL_Pos != 0b01)
	{
		// Resets backup domain
		RCC->BDCR |= RCC_BDCR_BDRST;
		RCC->BDCR &= ~RCC_BDCR_BDRST;

		// Selects the clock source for the RTC
		RCC->BDCR |= (0b01 << RCC_BDCR_RTCSEL_Pos);
	}

	// Enables LSE
	RCC->BDCR |= RCC_BDCR_LSEON;
	while (!(RCC->BDCR & RCC_BDCR_LSERDY));

	// Enables LSE Clock Security
	RCC->BDCR |= RCC_BDCR_LSECSSON;

	//
	// Configures the RTC
	//

	RCC->BDCR |= RCC_BDCR_RTCEN;
}
