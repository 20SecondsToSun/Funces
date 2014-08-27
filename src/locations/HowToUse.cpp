#include "HowToUse.h"
#include "MainGame.h"

using namespace ci;
using namespace ci::app;
using namespace useDefaults;
using namespace leapGestures;

HowToUse HowToUse::HowToUseState;
LeapController* HowToUse::leap	= LeapController::Instance();

void HowToUse::Setup()
{
	RULES_SCREENS_NUM = 4;
	animState =  0;
	zoomBuffer = 0;
	swipeBuffer = 0;
	gestureAnimIndex = 0;
	sign = 1;
	pause = 0;
	_scale = Vec2f(0.6f,0.6f);

	fon3   = gl::Texture(loadImage(getAssetPath("rules/_fon3.png")));
	fon4   = gl::Texture(loadImage(getAssetPath("rules/_fon4.png")));
	
	slide3 = gl::Texture(loadImage(getAssetPath("rules/_slide3.png")));
	slide4 = gl::Texture(loadImage(getAssetPath("rules/_slide4.png")));
	slide5 = gl::Texture(loadImage(getAssetPath("rules/_slide5.png")));
	how    = gl::Texture(loadImage(getAssetPath("rules/how.png")));
	hello  = gl::Texture(loadImage(getAssetPath("rules/HELLO.png")));

	for (int i = 0; i <12; i++)
	{
		string imgName = toString(i+1);
		if (imgName.length() == 1) imgName = "0"+ imgName;
		gesture1[i] = gl::Texture(loadImage(getAssetPath("gestures/"+imgName+".png")));
	}

	for (int i = 0; i <2; i++)
	{
		string imgName = toString(i+1);
		if (imgName.length() == 1) imgName = "1_0"+ imgName;
		else imgName = "1_"+ imgName;
		gesture2[i] = gl::Texture(loadImage(getAssetPath("gestures/"+imgName+".png")));
	}

	gesture3[0]= gl::Texture(loadImage(getAssetPath("gestures/2_01.png")));

	gestureTex = gl::Texture(loadImage(getAssetPath("rules/gesture.png")));

	mike = gl::Texture(loadImage(getAssetPath("rules/mike.png")));
	trans = gl::Texture(loadImage(getAssetPath("rules/trans.png")));

	photoramka = gl::Texture(loadImage(getAssetPath("rules/photoramka.png")));

}

void HowToUse::Init( LocationEngine* game)
{
	animState = 0;
	zoomBuffer = 0;
	swipeBuffer = 0;

	leap->resetInitParams();

	_game = game;

	state = INIT;
	
}

void HowToUse::Cleanup()
{
	
}

void HowToUse::Pause()
{
	
}
void HowToUse::Resume()
{
	
}

void HowToUse::MouseEvents( LocationEngine* game )
{
	MouseEvent event = game->getMouseEvent();	

	if(game->isAnimationRunning()) return;	
}

void HowToUse::HandleEvents( LocationEngine* game )
{

}

