#pragma once
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "AssetsManager.h"
#include "cinder/Text.h"
#include "cinder/Timeline.h"
#include "PreloaderCircle.h"

class QRcode
{
	public:
		void		setup();
		void		draw();
		void		initAnim();

private:
	ci::Font						qrCodeFont ;
	ci::Anim<ci::Vec2f>				startQRCodeHolderXY;
	ci::gl::Texture					qrCodeFon, qrCodeTextTexture, qrCodeTexture;

	PreloaderCircle					preloader;

};