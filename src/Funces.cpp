#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "LocationEngine.h"
#include "MainGame.h"
#include "KinectAdapter.h"
#include "HeadHelper.h"
#include "ReadyScreen.h"

#define DEBUGMODE

using namespace ci;
using namespace ci::app;
using namespace std;

class Funces : public AppNative {
  public:
	void setup();	
	void keyDown( ci::app::KeyEvent event );
	void update();
	void draw();
	void shutdown();

private:
	LocationEngine			 game;
};

void Funces::setup()
{	
	setWindowSize(1920, 1080);//640*2, 480*2);

	HeadHelper::loadHeads();

	ReadyScreen::Instance()->Setup();
	MainGame::Instance()->Setup();
	KinectAdapter::Instance()->Setup();

	game.Init("init",getWindow());
	game.ChangeState(MainGame::Instance());	
}

void Funces::update()
{
	game.Update();
	KinectAdapter::Instance()->update();
}

void Funces::draw()
{
	game.Draw();
}

void Funces::keyDown( KeyEvent event )
{
	switch ( event.getCode() ) {
	case KeyEvent::KEY_f:
		setFullScreen( !isFullScreen() );
		break;
	case KeyEvent::KEY_q:
		quit();
		break;
	}
}
void Funces::shutdown(  )
{
	KinectAdapter::Instance()->Shutdown();
}

CINDER_APP_NATIVE( Funces, RendererGl )