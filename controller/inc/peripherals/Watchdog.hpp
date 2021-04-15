/*
	>> Copyright Luke A.C.A. Rieff & Rien Dumore - Project-D ROC Nijmegen
*/

#include "inc.hpp"

#pragma once

class Watchdog {
public:
	/// Initializes the watchdog.
	static void Init (void) noexcept;

	/// Kicks the watchdog.
	static void Kick (void) noexcept;
};
