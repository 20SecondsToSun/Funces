#pragma once
#include "Gesture.h"

class TwoFingers : public Gesture {

	public:
	std::string	checkGesture(const Leap::Hand& hand, int count);
	void initFlags();
};