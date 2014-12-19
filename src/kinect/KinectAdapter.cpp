#include "KinectAdapter.h"
#include "Params.h"

using namespace kinectDefaults;

KinectAdapter KinectAdapter::KinectAdapterState;

void KinectAdapter::Setup()
{
	deviceOptions.enableColor(true);
	deviceOptions.enableDepth(true);
	deviceOptions.setDepthResolution(DEPTH_RESOLUTION);
	deviceOptions.setColorResolution(COLOR_RESOLUTION);
	deviceOptions.setSkeletonSelectionMode(MsKinect::SkeletonSelectionMode::SkeletonSelectionModeClosest2);	
	mDevice = MsKinect::Device::create();

	

	mDevice->connectEventHandler( [ & ]( MsKinect::Frame frame )
	{
		mFrame = frame;

		for (int i = 0; i < FACES_MAX; i++)
		{
			if ( !mFaceTracker[i] )  continue;
			
			if (SKELETS_IN_FRAME > 1 && !funces::oneHead) // IF SKELETONES IN FRAME HINT BY PASSING HEAD POINTS
			{
				mFaceTracker[i]->update( mFrame.getColorSurface(), mFrame.getDepthChannel(), userheadPoints[i]);
			}
			else // if not skeleton hints update only one tracker
			{
				mFaceTracker[0]->update( mFrame.getColorSurface(), mFrame.getDepthChannel());
			}
		}

		if ( frame.getColorSurface() ) 
		{
			mTextureColor = gl::Texture::create( frame.getColorSurface() );
			_mTextureColor = gl::Texture(frame.getColorSurface());
			surface8u =  frame.getColorSurface();
		} 
		else if ( frame.getInfraredChannel() ) 
		{
			mTextureColor = gl::Texture::create( frame.getInfraredChannel() );
			
		}
		if ( frame.getDepthChannel() ) 
		{
			mTextureDepth = gl::Texture::create( frame.getDepthChannel() );
		}

	} );
	
	kinectConnect();

	// ------------------------------------   INIT KINECT  FACES ---------------------------------------------

	for (int i = 0; i < FACES_MAX; i++)
	{
		// Kinect Face
		mFaceTracker[i] = MsKinect::FaceTracker::create();	
		mFaceTracker[i]->enableCalcMesh(false);
		mFaceTracker[i]->enableCalcMesh2d();
	}
		
	mFaceTracker[0]->connectEventHander( [ & ]( MsKinect::Face face )
	{
		mFace[0] = face;
	} );

	
	mFaceTracker[1]->connectEventHander( [ & ]( MsKinect::Face face )
	{
		mFace[1] = face;
	} );
	
	faceTrackStart();
}

void KinectAdapter::kinectConnect()
{
	try {
		mDevice->start(deviceOptions);
	} catch ( MsKinect::Device::ExcDeviceCreate ex ) {
		console() << ex.what() << endl;
	} catch ( MsKinect::Device::ExcDeviceInit ex ) {
		console() << ex.what() << endl;
	} catch ( MsKinect::Device::ExcDeviceInvalid ex ) {
		console() << ex.what() << endl;
	//} catch ( MsKinect::Device::ExcGetCoordinateMapper ex ) {
	//	console() << ex.what() << endl;
	} catch ( MsKinect::Device::ExcOpenStreamColor ex ) {
		console() << ex.what() << endl;
	} catch ( MsKinect::Device::ExcOpenStreamDepth ex ) {
		console() << ex.what() << endl;
	//} catch ( MsKinect::Device::ExcStreamStart ex ) {
	//	console() << ex.what() << endl;
	//} catch ( MsKinect::Device::ExcUserTrackingEnable ex ) {
	//	console() << ex.what() << endl;
	}
}

void KinectAdapter::faceTrackStart()
{
	try
	{
		for (int i = 0; i < FACES_MAX; i++)
		{	
			mFaceTracker[i]->start(deviceOptions);
		}
		
	}
	catch(...)
	{
	}
}

void KinectAdapter::Update() 
{
	updateSkeletonData();
	
	_facesDetectedNum = 0;

	for (int i = 0; i < FACES_MAX; i++)
	{
		_isFaceDetected[i] = false;
	}

	if (faceSleepTimer.isStopped() == false && faceSleepTimer.getSeconds()>faceSleepSeconds)
		faceSleepTimer.stop();

	if (!faceSleepTimer.isStopped()) return;		


	for (int i = 0; i < FACES_MAX; i++)
	{
		if ( mFace[i].getMesh2d().getNumVertices() > 0 && mFace[i].getTranslationTransform().z < 2.0f)
		{
			_isFaceDetected[i] = true;
			_facesDetectedNum++;			
		}
	}

	if (handsSleepTimer.isStopped() == false && handsSleepTimer.getSeconds()>handsSleepSeconds)
		handsSleepTimer.stop();

	
}

