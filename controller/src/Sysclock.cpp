#include "Sysclock.hpp"

/*
  == STM32F446 Clock Configuration for 180Mhz system ==

  -- PLL
  PLL Source Mux : HSE
  PLLM : /4
  PLLN : x180
  PLLP : /2
  PLLQ : /2
  PLLR : /2

  -- System
  System Clock Mux : PLLCLK
  AHB  : /1
  APB1 : /4
  APB2 : /2

  -- Clocks
  Power : 180Mhz
  HCLK, AHB Bus, Core, DMA : 180Mhz
  Cortex System Timer : 180Mhz
  FCLK : 180Mhz
  APB1 Peripheral : 45Mhz
  APB1 Timer : 90Mhz
  APB2 Peripheral: 90Mhz
  APB2 Timer : 180Mhz

  == End Clock Configuration for 180Mhz system ==
*/

void Sysclock::Init (void) noexcept {
  // Enables the HSE (High Speed External) Crystal.
  RCC->CR |= RCC_CR_HSEON;											// Enable High Speed External
  while (!(RCC->CR & RCC_CR_HSERDY));						// Wait for HSE to be stable

  // Configures the flash to use 5 wait states, this is for HCLK <= 168, so
  //  we're kinda breaking rules LOL.
  FLASH->ACR = (FLASH_ACR_LATENCY_5WS						// 5 flash wait states
    | FLASH_ACR_ICEN														// Instruction Cache Enable
    | FLASH_ACR_DCEN														// Data Cache Enable
    | FLASH_ACR_PRFTEN													// Prefetch Enable
  );

  // Configures AHB, APB1 and APB2
  RCC->CFGR = ((0b0000 << RCC_CFGR_HPRE_Pos)		// - /1 (No clock division)
    | (0b101 << RCC_CFGR_PPRE1_Pos)							// - /4 (Divide APB1 by 4)
    | (0b100 << RCC_CFGR_PPRE2_Pos)							// - /2 (Divide APB2 by 2)
  );	

  // Configures the PLL
  RCC->PLLCFGR = ((180 <<	RCC_PLLCFGR_PLLN_Pos)	// - x180 (Source clock x180)
    | (0b000100 << RCC_PLLCFGR_PLLM_Pos)				// - /4 (PLLM Divide by 4)
    | (0b00 << RCC_PLLCFGR_PLLP_Pos)						// - /2 (PLLP Divide by 2)
    | (0b0010 << RCC_PLLCFGR_PLLQ_Pos)					// - /4 (PLLQ Divide by 2)
    | RCC_PLLCFGR_PLLSRC												// PLL Source: HSE
  );

  // Enables the PLL and waits for it to be stable.
  RCC->CR |= RCC_CR_PLLON;											// Enable PLL
  while (!(RCC->CR & RCC_CR_PLLRDY));						// Wait for PLL to be ready

  // Use PLL as mainclock source.
  RCC->CFGR |= (0b10 << RCC_CFGR_SW_Pos);       // Use PLL as system clock source.
  while (((RCC->CFGR & RCC_CFGR_SW) >> RCC_CFGR_SW_Pos) != 0b10);
}
