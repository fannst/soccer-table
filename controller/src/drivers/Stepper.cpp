#include "drivers/Stepper.hpp"

Stepper::Stepper(uint8_t mf, uint8_t pu, uint8_t dir,
			GPIO_TypeDef *gpio, TIM_TypeDef *tim,
			uint16_t minSPS, uint16_t maxSPS, uint16_t spsInc,
			StepperTim_t timing) noexcept:
				m_MF(mf), m_PU(pu), m_DIR(dir),
				m_GPIO(gpio), m_TIM(tim),
				m_Pos(0),
				m_MinSPS(minSPS), m_MaxSPS(maxSPS), m_SPSInc(spsInc % 2 == 0 ? spsInc : spsInc + 1),
				m_Dir(Forward),
				m_Timing(timing),
				m_IsMoving(false),
				m_Enabled(false),
				m_AutoEnableDisabled(false)
			{
	memset(&m_COP, 0, sizeof (StepperOp_t));
}

void Stepper::init() noexcept {
	//
	// Configures the GPIO ( Modes )
	//

	// Resets all the pin modes inside the specified
	//  GPIO's mode register.
	m_GPIO->MODER &= ~((0x3 << (m_MF * 2))
			| (0x3 << (m_PU * 2))
			| (0x3 << (m_DIR * 2)));

	// Makes all the pins output.
	m_GPIO->MODER |= ((0x1 << (m_MF * 2))
			| (0x1 << (m_PU * 2))
			| (0x1 << (m_DIR * 2)));

	//
	// Configures the GPIO (Pull Up/Down)
	//

	// Resets the PUPDR value for all pins.
	m_GPIO->PUPDR &= ~((0x3 << (m_MF * 2))
			| (0x3 << (m_PU * 2))
			| (0x3 << (m_DIR * 2 )));

	// Makes all the pins pull-down.
	m_GPIO->PUPDR |= ((0x2 << (m_MF * 2))
			| (0x2 << (m_PU * 2))
			| (0x2 << (m_DIR * 2)));

	// Sets the default values for the pins ( disabled, no pulse, forward )
	m_GPIO->ODR &= ~((0x1 << m_MF));
	m_GPIO->ODR |= ((0x1 << m_PU)
			| (0x1 << m_DIR));

	//
	// Configures the GPIO (OSPEED)
	//

	// Makes pulse use high-speed clock
	m_GPIO->OSPEEDR |= (0x3 << (m_PU * 2));

	//
	// Configures the timer
	//

	m_TIM->PSC = m_Timing.prescalar;
	m_TIM->DIER = TIM_DIER_UIE;
}

void Stepper::setDir(StepperDir_t dir) noexcept {
	if (dir == Forward) {
		m_GPIO->ODR |= (0x1 << m_DIR);
	} else {
		m_GPIO->ODR &= ~(0x1 << m_DIR);
	}

	m_Dir = dir;
}

void Stepper::setAuto(bool enabled) noexcept {
	m_AutoEnableDisabled = enabled;
}

void Stepper::enable() noexcept {
	m_GPIO->ODR |= (0x1 << m_MF);
}

void Stepper::disable() noexcept {
	m_GPIO->ODR &= ~(0x1 << m_MF);
}

void Stepper::move(int32_t newPos) noexcept {
	if (m_IsMoving) {
		return;
	}

	// Sets moving to true, and sets the new target position
	//  to the one specified as argument.
	m_IsMoving = true;
	m_COP.targetPos = newPos;

	//
	// Prepare for movement...
	//

	// Calculates the number of steps to perform in total, and
	//  sets the direction we're going to move in.
	if (newPos > m_Pos) {
		m_COP.totalSteps = newPos - m_Pos;
		setDir(Forward);
	} else {
		m_COP.totalSteps = m_Pos - newPos;
		setDir(Backward);
	}

	//
	// Prepare for speedup and slow-down calculations..
	//

	// Rounds the number of steps, if not even.. This will
	//  prevent possible calculation errors.
	if ((m_COP.totalSteps % 2) != 0) {
		++m_COP.totalSteps;

		if (m_Dir == Forward) {
			++newPos;
		} else {
			--newPos;
		}
	}

	//
	// Calculate speedup and slow-down...
	//

	uint32_t rampStepSize = (m_MaxSPS - m_MinSPS) / m_SPSInc;

	if (rampStepSize > (m_COP.totalSteps / 2)) {
		m_COP.stopSpeedupAt = m_COP.startSlowdownAt = m_COP.totalSteps / 2;
	} else {
		m_COP.stopSpeedupAt = rampStepSize;
		m_COP.startSlowdownAt = m_COP.totalSteps - rampStepSize;
	}

	//
	// Enables the timer, and starts the movement ...
	//

	// Sets done steps to zero, and sets the current SPS.
	m_COP.doneSteps = 0;
	m_COP.currentSPS = m_MinSPS;

	// Enables the stepper (if auto)
	if (m_AutoEnableDisabled) {
		enable();
	}

	// Configures the timer.
	m_TIM->CNT = 0;
	m_TIM->ARR = 1000000 / static_cast<uint32_t>(m_COP.currentSPS);
	m_TIM->CR1 = TIM_CR1_CEN;
}

/**
 * Locks the current thread until motor stops moving.
 */
void Stepper::lockThread() noexcept {
	while (m_IsMoving);
}
