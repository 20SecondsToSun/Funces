#include "MainGame.h"


using namespace ci;
using namespace ci::app;

MainGame MainGame::MainGameState;

void MainGame::Setup()
{
	logoTexture			= AssetManager::getInstance()->getTexture( "images/logos.png" );	
	helloTexture		= AssetManager::getInstance()->getTexture( "images/HELLO.png" );
}

void MainGame::Init( LocationEngine* game)
{	
	_game = game;

	gameMode = new Head3D();
	gameMode->init();
	
	timeline().apply( &alphaHelloTex,1.0f, 0.0f, 0.8f, EaseOutQuad() );
}

void MainGame::Shutdown()
{
	
}

void MainGame::Cleanup()
{
	gameMode->clean();
	delete gameMode;
}
void MainGame::Pause()
{
	
}
void MainGame::Resume()
{
	
}

void MainGame::MouseEvents( LocationEngine* game )
{
	MouseEvent event = game->getMouseEvent();	

	if(game->isAnimationRunning()) return;	
	
	gotoReadyScreen();
}


void MainGame::HandleEvents( LocationEngine* game )
{

}

void MainGame::Update(LocationEngine* game) 
{	
	gameMode->update();
}

void MainGame::Draw(LocationEngine* game) 
{
	gameMode->draw();

	gl::enableAlphaBlending();

	gl::pushMatrices();			
		float _scaleY = float(getWindowHeight())/float(logoTexture->getBounds().getHeight());
		gl::scale(_scaleY, _scaleY);
		gl::draw( *logoTexture);	
	gl::popMatrices();	

	if (alphaHelloTex >0.f)
	{
		Rectf centeredRect = Rectf( helloTexture->getBounds() ).getCenteredFit( getWindowBounds(),true );
		gl::color(ColorA(1.f, 1.f, 1.f, alphaHelloTex));
		gl::draw( *helloTexture, centeredRect);		
	}	

	gl::disableAlphaBlending();
}

void MainGame::gotoReadyScreen() 
{
	ScreenshotHolder::screenshot  = copyWindowSurface();
	_game->ChangeState(ReadyScreen::Instance());
}