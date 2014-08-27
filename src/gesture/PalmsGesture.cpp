#include "PalmsGesture.h"

using namespace std;
using namespace ci;
using namespace ci::app;
using namespace LeapMotion;

float _diff;
float _prevValue = -10;

PalmsGesture::PalmsGesture()	
{
	
}

string PalmsGesture::checkGesture(const Leap::HandList& hands)
{
	string gesture_out = "NONE";

	if (hands.count() !=2 )
	{
		if (gestureBegin)
		{
			gestureBegin = false;
			gesture_out = "LOST_TWO_HANDS";
		}
		return gesture_out;
	}
	else
	{
		gestureBegin = true;
		float ddiff =   math<float>::abs(hands[0].palmPosition().x - hands[1].palmPosition().x); 

		_diff = ddiff- _prevValue;
		_prevValue = ddiff;	
	}

	if (_diff>2.3)
	{
			gesture_out = "BIGGER";	
			console()<<gesture_out<<std::endl;
	}
	else if (_diff<-2.3)
	{
			gesture_out = "SMALLER";	
			console()<<gesture_out<<std::endl;
	}	

	return gesture_out;	
}