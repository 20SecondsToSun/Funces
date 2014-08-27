#pragma once
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "AssetsManager.h"
#include "cinder/Text.h"
#include "cinder/Timeline.h"


class ChangeModeButton
{
	public:		
		void		draw();
		bool		isDown(Vec2f coords);

		void		setState(std::string state);
		

private:
	ci::Font						qrCodeFont ;
	ci::Anim<ci::Vec2f>				startQRCodeHolderXY;
	ci::gl::Texture					qrCodeFon, qrCodeTextTexture, qrCodeTexture;

	ci::Color						color;


};