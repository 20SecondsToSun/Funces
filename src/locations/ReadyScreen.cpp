#include "ReadyScreen.h"

using namespace ci;
using namespace ci::app;
using namespace ReadyScreenDefaults;

ReadyScreen ReadyScreen::ReadyScreenState;
LeapController* ReadyScreen::leap	= LeapController::Instance();

void ReadyScreen::Setup()
{
	for (int i = 1; i <= FONS_SCREENS_NUM; i++)	
		fonImgVector.push_back(AssetManager::getInstance()->getTexture("images/fons/bg"+to_string(i)+".jpg"));
	
	qrCode.setup();
}

void ReadyScreen::Init( LocationEngine* game)
{
	currentFon				= Rand::randInt(FONS_SCREENS_NUM);	
	screnshot				= ScreenshotHolder::screenshot;
	ScreenshotHolder::resize();

	alphaFade = 0.0f;
	
	qrCode.init();	
	leap->resetInitParams();	

	serverSignalCon = server.serverHandler.connect( 
			boost::bind(&ReadyScreen::serverSignal, this) 
		);

	
	isServerFinishHisWork =false;

	#ifdef DEBUG
		isServerFinishHisWork = true;
	#endif

	state = FLASH;	
	_game = game;

	timeline().apply( &alphaFlash, 1.0f, 0.0f, 0.7f, EaseOutExpo() ).delay(0.2f).finishFn( bind( &ReadyScreen::animationFlashFinish, this )  );	
}

void ReadyScreen::Shutdown()
{
	serverSignalCon.disconnect();
}

void ReadyScreen::Cleanup()
{
	serverSignalCon.disconnect();
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

	#ifdef DEBUG

	if (event.isLeftDown())
	{
		if (backToStart.isDown(event.getPos()))
		{
			game->ChangeState(ScanFace::Instance());
		}
		else game->ChangeState(MainGame::Instance());
	}

	#endif

	if (event.isLeftDown())
	{
		gotoLocation = Instruction::Instance();
		timeline().apply( &alphaFade, 0.0f, 1.0f, 0.3f, EaseOutQuad() ).finishFn( bind( &ReadyScreen::animationOutFinish, this ) );	
	}

}

void ReadyScreen::HandleEvents( LocationEngine* game )
{
}

void ReadyScreen::Update(LocationEngine* game) 
{
	if (!isServerFinishHisWork) return;

	std::string gesture  = leap->getLastGestureName();	

	if ( gesture== leapGestures::TWO_FINGERS)
	{
		gotoLocation = MainGame::Instance();
		timeline().apply( &alphaFade, 0.0f, 1.0f, 0.8f, EaseOutQuad() ).finishFn( bind( &ReadyScreen::animationOutFinish, this ) );		
	}
	else if (gesture == leapGestures::ONE_FINGER)
	{
		gotoLocation = Instruction::Instance();
		timeline().apply( &alphaFade, 0.0f, 1.0f, 0.8f, EaseOutQuad() ).finishFn( bind( &ReadyScreen::animationOutFinish, this ) );	
	}	
}

void ReadyScreen::Draw(LocationEngine* game) 
{
	gl::clear(Color::white());
	gl::enableAlphaBlending();

	Rectf centeredRect = Rectf( fonImgVector[currentFon]->getBounds() ).getCenteredFit( getWindowBounds(),true );
	//console()<<"  alphaFade  "<<alphaFade<<endl;
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

			if (alphaFade <= 0)
				qrCode.draw();	

			gl::color(ColorA(0, 0, 0, alphaFade));
			gl::drawSolidRect(Rectf(0, 0, getWindowWidth(), getWindowHeight()));
			gl::color(Color(1, 1, 1));

			break;

		case LOAD_TO_SERVER:
			break;
	}	

	#ifdef DEBUG
		backToStart.draw();
	#endif

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
	#ifndef DEBUG
		server.sendPhoto();
	#endif
}

void ReadyScreen::serverSignal()
{
	if (server.isResponseOK())
	{
		qrCode.setTextureString(server.getBuffer());
		qrCode.setLink(server.getLink());
		qrCode.isReady = true;
		qrCode.isError = false;
	}
	else
	{
		qrCode.isError = true;
	}

	isServerFinishHisWork = true;	
}

void ReadyScreen::animationOutFinish()
{
	_game->ChangeState(gotoLocation);
}