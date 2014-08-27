#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/Timer.h"
#include "cinder/Timeline.h"

#include "Location.h"

#include "Head3DScreen.h"
#include "FaceScreen.h"

#include "ModeBaseKinect.h"
#include "ReadyScreen.h"
#include "KinectAdapter.h"
#include "AssetsManager.h"
#include "ScreenshotHolder.h"
#include "ChangeModeButton.h"
#include "LeapController.h"


using namespace ci;
using namespace ci::app;
using namespace gl;
using namespace std;

namespace MainGameDefaults
{	
	const  std::string	 gameModeHEAD = "HEAD";
	const  std::string	 gameModeFACE = "FACE";
}

class MainGame : public Location
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

	static MainGame* Instance() {
		return &MainGameState;
	}

protected:
	MainGame() { }

private:
	void					MouseEvents( LocationEngine* game );

	LocationEngine*			_game;
	static MainGame			MainGameState;

	static LeapController*	leap;

	ModeBaseKinect*			gameMode;	

	Texture*				logoTexture, *helloTexture;

	ci::Anim<float>			alphaHelloTex;

	void					gotoReadyScreen();	
	void					changeGameMode();

	ChangeModeButton		drawChangeModeButton ;

	Head3D*					headRef;
	myFaceScreen::Face*     faceRef;

	bool					makeScreenShotAndGo;

	

	void					changeState( );

	std::string				gameModeID;
};