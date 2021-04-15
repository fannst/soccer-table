/*
	>> Copyright Luke A.C.A. Rieff & Rien Dumore - Project-D ROC Nijmegen
*/


#include <cstdint>
#include <cstring>
#include <algorithm>

#include "inc.hpp"

#pragma once

typedef enum {
	Forward,
	Backward
} StepperDir_t;

typedef struct {
	uint32_t 		stopSpeedupAt, startSlowdownAt;
	uint32_t 		totalSteps, doneSteps;
	int32_t 		targetPos;
	uint16_t		currentSPS;
} StepperOp_t;

typedef struct {
	uint16_t prescalar, oscilator;
} StepperTim_t;

#define STEPPER_ISR(NAME, STEPPER) \
	extern "C" void NAME(void) \
	{ \
		/* Checks if the event is update ( overflow ) */ \
		if (STEPPER.m_TIM->SR & TIM_SR_UIF) \
		{ \
			/* Clears the update vent flag */ \
			STEPPER.m_TIM->SR &= ~(TIM_SR_UIF); \
			\
			/* Checks if we should stop */ \
			if (STEPPER.m_COP.doneSteps >= STEPPER.m_COP.totalSteps) \
			{ \
				if (STEPPER.m_AutoEnableDisabled) STEPPER.disable(); \
				STEPPER.m_IsMoving = false; \
				STEPPER.m_COP.currentSPS = 0; \
				STEPPER.m_TIM->CR1 = 0; \
				return; \
			} \
			\
			/* Sends the stepper pulse */ \
			STEPPER.m_GPIO->ODR &= ~(1 << STEPPER.m_PU); \
			for (uint16_t i = 0; i < 100; ++i) asm("NOP\n\t"); \
			STEPPER.m_GPIO->ODR |= (1 << STEPPER.m_PU); \
			\
			/* Checks if speed up required */ \
			if (STEPPER.m_COP.doneSteps < STEPPER.m_COP.stopSpeedupAt) \
			{ \
				STEPPER.m_COP.currentSPS += STEPPER.m_SPSInc; \
				STEPPER.m_TIM->ARR = 1000000 / STEPPER.m_COP.currentSPS; \
			} else if (STEPPER.m_COP.doneSteps >= STEPPER.m_COP.startSlowdownAt) \
			{ \
				STEPPER.m_COP.currentSPS -= STEPPER.m_SPSInc; \
				STEPPER.m_TIM->ARR = 1000000 / STEPPER.m_COP.currentSPS; \
			} \
			\
			/* Increments step, and updates the position */ \
			++STEPPER.m_COP.doneSteps; \
			if (STEPPER.m_Dir == Forward) ++STEPPER.m_Pos; \
			else --STEPPER.m_Pos; \
		} \
	}

class Stepper
{
public:
	Stepper(uint8_t mf, uint8_t pu, uint8_t dir,
			GPIO_TypeDef *gpio, TIM_TypeDef *tim,
			uint16_t minSPS, uint16_t maxSPS, uint16_t spsInc,
			StepperTim_t timing) noexcept;

	virtual void init() noexcept;

	void setDir(StepperDir_t dir) noexcept;
	void setAuto(bool enabled) noexcept;

	void enable() noexcept;
	void disable() noexcept;

	virtual void move(int32_t pos) noexcept;

	/**
	 * Locks the current thread until motor stops moving.
	 */
	void lockThread() noexcept;
public: /* Do not access please! */
	/* =================== */
	uint8_t 		m_MF,
					m_PU,
					m_DIR;
	/* =================== */
	GPIO_TypeDef*	m_GPIO;
	TIM_TypeDef*	m_TIM;
	/* =================== */
	int32_t 		m_Pos;
	uint16_t 		m_MinSPS, m_MaxSPS;
	uint16_t 		m_SPSInc;
	StepperDir_t 	m_Dir;
	/* =================== */
	StepperOp_t		m_COP;
	StepperTim_t	m_Timing;
	/* =================== */
	unsigned		m_IsMoving : 1;
	unsigned		m_Enabled : 1;
	unsigned		m_AutoEnableDisabled : 1;
};