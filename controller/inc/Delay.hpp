/*
	>> Copyright Luke A.C.A. Rieff & Rien Dumore - Project-D ROC Nijmegen
*/

#include "inc.hpp"

#pragma once

class Delay {
public:
	static void Init (void) noexcept;
	static void Us (uint16_t n) noexcept;
	static void Ms (uint16_t n) noexcept;
	static void S (uint16_t n) noexcept;
};