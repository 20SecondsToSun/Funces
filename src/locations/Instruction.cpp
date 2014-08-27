#include "Instruction.h"
#include "MainGame.h"

using namespace ci;
using namespace ci::app;
using namespace instructionDefaults;

Instruction Instruction::InstructionState;

KinectAdapter* Instruction::kinect	= KinectAdapter::Instance();
LeapController* Instruction::leap	= LeapController::Instance();

void Instruction::Setup()
{	
	fon1 = gl::Texture(loadImage(getAssetPath("rules/_fon1.png")));
	fon2 = gl::Texture(loadImage(getAssetPath("rules/_fon2.png")));
	
	slide1 = gl::Texture(loadImage(getAssetPath("rules/_slide1.png")));
	slide2 = gl::Texture(loadImage(getAssetPath("rules/_slide2.png")));
	
	how = gl::Texture(loadImage(getAssetPath("rules/how.png")));	

	kinect->calculateAspects();	
}

void Instruction::Init( LocationEngine* game)
{
	leap->resetInitParams();
	kinect->faceSleep(3);
	
	_game = game;
	state = INIT;	
}

void Instruction::Cleanup()
{
	
}
void Instruction::Pause()
{
	
}
void Instruction::Resume()
{
	
}
void Instruction::MouseEvents( LocationEngine* game )
{	
}
void Instruction::HandleEvents( LocationEngine* game )
{

}

void Instruction::Update(LocationEngine* game) 
{			
	switch (state)
	{
		case INIT:
			if (kinect->isFaceDetected())
			{
				state = FACE_DETECTED;
				timeline().apply( &coord1, Vec2f(0.0f, 0.0f), Vec2f(-getWindowBounds().getWidth(),0.0f), 0.8f, EaseOutCubic() ).finishFn( bind( &Instruction::animationInitFinish, this ) );
				coord2 = Vec2f((getWindowBounds().getWidth()-how.getBounds().getWidth())*0.5f,-how.getBounds().getHeight());
				timeline().apply( &coord2,Vec2f((getWindowBounds().getWidth()-how.getBounds().getWidth())*0.5f,0.0f), 1.5f, EaseOutCubic() );	
			}
			
		break;

		case CHECK_HAND_ON_LEAP:			
			if( leap->getLastGestureName() != leapGestures::NONE)
			{
				state = HAND_DETECTED;
				timeline().apply( &alphaFade, 0.0f, 1.0f, 0.3f, EaseOutQuad() ).finishFn( bind( &Instruction::animationFadeOutFinish, this )  );
			}
		break;
	}
}

void Instruction::Draw(LocationEngine* game) 
{
	gl::enableAlphaBlending();
	gl::clear(); 	
	gl::color( ColorA(1.0f, 1.0f, 1.0f, 1.0f) );
	Rectf centeredRect = Rectf( 0,0, 1920,1080 ).getCenteredFit( getWindowBounds(),true );

	switch (state)
	{
		case INIT:			
			gl::draw( fon1, centeredRect);
			gl::draw( slide1, centeredRect);
		break;

		case FACE_DETECTED:
			gl::draw( fon1, centeredRect);			
			gl::pushMatrices();
				gl::translate(coord1);	
				gl::draw( slide1, centeredRect);
				gl::translate(Vec2f(getWindowBounds().getWidth(),0.0f));
				gl::draw(slide2, centeredRect);
			gl::popMatrices();

			setGraphObject(how, coord2);
		break;

		case CHECK_HAND_ON_LEAP:
			gl::draw( fon1, centeredRect);
			gl::draw( slide2, centeredRect);
			setGraphObject(how, coord2);
		break;
		case HAND_DETECTED:			
			gl::draw( fon1, centeredRect);
			gl::draw( slide2, centeredRect);
			setGraphObject(how, coord2);

			gl::color(ColorA(0.0f, 0, 0, alphaFade));
			gl::drawSolidRect(Rectf(0, 0, getWindowWidth(), getWindowHeight()));
			break;
	}
	gl::disableAlphaBlending();	
}

void Instruction::animationInitFinish() 
{
	state = CHECK_HAND_ON_LEAP;
}

void Instruction::animationFadeOutFinish() 
{
	_game->ChangeState(ScanFace::Instance());
}

void Instruction::setGraphObject(ci::gl::Texture tex, Vec2f _value )
{
	gl::pushMatrices();	
			gl::translate(_value);
			gl::draw( tex);
	gl::popMatrices();

}
Vec2f Instruction::getCenterOfScreen(ci::gl::Texture tex )
{
	return Vec2f((getWindowWidth() - tex.getBounds().getWidth())*0.5,(getWindowHeight() - tex.getBounds().getHeight())*0.5);
}