#include "LeapController.h"

using namespace std;
using namespace ci;
using namespace ci::app;

using namespace leapGestures;
using namespace LeapMotion;

LeapController LeapController::LeapControllerState;

void LeapController::Setup()
{
	resetInitParams();
	
	if (leapDevice && leapDevice->isConnected()) return;
	
	leapDevice 	= Device::create();
	leapDevice->connectEventHandler( &LeapController::onFrame, this );
	
	// Enable all gesture types
	Leap::Controller* controller = leapDevice->getController();
	controller->enableGesture( Leap::Gesture::Type::TYPE_SWIPE );

	// Write gesture config to console
	Leap::Config config = controller->config();

	// Update config to make gestures easier
	config.setFloat( "Gesture.Swipe.MinLength", 45.0f );
	config.setFloat( "Gesture.Swipe.MinVelocity", 300.0f );	
}

void LeapController::resetInitParams()
{
	sleep(3);
}

void LeapController::sleep(int seconds)
{
	GESTURE_ALLOW = false;
	GESTURE_ALLOW_TIMER = seconds;
	gestureTimer.start();
}

string LeapController::getLastGestureName()
{
	return lastGestureName;
}

void LeapController::Update()
{	
	if (leapDevice->isConnected() == false) 
	{
		lastGestureName  = "NONE";
		return;
	}

	// Process hand data
	const Leap::HandList& hands = leapFrame.hands();	

	if (gestureTimer.isStopped() == false && gestureTimer.getSeconds()> GESTURE_ALLOW_TIMER)
	{
		GESTURE_ALLOW = true;
		gestureTimer.stop();
	}

	if (GESTURE_ALLOW)
	{
		string status = palmsWideGesture.checkGesture(hands) ;

		if ( status == BIGGER || status ==SMALLER)
		{
			lastGestureName  = status;
			return;
		}
		else if (status == "LOST_TWO_HANDS")
		{
			sleep(2);
			lastGestureName = NONE;
			return;
		}
	}

	const Leap::GestureList& gestures = leapFrame.gestures();
	
	if (GESTURE_ALLOW)
		for ( Leap::GestureList::const_iterator iter = gestures.begin(); iter != gestures.end(); ++iter ) 
		{		
			const Leap::Gesture& gesture	= *iter;
			Leap::Gesture::Type type		= gesture.type();		

			if ( type == Leap::Gesture::Type::TYPE_SWIPE ) 
			{
				// Cast to swipe gesture and read data
				const Leap::SwipeGesture& gesture = (Leap::SwipeGesture)*iter;

				ci::Vec2f a	=  warpVector(gesture.startPosition());
				ci::Vec2f b	=  warpVector(gesture.position());

				float x_diff =  math<float>::abs(b.x - a.x) ;
				float y_iff =  math<float>::abs(b.y - a.y) ;			

				if (y_iff > 50 || x_diff>50 )
				{
					sleep(1);

					string status;

					if ( b.x < a.x ) 
					{					
						status = SWIPE_LEFT;						
					}
					else
					{					
						status = SWIPE_RIGHT;								
					}
					lastGestureName = status;
					return;
				}				
			}
		}

	for ( Leap::HandList::const_iterator handIter = hands.begin(); handIter != hands.end(); ++handIter )
	{
		const Leap::Hand& hand = *handIter;		
		const Leap::PointableList& pointables = hand.pointables();

		if (GESTURE_ALLOW)
		{
			string status = twoFingersGesture.checkGesture(hand, hands.count());	
			if (status == TWO_FINGERS)
			{
				sleep(2);
				lastGestureName  = status;
				return;
			}
			status = oneFingersGesture.checkGesture(hand, hands.count());
			if (status == ONE_FINGER)
			{
				sleep(2);
				lastGestureName  = status;
				return;
			}
		}	
	}

	if (hands.count())
	{
		lastGestureName = HAND_OVER;
		return;
	}

	lastGestureName  = NONE;
}

void LeapController::onFrame( Leap::Frame frame )
{
	leapFrame = frame;
}

// Maps Leap vector to the screen in pixels
Vec2f LeapController::warpVector( const Leap::Vector& v )
{
	Vec3f result	= Vec3f::zero();
	if ( leapDevice ) {
		const Leap::Screen& screen = leapDevice->getController()->locatedScreens().closestScreen( v );
		
		result		= LeapMotion::toVec3f( screen.project( v, true ) );
	}
	result			*= Vec3f( getWindowSize(), 0.0f );
	result.y		= (float)getWindowHeight() - result.y;
	return result.xy();
}

void LeapController::Shutdown( )
{
}