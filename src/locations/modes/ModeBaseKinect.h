#pragma once

#include "KinectAdapter.h"
#include "cinder/app/AppNative.h"
#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"

class ModeBaseKinect
{
public:
	virtual void draw() = 0;
	virtual void update() = 0;
	virtual void init() = 0;
	virtual void clean() = 0;	
	virtual void save() = 0;	
	virtual void setup() = 0;
	virtual void changeItem() = 0;
	virtual void changeSize(std::string value) = 0;	

	static KinectAdapter*		kinect;

protected:
	bool						isCleaned;


};