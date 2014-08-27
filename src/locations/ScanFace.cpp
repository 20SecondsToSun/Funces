#include "ScanFace.h"

using namespace ci;
using namespace ci::app;
using namespace faceDefaults;

ScanFace ScanFace::ScanFaceState;
KinectAdapter* ScanFace::kinect	= KinectAdapter::Instance();

void ScanFace::Setup()
{		
	hintFont = Font( loadFile(getAssetPath("fonts/Helvetica Neue Bold.ttf")), 76 );
	kinect->calculateAspects();
}

void ScanFace::Init( LocationEngine* game)
{	
	_game = game;
	state = SCANNING;
 	isPositionZOK = isPositionXYOK = isHeadAngleOK = false;
}

void ScanFace::Cleanup()
{
	photoTimer.stop();
}

void ScanFace::Pause()
{
	
}

void ScanFace::Resume()
{
	
}

void ScanFace::MouseEvents( LocationEngine* game )
{

}

void ScanFace::HandleEvents( LocationEngine* game )
{
	
}

void ScanFace::saveFaceThread( )
{
	ci::ThreadSetup threadSetup; // instantiate this if you're talking to Cinder from a secondary thread
	
	FaceController::saveFace();

	saverThread->detach();

	state = START_FADE_OUT_ANIM;
}

void ScanFace::Update(LocationEngine* game) 
{
	switch(state)
	{
		case START_FADE_IN_ANIM:
			timeline().apply( &alphaFade, 1.0f, 0.0f, 0.8f, EaseOutQuad() ).finishFn( bind( &ScanFace::animationFadeInFinish, this )  );	
			state = FADE_IN_ANIM;
		break;

		case START_FADE_OUT_ANIM:
			timeline().apply( &alphaFade, 0.0f, 1.0f, 0.8f, EaseOutQuad() ).finishFn( bind( &ScanFace::animationFadeOutFinish, this )  );	
			state = FADE_OUT_ANIM;
		break;

		case SCANNING:
			setActiveFaceID(); 

			if (activeFaceID != -1)
			{
				isPositionZOK = kinect->getFace(activeFaceID).getTranslationTransform().z < 1.3;
				Vec3f angleEpsilon = faceDefaults::angleEpsilon;
				Vec3f rotTransform = kinect->getFace(activeFaceID).getRotationTransform();
				isHeadAngleOK = (-angleEpsilon.x < rotTransform.x && rotTransform.x <  angleEpsilon.x  &&
								-angleEpsilon.y < rotTransform.y && rotTransform.y <  angleEpsilon.y  &&
								-angleEpsilon.z  < rotTransform.z && rotTransform.z <  angleEpsilon.z);	

				float _XCoord = kinect->getFace(activeFaceID).getMesh2d().calcBoundingBox().getCenter().x;
				float _YCoord = kinect->getFace(activeFaceID).getMesh2d().calcBoundingBox().getCenter().y;
				float _width  = kinect->getColorResolutionRectf().getWidth();
				float _height = kinect->getColorResolutionRectf().getHeight();
				float  gamma  = 50;

				isPositionXYOK = ((_XCoord < _width*0.5 + gamma && _XCoord > _width*0.5 - gamma)
					&&  (_YCoord < _height*0.5 + gamma && _YCoord > _height*0.5 - gamma));
			}

			if (isGoodConditionsForHeadScan() && photoTimer.isStopped())	
				photoTimer.start();	

			if (isWrongConditionsForHeadScan() || activeFaceID == -1)
				photoTimer.stop();
			break;
	}	
}

void ScanFace::setActiveFaceID() 
{
	if (kinect->isFaceDetected(0) && kinect->isFaceDetected(1) )
	{
		if(kinect->getFace(0).getTranslationTransform().z< kinect->getFace(1).getTranslationTransform().z)
			activeFaceID = 0;
		else 
			activeFaceID = 1;		
	}
	else if (kinect->isFaceDetected(0))
	{
		activeFaceID = 0;		
	}
	else if (kinect->isFaceDetected(1))
	{
		activeFaceID = 1;		
	}
	else
	{
		activeFaceID = -1;	
		isPositionZOK = false;
		isHeadAngleOK = false;
		isPositionXYOK = false;
	}
}

bool ScanFace::isWrongConditionsForHeadScan()
{
	return (isPositionZOK == false || isPositionXYOK == false || isHeadAngleOK == false);
}

