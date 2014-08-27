#pragma once
#include "Gesture.h"

class PalmsGesture : public Gesture {

	public:
	PalmsGesture();
	std::string	checkGesture(const Leap::HandList&);
	void initFlags();
};