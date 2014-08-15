#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/Timer.h"
#include "cinder/Timeline.h"
#include "cinder/Rand.h"

#include "Location.h"

#include "ScreenshotHolder.h"
#include "AssetsManager.h"
#include "QRcode.h"
#include "Server.h"

using namespace ci;
using namespace ci::app;
using namespace gl;
using namespace std;

namespace ReadyScreenDefaults
{
	const int  FONS_SCREENS_NUM					    = 6;
	enum states { FLASH, SCREESHOT_ANIM, LOAD_TO_SERVER, BLOCK };
}



class ReadyScreen : public Location
{

public:
	void Setup();
	void Init( LocationEngine* game);
	void Cleanup();

	void Pause();
	void Resume();

	void Shutdown();

	void HandleEvents(LocationEngine* game);
	void Update(LocationEngine* game);
	void Draw(LocationEngine* game);

	static ReadyScreen* Instance() {
		return &ReadyScreenState;
	}

protected:
	ReadyScreen() { }

private:
	void							MouseEvents( LocationEngine* game );
	void							animationFlashFinish();
	void							animationLastFinish();

	LocationEngine*					_game;
	static ReadyScreen				ReadyScreenState;
	
	Texture*						logoTexture, *helloTexture;
	
	ci::Anim<float>					alphaHelloTex;
	std::vector< ci::gl::Texture*>  fonImgVector;
	int								currentFon;

	ci::Surface						screnshot;

	ReadyScreenDefaults::states		state;

	ci::Anim<float>					alphaFlash;
	ci::Anim<ci::Vec2f>				startPhotoScale, startPhotoXY;

	QRcode							qrCode;

	Server							server;
};