bool ScanFace::isGoodConditionsForHeadScan()
{
	return (isPositionZOK && isPositionXYOK && isHeadAngleOK);
}

void ScanFace::Draw(LocationEngine* game) 
{
	gl::clear();

	kinect->drawKinectCameraColorSurface();

	switch(state)
	{
		case FADE_OUT_ANIM:
		case FADE_IN_ANIM:
			
			gl::enableAlphaBlending();			
			gl::color(ColorA(0, 0, 0, alphaFade));
			gl::drawSolidRect(Rectf(0, 0, getWindowWidth(), getWindowHeight()));
			gl::color(Color::white());
			gl::disableAlphaBlending();
			break;

		case SCANNING:
			drawFaceWiredMask();

			drawTextHints();

			if (photoTimer.isStopped() == false && ( (int)photoTimer.getSeconds()> faceDefaults::scanSeconds))				
				makefaceScreenShot();

			break;
	}	
}

void ScanFace::drawFaceWiredMask()
{
	if (activeFaceID != -1 && kinect->isFaceDetected(activeFaceID))
	{
		gl::pushMatrices();			
			gl::translate(kinect->viewShiftX, kinect->viewShiftY);
			gl::scale(kinect->headScale, kinect->headScale );

			gl::enableWireframe();
			gl::color(ColorA(1,0,0,0.5));
			gl::draw( kinect->getFace(activeFaceID).getMesh2d());
			gl::color(Color::white());
			gl::disableWireframe();
		gl::popMatrices();	
	}
}

void ScanFace::drawTextHints()
{
	gl::enableAlphaBlending();

	ColorA colorZhint = ColorA(1,0,0,1);
	if (isPositionZOK)  colorZhint= ColorA(1,0,0,0.2);

	Utils::textFieldDraw(faceDefaults::distanceHint, hintFont, Vec2f(0.f, 0.f), colorZhint);

	ColorA colorXYhint = ColorA(1,0,0,1);
	if (isPositionXYOK)  colorXYhint= ColorA(1,0,0,0.2);

	Utils::textFieldDraw(faceDefaults::faceXYHint, hintFont, Vec2f(0.f, 100.f), colorXYhint);

	ColorA colorAnglehint = ColorA(1,0,0,1);
	if (isHeadAngleOK)  colorAnglehint= ColorA(1,0,0,0.2);

	Utils::textFieldDraw(faceDefaults::faceAngleHint, hintFont, Vec2f(0.f, 200.f), colorAnglehint);

	if (!photoTimer.isStopped())
	{		
		int currentSecond = (int)photoTimer.getSeconds();		
		Utils::textFieldDraw(to_string(currentSecond), hintFont, Vec2f(1000.f, 400.f), ColorA(1,0,0,1));
	}

	gl::disableAlphaBlending();
}

void ScanFace::makefaceScreenShot()
{
	if (activeFaceID != -1 && kinect->isFaceDetected(activeFaceID))
	{		
		renderFaceToFbo();		
		FaceController::prepareFaceToSave( kinect->getFace(activeFaceID).getMesh2d(), faceTexture, kinect->headScale);	
		state = SAVING_FACE;
		saverThread = shared_ptr<thread>( new thread( bind( &ScanFace::saveFaceThread, this ) ) );	
	}
}

void ScanFace::renderFaceToFbo()
{
	TriMesh2d mesh = kinect->getFace(activeFaceID).getMesh2d();
	Rectf     rect =	mesh.calcBoundingBox();
	faceFbo = gl::Fbo( rect.getWidth(), rect.getHeight());

	 gl::SaveFramebufferBinding bindingSaver;	

     faceFbo.bindFramebuffer();

	 Area saveView = getViewport();

     gl::setViewport(faceFbo.getBounds() );

	 gl::pushMatrices();
		  gl::setMatricesWindow( faceFbo.getSize(), false );
		  gl::clear( Color( 0, 0, 0 ) ); 		 
		  gl::translate(-Vec2i(rect.x1 , rect.y1));
		  gl::draw(kinect->getColorTex());	 
	 gl::popMatrices();
	
	 gl::setViewport(saveView);

	 faceTexture = faceFbo.getTexture();
}

void ScanFace::animationFadeOutFinish()
{
	_game->ChangeState(MainGame::Instance());
}
void ScanFace::animationFadeInFinish()
{
	state = SCANNING;
}