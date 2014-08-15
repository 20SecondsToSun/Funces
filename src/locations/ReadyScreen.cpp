#include "ReadyScreen.h"
#include "MainGame.h"


using namespace ci;
using namespace ci::app;
using namespace ReadyScreenDefaults;

ReadyScreen ReadyScreen::ReadyScreenState;

void ReadyScreen::Setup()
{
	for (int i = 1; i <= FONS_SCREENS_NUM; i++)
	{
		fonImgVector.push_back(AssetManager::getInstance()->getTexture("images/fons/bg"+to_string(i)+".jpg"));
	}
	qrCode.setup();

}

void ReadyScreen::Init( LocationEngine* game)
{
	currentFon				= Rand::randInt(FONS_SCREENS_NUM);	
	screnshot = ScreenshotHolder::screenshot;

	state = FLASH;		
	timeline().apply( &alphaFlash, 1.0f, 0.0f, 0.7f, EaseOutQuad() ).delay(0.2f).finishFn( bind( &ReadyScreen::animationFlashFinish, this )  );	
	
}

void ReadyScreen::Shutdown()
{
	
}

void ReadyScreen::Cleanup()
{
	
}

void ReadyScreen::Pause()
{
	
}
void ReadyScreen::Resume()
{
	
}

void ReadyScreen::MouseEvents( LocationEngine* game )
{
	MouseEvent event = game->getMouseEvent();	

	if(game->isAnimationRunning()) return;	

	game->ChangeState(MainGame::Instance());
	
}

void ReadyScreen::HandleEvents( LocationEngine* game )
{
}

void ReadyScreen::Update(LocationEngine* game) 
{	
		
}

void ReadyScreen::Draw(LocationEngine* game) 
{
	gl::clear(Color::white());
	gl::enableAlphaBlending();

	Rectf centeredRect = Rectf( fonImgVector[currentFon]->getBounds() ).getCenteredFit( getWindowBounds(),true );
	switch (state)
	{
		case FLASH:

			gl::draw(screnshot);
			gl::color(ColorA(1.f,1.f, 1.f, alphaFlash));
			gl::drawSolidRect(getWindowBounds());
			gl::color(Color::white());
			break;

		case SCREESHOT_ANIM:					
			gl::draw(*fonImgVector[currentFon], centeredRect);
			gl::pushMatrices();	
				gl::scale(startPhotoScale);
				gl::translate(startPhotoXY);	
				gl::draw(screnshot);
			gl::popMatrices();

			qrCode.draw();			

			break;

		case LOAD_TO_SERVER:
			break;
	}	

	gl::disableAlphaBlending();
}

void ReadyScreen::animationFlashFinish() 
{
	state = SCREESHOT_ANIM;		

	timeline().apply( &startPhotoXY,Vec2f(0.0f, 0.0f), Vec2f(330.f, 215.f), 0.6f, EaseOutQuad() ).finishFn( bind( &ReadyScreen::animationLastFinish, this )  );	
	timeline().apply( &startPhotoScale, Vec2f(1.f, 1.f),Vec2f(1130.0f/1920.0f, 1130.0f/1920.0f), 0.6f, EaseOutQuad() );
	qrCode.initAnim();	
}

void ReadyScreen::animationLastFinish() 
{
	server.sendPhoto();
}