void KinectAdapter::updateSkeletonData()
{
	_NUI_SKELETON_POSITION_INDEX  headJoint = NUI_SKELETON_POSITION_HEAD;
	_NUI_SKELETON_POSITION_INDEX  neckJoint = NUI_SKELETON_POSITION_HEAD;

	_NUI_SKELETON_POSITION_INDEX   handRight     = NUI_SKELETON_POSITION_HAND_RIGHT;
	_NUI_SKELETON_POSITION_INDEX   handLeft      = NUI_SKELETON_POSITION_HAND_LEFT;

	SKELETS_IN_FRAME = 0;

	handsPosition.clear();

	if ( mFrame.getDepthChannel() ) 
	{
		for( size_t i = 0, ilen= mFrame.getSkeletons().size(); i < ilen; i++) 
        {
			if(mFrame.getSkeletons().at(i).find(headJoint) != mFrame.getSkeletons().at(i).end())
            {
				const MsKinect::Bone& headBone = mFrame.getSkeletons().at(i).find(headJoint)->second;
				auto  startV3				   = mFrame.getSkeletons().at(i).at( headBone.getStartJoint()).getPosition();
				auto  endtV3 = mFrame.getSkeletons().at(i).at( headBone.getEndJoint()).getPosition();		

				userheadPoints[SKELETS_IN_FRAME][0] = startV3;
				userheadPoints[SKELETS_IN_FRAME][1] = endtV3;

				SKELETS_IN_FRAME++;
			}

			if (!handsSleepTimer.isStopped()) continue;

			if(mFrame.getSkeletons().at(i).find(handRight) != mFrame.getSkeletons().at(i).end())
            {
				const MsKinect::Bone& handBone = mFrame.getSkeletons().at(i).find(handRight)->second;
				Vec2i v0 = mDevice->mapSkeletonCoordToDepth( handBone.getPosition() );	
				handsPosition.push_back(v0);
			}

			if(mFrame.getSkeletons().at(i).find(handLeft) != mFrame.getSkeletons().at(i).end())
            {
				const MsKinect::Bone& handBone = mFrame.getSkeletons().at(i).find(handLeft)->second;
				Vec2i v0 = mDevice->mapSkeletonCoordToDepth( handBone.getPosition() );
				handsPosition.push_back(v0);
			}
		}				
	}
}

void KinectAdapter::handsSleep(int seconds)
{
	handsSleepSeconds = seconds;
	handsSleepTimer.start();
}

void KinectAdapter::faceSleep(int seconds)
{
	faceSleepSeconds = seconds;
	faceSleepTimer.start();
}

vector<Vec2i> KinectAdapter::getHandsPosition()
{
	return handsPosition;
}

void KinectAdapter::Shutdown()
{	
	mDevice->stop();

	for (int i = 0; i < FACES_MAX; i++)		
		mFaceTracker[i]->stop();	
}

ci::gl::TextureRef KinectAdapter::getColorTexRef()
{
	return mTextureColor;
}

ci::gl::Texture KinectAdapter::getColorTex()
{
	return _mTextureColor;
}

ci::gl::TextureRef KinectAdapter::getDepthTexRef()
{
	return mTextureDepth;
}

ci::Surface8u  KinectAdapter::getSurface8u()
{
	return surface8u;
}

Rectf KinectAdapter::getColorResolutionRectf()
{
	if (MsKinect::ImageResolution::NUI_IMAGE_RESOLUTION_640x480 == COLOR_RESOLUTION)
		return Rectf(0.0f, 0.0f, 640.0f,480.0f);

	if (MsKinect::ImageResolution::NUI_IMAGE_RESOLUTION_1280x960== COLOR_RESOLUTION)
		return Rectf(0.0f, 0.0f, 1280.0f,960.0f);
}	

int KinectAdapter::getSkeletsInFrame()
{
	return SKELETS_IN_FRAME;
}

bool KinectAdapter::isFaceDetected(int i) 
{
	return _isFaceDetected[i];
}

bool KinectAdapter::isFaceDetected() 
{
	for (int i = 0; i < FACES_MAX; i++)
	{		
		if (_isFaceDetected[i]) return true;
	}

	return false;
}

int KinectAdapter::getFacesDetectedNum() 
{
	return _facesDetectedNum;
}

MsKinect::Face KinectAdapter::getFace(int i) 
{
	return mFace[i];
}

int KinectAdapter::getLeftFaceID() 
{
	if ( mFace[0].getMesh2d().calcBoundingBox().getCenter().x >  mFace[1].getMesh2d().calcBoundingBox().getCenter().x)
		return 0;
	else 
		return 1;
}

int KinectAdapter::getRightFaceID() 
{
	if ( mFace[0].getMesh2d().calcBoundingBox().getCenter().x >  mFace[1].getMesh2d().calcBoundingBox().getCenter().x)
		return 1;
	else 
		return 0;
}

void KinectAdapter::drawKinectCameraColorSurface()
{
	gl::clear();
	gl::setMatricesWindow( getWindowSize());
	gl::enableAlphaBlending();

	Texture colorRef =  getColorTex();

	if (colorRef)
	{
		gl::enable( GL_TEXTURE_2D );	

		gl::color( ColorAf::white() );
		
		gl::pushMatrices();
			gl::translate(viewShiftX, viewShiftY);	
			gl::draw( colorRef, colorRef.getBounds(), Rectf(0, 0, float(viewWidth),float(viewHeight)));	
		gl::popMatrices();		

		gl::disable( GL_TEXTURE_2D );
	}

	gl::disableAlphaBlending();
}

void KinectAdapter::calculateAspects()
{
	Rectf kinectResolutionR = getColorResolutionRectf();
	float aspect =  kinectResolutionR.getWidth()/kinectResolutionR.getHeight();
			
	if( getWindowWidth() / getWindowHeight() > aspect)			
	{
		viewHeight = getWindowHeight();
		viewWidth = int(viewHeight * aspect);	
		headScale = viewHeight/ kinectResolutionR.getHeight();
	}
	else 
	{ 
		viewWidth = getWindowWidth();
		viewHeight = int(viewWidth / aspect);	
		headScale  = viewWidth/ kinectResolutionR.getWidth();
	}
	
	viewShiftX =float( 0.5 * (getWindowWidth()  - viewWidth));
	viewShiftY= float( 0.5 * (getWindowHeight() - viewHeight));		
}