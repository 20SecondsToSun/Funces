#pragma once
#include "cinder/app/AppBasic.h"
#include "cinder/Timer.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Timeline.h"
#include "Cinder-LeapMotion.h"

class Gesture 
{	  
  public:
	Gesture();
	void				init();  
	virtual std::string	checkGesture(const Leap::Hand& hand, int count);

	bool gestureContinue;
	bool gestureBegin;
	bool gestureEnd;	
	int handID ;
	float gestureTime ;

	ci::Timer				gestureTimer;
};