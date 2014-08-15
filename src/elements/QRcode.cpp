#include "QRcode.h"

void QRcode::setup()
{	
	qrCodeFon = *AssetManager::getInstance()->getTexture("images/qrCodeHolder.png");
	qrCodeFont = Font( loadFile(getAssetPath("fonts/Helvetica Neue Bold.ttf")), 36 );

	preloader.setup();
}

void QRcode::draw()
{
	gl::pushMatrices();					
		gl::translate(startQRCodeHolderXY);
		gl::draw(qrCodeFon);
	

	if(qrCodeTextTexture)
	{
		gl::pushMatrices();			
		gl::translate(33, 885);	
		gl::draw(qrCodeTextTexture);
		gl::popMatrices();
	}
	else
	{
		TextLayout simple;
		simple.setFont( qrCodeFont );
		simple.setColor( Color( 1, 1, 1 ) );
		simple.addLine("FUNSES.RU");
		qrCodeTextTexture = gl::Texture( simple.render( true, false ) );
	}


	if(qrCodeTexture)
	{
		gl::pushMatrices();			
		gl::translate(56, 474);		
		gl::draw(qrCodeTexture);
		gl::popMatrices();
	}
	else
	{
		gl::pushMatrices();
			gl::translate(220, 650);	
			preloader.draw();			
		gl::popMatrices();
	}

	gl::popMatrices();	
}

void QRcode::initAnim()
{
	timeline().apply( &startQRCodeHolderXY,	Vec2f(1270.0f,-971.0f), Vec2f(1270.f, 0.f), 0.6f, EaseOutQuad() );
}
