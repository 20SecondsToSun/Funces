#include "Gesture.h"

using namespace std;
using namespace ci;
using namespace ci::app;
using namespace LeapMotion;

Gesture::Gesture()	
{
	
}

void Gesture::init()
{
	gestureContinue = false;
	gestureBegin = false;
	gestureEnd = false;
	handID = -1;
	gestureTime = 1.4f;
}

string Gesture::checkGesture(const Leap::Hand& hand, int count)
{	
	return "NONE";
}	