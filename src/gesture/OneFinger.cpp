#include "OneFinger.h"

using namespace std;
using namespace ci;
using namespace ci::app;
using namespace LeapMotion;

string OneFinger::checkGesture(const Leap::Hand& hand, int count)
{
	string gesture_out = "NONE";

	if (count == 1)
	{//hand.fingers().extended().count()
		auto pointables = hand.fingers().extended().count();
		float degree = -57.2957795 *hand.palmNormal().roll();		
		float velX = math<float>::abs( hand.palmVelocity().x );
		float velY = math<float>::abs( hand.palmVelocity().y );
		float velZ = math<float>::abs( hand.palmVelocity().z );			
		float avrgVel = (velX + velY + velZ) / 3;
		
		
		if (pointables == 0 && degree>-30 && degree<30 && avrgVel< 50)
		{
			if (gestureBegin == false)
			{
				gestureBegin = true;
				gestureEnd = false;
				handID = hand.id();
				gestureTimer.start();
			}
			else
			{
				if (handID == hand.id())
				{
					if (gestureTime <= gestureTimer.getSeconds() && !gestureTimer.isStopped())
					{
						gestureContinue = false;
						gestureBegin = false;
						gestureEnd = true;
						gestureTimer.stop();					

						gesture_out = "ONE_FINGER";						
						return gesture_out;
					}
				}
				else
				{
					initFlags();						
				}
			}
		}
		else
		{
			initFlags();			
		}
	}
	else
	{
		initFlags();	
	}

	return gesture_out;
}

void OneFinger::initFlags()
{
	gestureContinue = false;
	gestureBegin = false;
	gestureEnd = false;
	gestureTimer.stop();	
}