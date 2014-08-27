#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/Timer.h"

#include "Location.h"
#include "LeapController.h"
#include "KinectAdapter.h"

using namespace ci;
using namespace ci::app;
using namespace gl;
using namespace std;

namespace instructionDefaults
{
	enum states {INIT, CHECK_HAND_ON_LEAP,  WAIT, FACE_DETECTED, HAND_DETECTED };
}

class Instruction : public Location
{

public:
	void Setup();
	void Init( LocationEngine* game);
	void Cleanup();

	void Pause();
	void Resume();

	void HandleEvents(LocationEngine* game);
	void Update(LocationEngine* game);
	void Draw(LocationEngine* game);

	static Instruction* Instance() {
		return &InstructionState;
	}

protected:
	Instruction() { };

private:
	void					 MouseEvents( LocationEngine* game );

	LocationEngine*			_game;
	static Instruction		InstructionState;

	int						state;

	void					animationInitFinish() ;
	void					animationFadeOutFinish() ;
	void					animationFinish();
	ci::Anim<float>			alphaFade;
	
	ci::Anim<ci::Vec2f>		coord1, coord2;

	ci::gl::Texture			fon1,fon2, slide1,slide2, how;	
		
	void					setGraphObject(ci::gl::Texture tex, ci::Vec2f _value );
	ci::Vec2f				getCenterOfScreen(ci::gl::Texture tex );	
	ci::Vec2f				_scale;

	static KinectAdapter*	kinect;
	static LeapController*	leap;	
};