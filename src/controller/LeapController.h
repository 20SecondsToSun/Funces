#pragma once
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Timeline.h"
#include "cinder/Timer.h"
#include "cinder/Rand.h"
#include "cinder/Camera.h"

#include "Cinder-LeapMotion.h"
#include "PalmsGesture.h"
#include "TwoFingers.h"
#include "OneFinger.h"

namespace leapGestures
{
	const std::string    TWO_FINGERS   =  "TWO_FINGERS";
	const std::string    ONE_FINGER    =  "ONE_FINGER";
	const std::string    SWIPE_LEFT    =  "SWIPE_LEFT";
	const std::string    SWIPE_RIGHT   =  "SWIPE_RIGHT";
	const std::string    BIGGER		   =  "BIGGER";
	const std::string    SMALLER	   =  "SMALLER";
	const std::string    NONE		   =  "NONE";
	const std::string    HAND_OVER	   =  "HAND_OVER";	
	const std::string    CIRCLE	       =  "CIRCLE";		
}

class LeapController 
{	  
  public:
	
	void						Setup(); 
	void						Update();
	void						Shutdown();	

	void						resetInitParams();
	std::string					getLastGestureName();

	void						sleep(int seconds);

	static LeapController* Instance() 
	{
		return &LeapControllerState;
	};

protected:
	LeapController(){};
	
 private:
	 static LeapController		LeapControllerState;

	 void 						onFrame( Leap::Frame frame );	
	 void						setGestureAllowTimer();

	 ci::Vec2f					warpVector( const Leap::Vector& v );
	
	 bool						GESTURE_ALLOW;	
	 int						GESTURE_ALLOW_TIMER;

	  // Leap
	 LeapMotion::DeviceRef		leapDevice;
	 Leap::Frame				leapFrame;

	 TwoFingers					twoFingersGesture;	
	 OneFinger					oneFingersGesture;
	 PalmsGesture				palmsWideGesture;

	 ci::Timer					gestureTimer;

	 bool						canLeap;
	 std::string				lastGestureName;
};

inline LeapController&	leap() { return *LeapController::Instance(); };