/*
	>> Copyright Luke A.C.A. Rieff & Rien Dumore - Project-D ROC Nijmegen
*/

#include "inc.hpp"

#pragma once

#define APB1_PERIPHERAL 	(45)
#define APB1_TIMER				(90)
#define APB2_PERIPHERAL		(90)
#define APB2_TIMER				(180)
#define MHZ(A) 						(A * 1000 * 1000)

class Sysclock {
public:
	static void Init (void) noexcept;
};