void HowToUse::Update(LocationEngine* game) 
{			
	switch (state)
	{
		case INIT:
			coord2 = Vec2f((getWindowBounds().getWidth()-342)*0.5f, -182);
			coord3 = Vec2f(0.0f, getWindowBounds().getHeight());
			coord4 = Vec2f(0.0f, 0.0f);			
			coordMike = Vec2f((getWindowWidth() - 404)*0.5f+ 20.0f, (getWindowBounds().getHeight() - 431)*0.5);
			timeline().apply( &alphaFade, 1.0f, 0.0f, 0.3f, EaseOutQuad() );
			state = WAITING_FOR_WAVE;
		break;

		case WAITING_FOR_WAVE:	
			
			if( leap->getLastGestureName()  == SWIPE_LEFT || leap->getLastGestureName()  == SWIPE_RIGHT)
			{
				if (swipeBuffer < 1) 
				{
					swipeBuffer++;
					return ;
				}
				else
				{
					state = CHANGE_HERO;
					timeline().apply( &alpha, ColorA(1.0f, 1.0f, 1.0f, 1.0f), ColorA(1.0f, 1.0f, 1.0f, 0.0f), 0.8f, EaseOutCubic() ).finishFn( bind( &HowToUse::animationChangeHeroFinish, this ) );
					timeline().apply( &coord3,Vec2f(0,0), Vec2f(-getWindowBounds().getWidth(),0), 0.8f, EaseOutCubic() );
				}			
			}
			
		break;

		case WAITING_FOR_ZOOM:	

			if(leap->getLastGestureName()  == BIGGER || leap->getLastGestureName()  == SMALLER)
			{
				if (_scale.x<0.8 && leap->getLastGestureName() == BIGGER)
				{
					_scale+=0.1;
				}
				if (_scale.x>0.3 && leap->getLastGestureName() == SMALLER)
				{
					_scale-=0.1;
				}	
				if (zoomBuffer < 20) 
				{
					zoomBuffer++;
					return ;
				}			

				alpha = ColorA(1.0f, 1.0f, 1.0f, 1.0f);
				timeline().apply( &alpha,ColorA(1.0f, 1.0f, 1.0f, 0.0f), 0.8f, EaseOutCubic() ).finishFn( bind( &HowToUse::animationZoomFinish, this ) );

				timeline().apply( &coord3,Vec2f(0,0), Vec2f(0,-getWindowBounds().getHeight()), 0.4f, EaseOutCubic() );

				coordLastFrameText = Vec2f(0,-getWindowBounds().getHeight());
				timeline().apply( &coordLastFrameText,Vec2f(0,0 ), 0.6f, EaseOutCubic());

				coordPhotoRamka = Vec2f((getWindowBounds().getWidth()-how.getBounds().getWidth())*0.5 -50,getWindowBounds().getHeight());
				timeline().apply( &coordPhotoRamka,Vec2f((getWindowBounds().getWidth()-how.getBounds().getWidth())*0.5 - 50,237), 0.8f, EaseOutCubic() );

				timeline().apply( &roboScale,_scale,Vec2f(0.4,0.4), 0.6f, EaseOutCubic() );
				timeline().apply( &roboCoord,Vec2f(0,0), Vec2f(200, 0), 0.6f, EaseOutCubic() );	

				state = ZOOM_FINISHED;
			}

			break;

		case WAITING_FOR_TWO_FINGERS:	
			if(leap->getLastGestureName()  == TWO_FINGERS )
			{
				timeline().apply( &alphaFade, 0.0f, 1.0f, 0.3f, EaseOutQuad() ).finishFn( bind( &HowToUse::animationFadeOutFinish, this )  );	
			}
			break;
	}
}

