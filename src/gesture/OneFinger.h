#pragma once
#include "Gesture.h"

class OneFinger : public Gesture {

	public:	
	std::string	checkGesture(const Leap::Hand& hand, int count);
	void initFlags();
};