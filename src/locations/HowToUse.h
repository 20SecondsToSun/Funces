#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Timeline.h"
#include "cinder/gl/Texture.h"
#include "cinder/Text.h"
#include "cinder/Timer.h"

#include "Location.h"
#include "LeapController.h"
#include "KinectAdapter.h"

using namespace ci;
using namespace ci::app;
using namespace gl;
using namespace std;

namespace useDefaults
{
	enum states {INIT, WAITING_FOR_WAVE,  CHANGE_HERO, FACE_DETECTED, HAND_DETECTED, WAITING_FOR_ZOOM , ZOOM_FINISHED, WAITING_FOR_TWO_FINGERS};
}

class HowToUse : public Location
{

public:
	void Setup();
	void Init( LocationEngine* game);
	void Cleanup();

	void Pause();
	void Resume();

	void HandleEvents(LocationEngine* game);
	void Update(LocationEngine* game);
	void Draw(LocationEngine* game);

	static HowToUse* Instance() {
		return &HowToUseState;
	}

protected:
	HowToUse() { };

private:
	void					 MouseEvents( LocationEngine* game );

	LocationEngine*			_game;
	static HowToUse		HowToUseState;



	int				state;

	void			animationInitFinish() ;
	void			animationFadeOutFinish() ;
	ci::Anim<float>	alphaFade;

	void			animationZoomFinish();

	void			animationChangeHeroFinish();




	ci::gl::Texture  firstSlide;

	int				currentSlide;
	std::map<int, ci::gl::Texture>  slideDic;	

	ci::Anim<ci::Vec2f>		coord1, coord2, coord3, coord4, roboScale;
	ci::Anim<ci::ColorA>		alpha;
	ci::gl::Texture fon1,fon2,fon3,fon4;
	ci::gl::Texture slide1,slide2,slide3,slide4,slide5, how, hello;

	ci::gl::Texture gesture1 [12];
	ci::gl::Texture gesture2 [2];
	ci::gl::Texture gesture3[1];

	ci::Anim<ci::Vec2f> coordMike, coordPhotoRamka, coordLastFrameText, roboCoord;

	ci::gl::Texture gestureTex;
	ci::gl::Texture mike, trans;
	ci::gl::Texture textTexture;
	ci::gl::Texture photoramka;
	
	void setGraphObject(ci::gl::Texture tex,  ci::Vec2f _value );
	ci::Vec2f getCenterOfScreen(ci::gl::Texture tex );

	int			RULES_SCREENS_NUM ;

	int			animState;
	int			zoomBuffer, swipeBuffer, startBuffer;
	
	int			gestureAnimIndex;
	int			sign ;
	int			pause ;

	ci::Vec2f   _scale;

	void animationFinish();


	
	static LeapController*			leap;
	
};