void HowToUse::Draw(LocationEngine* game) 
{
	gl::enableAlphaBlending();
	gl::clear(); 	
	gl::color( ColorA(1.0f, 1.0f, 1.0f, 1.0f) );
	Rectf centeredRect = Rectf( fon1.getBounds() ).getCenteredFit( getWindowBounds(),true );

	Vec2f t = getCenterOfScreen(trans);
	Vec2f _roboCoord;
	
	switch(state)
	{		
		case WAITING_FOR_WAVE:
			gl::color( ColorA(1.0f, 1.0f, 1.0f, 1.0f) );
			gl::draw( fon2, centeredRect);

			gl::pushMatrices();	
				gl::translate(0,getWindowHeight()-gestureTex.getBounds().getHeight());
				gl::draw( gestureTex);
			gl::popMatrices();

			gl::pushMatrices();	
				gl::translate(coord4);
				gl::draw( slide3, centeredRect);
			gl::popMatrices();
		
			setGraphObject(mike, "coord", coordMike);
			setGraphObject(how, "coord", coord2);

			gl::color(ColorA(0, 0, 0, alphaFade));
			gl::drawSolidRect(Rectf(0, 0, getWindowWidth(), getWindowHeight()));

		break;

		case CHANGE_HERO:
			gl::color( ColorA(1.0f, 1.0f, 1.0f, 1.0f) );
			gl::draw( fon3, centeredRect);

			gl::color( alpha );
			gl::draw( fon2, centeredRect);			

			gl::color( ColorA(1.0f, 1.0f, 1.0f, 1.0f) );

			gl::pushMatrices();	
				gl::translate(coord3);
				gl::draw( slide3, centeredRect);
				setGraphObject(mike, "coord", coordMike);
				gl::translate(Vec2f(getWindowBounds().getWidth(),0));
				gl::draw(slide4, centeredRect);
				gl::pushMatrices();				
					gl::scale(_scale);
					gl::translate( getWindowWidth() *0.5* (1/_scale.x)- trans.getBounds().getWidth()*0.5, getWindowHeight()*0.5* (1/_scale.y) - trans.getBounds().getHeight()*0.5);
					gl::draw(trans);
				gl::popMatrices();
			gl::popMatrices();

			setGraphObject(how, "coord", coord2);
			setGraphObject(gestureTex, "coord", Vec2f(0,getWindowHeight()-gestureTex.getBounds().getHeight()));

		break;

		case WAITING_FOR_ZOOM:
			gl::color( ColorA(1.0f, 1.0f, 1.0f, 1.0f) );
			gl::draw( fon3, centeredRect);
			gl::draw( slide4, centeredRect);
			
			gl::pushMatrices();				
				gl::scale(_scale);
				gl::translate( getWindowWidth() *0.5* (1/_scale.x)- trans.getBounds().getWidth()*0.5, getWindowHeight()*0.5* (1/_scale.y) - trans.getBounds().getHeight()*0.5);
				gl::draw(trans);
			gl::popMatrices();

			setGraphObject(how, "coord", coord2);
			setGraphObject(gestureTex, "coord", Vec2f(0,getWindowHeight() - gestureTex.getBounds().getHeight()));

			gl::pushMatrices();	
				gl::translate((getWindowWidth() -gesture2[0].getBounds().getWidth())*0.5,getWindowHeight()-gesture2[0].getBounds().getHeight()+10);
				if (getElapsedFrames() % (20)==0)
				{
					if (pause>0) pause--;			

					if (pause == 1 ) gestureAnimIndex = 0;
					if (pause == 0 )
					{
						gestureAnimIndex += sign;
						if (gestureAnimIndex>=1) 
						{
							//gestureAnimIndex=0;
							pause = 5;
						}											
					}				
				}
				gl::draw( gesture2[gestureAnimIndex]);

			gl::popMatrices();

		break;
		
		case ZOOM_FINISHED:
		case WAITING_FOR_TWO_FINGERS:
			
			gl::color( ColorA(1.0f, 1.0f, 1.0f, 1.0f) );
			gl::draw( fon4, centeredRect);

			gl::color( alpha );
			gl::draw( fon3, centeredRect);			

			gl::color( ColorA(1.0f, 1.0f, 1.0f, 1.0f) );
			gl::pushMatrices();	
				gl::translate(coord3);
				gl::draw( slide4, centeredRect);			
			gl::popMatrices();
			setGraphObject(how, "coord", coord2);
			setGraphObject(gestureTex, "coord", Vec2f(0,getWindowHeight() - gestureTex.getBounds().getHeight()));

			setGraphObject(photoramka, "coord", coordPhotoRamka);
			setGraphObject(slide5, "coord", coordLastFrameText);

			gl::pushMatrices();				
				gl::scale(roboScale);
				_scale = roboScale;
				_roboCoord = roboCoord;
				gl::translate( getWindowWidth() *0.5* (1/_scale.x)- trans.getBounds().getWidth()*0.5, getWindowHeight()*0.5* (1/_scale.y) - trans.getBounds().getHeight()*0.5 - _roboCoord.x);
				gl::draw(trans);
			gl::popMatrices();

			gl::color(ColorA(0, 0, 0, alphaFade));
			gl::drawSolidRect(Rectf(0, 0, getWindowWidth(), getWindowHeight()));
		break;
	}

	gl::disableAlphaBlending();
}

void HowToUse::animationChangeHeroFinish() 
{
	state = WAITING_FOR_ZOOM;
}

void HowToUse::animationZoomFinish() 
{
	state = WAITING_FOR_TWO_FINGERS;
}

void HowToUse::animationInitFinish() 
{
	//state = CHECK_HAND_ON_LEAP;
}

void HowToUse::animationFadeOutFinish() 
{
	_game->ChangeState(MainGame::Instance());
}

void HowToUse::setGraphObject(ci::gl::Texture tex, string _property,Vec2f _value )
{
	gl::pushMatrices();	
			gl::translate(_value);
			gl::draw( tex);
	gl::popMatrices();

}

Vec2f HowToUse::getCenterOfScreen(ci::gl::Texture tex )
{
	return Vec2f((getWindowWidth() - tex.getBounds().getWidth())*0.5,(getWindowHeight() - tex.getBounds().getHeight())*0.5);
}