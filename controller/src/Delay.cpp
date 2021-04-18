#include "Delay.hpp"
#include "Sysclock.hpp"
#include "peripherals/Watchdog.hpp"

void Delay::Init (void) noexcept {
	RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;
	TIM8->PSC = APB2_TIMER;
}

void Delay::Us (uint16_t n) noexcept {
	Watchdog::Kick ();
	TIM8->CNT = 0;
	TIM8->ARR = n;
	TIM8->CR1 = TIM_CR1_CEN | TIM_CR1_OPM;
	while (TIM8->CR1 & TIM_CR1_CEN);
}

void Delay::Ms (uint16_t n) noexcept {
	for (uint16_t i = 0; i < n; ++i) {
		Delay::Us(1000);
	}
}

void Delay::S (uint16_t n) noexcept {
	for (uint16_t i = 0; i < n; ++i) {
		Delay::S(1000);
	}
}
