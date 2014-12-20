#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "LeapController.h"
#include "LocationEngine.h"
#include "MainGame.h"
#include "KinectAdapter.h"
#include "HeadHelper.h"
#include "ReadyScreen.h"
#include "ScanFace.h"
#include "FaceController.h"
#include "Instruction.h"
#include "HowToUse.h"
#include "Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Funces : public AppNative 
{
public:
	void setup();	
	void keyDown(ci::app::KeyEvent event );
	void update();
	void draw();
	void shutdown();	

private:
	LocationEngine game;
};

void Funces::setup()
{	
	setWindowSize(1920, 1080);

	Instruction::Instance()->Setup();
	HowToUse::Instance()->Setup();
	HeadHelper::loadHeads();
	//FaceController::loadFaces();

	ReadyScreen::Instance()->Setup();
	MainGame::Instance()->Setup();
	//ScanFace::Instance()->Setup();
	
	kinect().Setup();
	leap().Setup();

	game.Init("init", getWindow());
	game.ChangeState(Instruction::Instance());	
}

void Funces::update()
{		
	game.Update();
}

void Funces::draw()
{
	game.Draw();
}

void Funces::keyDown( KeyEvent event )
{
	switch ( event.getCode() )
	{
	case KeyEvent::KEY_f:
		setFullScreen( !isFullScreen() );
		break;

	case KeyEvent::KEY_q:
		quit();
		break;

	case KeyEvent::KEY_1:
		KinectAdapter::oneHead = true;
		break;

	case KeyEvent::KEY_2:
		KinectAdapter::oneHead = false;
		break;
	}
}

void Funces::shutdown(  )
{		
}

CINDER_APP_NATIVE( Funces, RendererGl )