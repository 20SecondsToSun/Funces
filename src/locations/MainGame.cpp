#include "MainGame.h"
#include "Params.h"
using namespace ci;
using namespace ci::app;

MainGame MainGame::MainGameState;

LeapController* MainGame::leap	= LeapController::Instance();

void MainGame::Setup()
{
	logoTexture			= AssetManager::getInstance()->getTexture( "images/logos.png" );	
	helloTexture		= AssetManager::getInstance()->getTexture( "images/HELLO.png" );

	gameModeID = MainGameDefaults::gameModeHEAD;

	headRef = new Head3D();
	faceRef = new myFaceScreen::Face();
	headRef->setup();
	faceRef->setup();
}

void MainGame::Init( LocationEngine* game)
{	
	makeScreenShotAndGo = false;
	_game = game;	

	changeGameMode();	
	
	timeline().apply( &alphaHelloTex,1.0f, 0.0f, 0.8f, EaseOutQuad() );

	leap->resetInitParams();	

	drawChangeModeButton.setState(gameModeID);
}
void MainGame::Shutdown()
{
	
}
void MainGame::Cleanup()
{	
	if (gameMode)
		gameMode->clean();	
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

	#ifdef DEBUG

	if (event.isLeftDown())
	{
		if (drawChangeModeButton.isDown(event.getPos()))
		{
			changeState();
			changeGameMode();
		}
		else
		{
			//gameMode->changeItem();
			makeScreenShotAndGo = true;
		}
	}

	#endif
}
void MainGame::changeState( )
{
	if (gameModeID  == MainGameDefaults::gameModeHEAD)
		gameModeID = MainGameDefaults::gameModeFACE;
	else
		gameModeID = MainGameDefaults::gameModeHEAD;

	drawChangeModeButton.setState( gameModeID);
}

void MainGame::HandleEvents( LocationEngine* game )
{

}

void MainGame::Update(LocationEngine* game) 
{	
	gameMode->update();

	std::string gesture = leap->getLastGestureName();	
	
	if (gesture == leapGestures::TWO_FINGERS)
	{
		makeScreenShotAndGo = true;
	}
	else if (gesture == leapGestures::ONE_FINGER)
	{
		changeState();
		changeGameMode();
	}
	else if  (gesture == leapGestures::SWIPE_LEFT || gesture == leapGestures::SWIPE_RIGHT)
	{
		gameMode->changeItem();
	}
	else if  (gesture == leapGestures::BIGGER || gesture == leapGestures::SMALLER)
	{
		if (gameModeID == MainGameDefaults::gameModeHEAD)		
			gameMode->changeSize(gesture);		
	}

	KinectAdapter* kinect = KinectAdapter::Instance();
	for (int i = 0; i < kinect->getHandsPosition().size(); i++)
	{
		//gl::drawSolidCircle(kinect->getHandsPosition()[i], 10);
		if ((kinect->getHandsPosition()[i].x > kinect->getColorResolutionRectf().getWidth() - 200) &&  
			(kinect->getHandsPosition()[i].y < 200))
		{
			changeState();
			changeGameMode();
			kinect->handsSleep(2);
		}
	}	
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

	if (alphaHelloTex >0.f && makeScreenShotAndGo == false)
	{
		Rectf centeredRect = Rectf( helloTexture->getBounds() ).getCenteredFit( getWindowBounds(),true );
		gl::color(ColorA(1.f, 1.f, 1.f, alphaHelloTex));
		gl::draw( *helloTexture, centeredRect);		
	}	

	if (makeScreenShotAndGo == false)
		drawChangeModeButton.draw();
	else
	{
		gotoReadyScreen();
	}

	gl::disableAlphaBlending();
}

void MainGame::gotoReadyScreen() 
{
	ScreenshotHolder::screenshot  = copyWindowSurface();
	_game->ChangeState(ReadyScreen::Instance());
}

void MainGame::changeGameMode() 
{
	if (gameMode)
		gameMode->clean();

	if (gameModeID == MainGameDefaults::gameModeHEAD)
	{
		gameMode = headRef;
		gameMode->init();
	}
	else if (gameModeID == MainGameDefaults::gameModeFACE)
	{
		gameMode = faceRef;
		gameMode->init();
	}	
}