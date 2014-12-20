#include "TwoFingers.h"

using namespace std;
using namespace ci;
using namespace ci::app;
using namespace LeapMotion;

string TwoFingers::checkGesture(const Leap::Hand& hand, int count)
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

		vector<Vec3f> knuckles;
		const Leap::FingerList fingers = hand.fingers();
		for ( Leap::FingerList::const_iterator fingerIter = fingers.begin(); fingerIter != fingers.end(); ++fingerIter ) {
			const Leap::Finger& finger = *fingerIter;

			if (finger.type() == 0 || finger.type() == 3|| finger.type() == 4) 
				continue;			

		//	for ( int32_t i = 3; i < 4; ++i ) {
		//		const Leap::Bone& bone = finger.bone( (Leap::Bone::Type)i );
		//		Vec3f center	= LeapMotion::toVec3f( bone.center() );
		//		Vec3f direction	= LeapMotion::toVec3f( bone.direction() );
		//		Vec3f start		= center - direction * bone.length() * 0.5f;
		//		Vec3f end		= center + direction * bone.length() * 0.5f;
		//		knuckles.push_back( end );	
		//	}
		}

	//	console()<<"  knuckles.size() "<<knuckles.size()<<endl;
		float dist = 0;
		if (knuckles.size() == 2)
				dist = abs((knuckles[0] - knuckles[1]).length());








		console()<<"try gesture!!!!!!!!!   "<<gestureTime<<endl;




		
		
		if (/*dist> 50 &&*/ pointables <= 3 && pointables > 1 && avrgVel< 50 && velX < 50 && velY < 50 && velZ < 50)// && degree>-30 && degree<30 
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

						gesture_out = "TWO_FINGERS";	

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

void TwoFingers::initFlags()
{
	gestureTime = 0.9;
	gestureContinue = false;
	gestureBegin = false;
	gestureEnd = false;
	gestureTimer.stop();	
}