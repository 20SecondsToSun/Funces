#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
#include "cinder/Timer.h"

#include "Location.h"
#include "KinectAdapter.h"
#include "FaceController.h"

#include "MainGame.h"
#include "Utils.h"
#include "cinder/Timeline.h"

using namespace ci;
using namespace ci::app;
using namespace gl;
using namespace std;

namespace faceDefaults
{
	const std::string    distanceHint     =  "œŒƒŒ…ƒ»“≈ ¡À»∆≈";
	const std::string    faceXYHint       =  "À»÷Œ ¬ ÷≈Õ“–!!!!!";
	const std::string    faceAngleHint    =  "√ŒÀŒ¬” Õ≈ Õ¿ ÀŒÕﬂ≈Ã!";
	const int		     scanSeconds      =   2;	
	const ci::Vec3f		 angleEpsilon	  = ci::Vec3f(20.0f, 20.0f, 20.0f);

	enum states {START_FADE_OUT_ANIM, SAVING_FACE, FADE_OUT_ANIM, SCANNING, WAIT, START_FADE_IN_ANIM, FADE_IN_ANIM };
}

class ScanFace : public Location
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

	static ScanFace* Instance() 
	{
		return &ScanFaceState;
	}

protected:
	ScanFace() { }

private:
	void					 MouseEvents( LocationEngine* game );

	LocationEngine*			_game;
	static ScanFace			ScanFaceState;
	int						state, currentSec;

	static KinectAdapter*	kinect;

	bool					isScanFinished;	

	FaceController			faceController;	

	void					saveFaceThread( );
	std::shared_ptr<std::thread>				saverThread;

	bool					isScanned;

	int						activeFaceID;

	bool					isPositionZOK, isPositionXYOK, isHeadAngleOK, readyToScreenshot;

	ci::Font				hintFont;

	ci::Timer				photoTimer;	

	void					setActiveFaceID() ;

	bool					isWrongConditionsForHeadScan();
	bool					isGoodConditionsForHeadScan();
	void					renderFaceToFbo();

	gl::Fbo					faceFbo;
	gl::Texture				faceTexture;

	void					drawFaceWiredMask();
	void					drawTextHints();
	void					makefaceScreenShot();
	void					animationFadeInFinish();
	void					animationFadeOutFinish();

	ci::Anim<float>			alphaFade;
};