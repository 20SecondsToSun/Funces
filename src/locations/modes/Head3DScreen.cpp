#include "Head3DScreen.h"
#include "Utils.h"

using namespace model;

void Head3D::setup() 
{
	kinect->calculateAspects();
	setUpCamera();

	debugFont   = Font( loadFile(getAssetPath("fonts/Helvetica Neue Bold.ttf")), 36 );
}

void Head3D::init() 
{	
	headsController.initHeads();
}

void Head3D::clean() 
{
	headsController.cleanHeads();	
}

void Head3D::update() 
{	
	//console()<<" is face ::  "<<kinect->isFaceDetected()<<std::endl;
}

void Head3D::draw() 
{
	detectedPeopleInFrameNum = kinect->getSkeletsInFrame();

	kinect->drawKinectCameraColorSurface();
	
	drawHeads3D();
}

void Head3D::drawHeads3D()
{	
	gl::enableDepthRead();

	//int detected_faces = 0;

	bool face1_alive = false;
	bool face2_alive = false;
	bool head1_alive = false;
	bool head2_alive = false;

	//console()<<"------------------------------"<<endl;
	for (int i = 0, maxfaces = kinect->maxFaces(); i < maxfaces; i++)
	{	
		
		if (kinect->isFaceDetected(i))
		{
			/*if (i == 0)
			{
				face1_alive = head1_alive = true;
			}
			else
			{
				face2_alive = head2_alive = true;
			}*/
			MsKinect::Face face = kinect->getFace(i);
			HeadObject *head	= headsController.getHeadObject(i, face);
			if (head != NULL)
			{				
				//console()<<"draw"<<endl;
				drawHead(head);
			}

			//detected_faces++;
		}
		else
		{
			HeadObject *head	= headsController.getHeadDyingObject(i);

			if (head != NULL)
			{
				drawHead(head);

				/*if (i == 0)
				{
					head1_alive = true;
				}
				else
				{
					head2_alive = true;
				}*/
			}
		}
	}

	/*Utils::textFieldDraw("FACE_1 - DETECTED: " + to_string(face1_alive), debugFont, Vec2f(100, 100), Color(1,0,0));
	Utils::textFieldDraw("HEAD_1 - ALIVE: " + to_string(head1_alive), debugFont, Vec2f(100, 200), Color(1,0,0));

	Utils::textFieldDraw("FACE_2 - DETECTED: " + to_string(face2_alive), debugFont, Vec2f(100, 300), Color(1,0,0));
	Utils::textFieldDraw("HEAD_2 - ALIVE: " + to_string(head2_alive), debugFont, Vec2f(100, 400), Color(1,0,0));*/

	gl::disableDepthRead();
}

void Head3D::drawHead(HeadObject* head)
{
	Vec3f rotHeadAngle  = head->getModelOrientation();	
	Vec3f scaleStandart = head->getScale();	
	Vec3f scaleLeap		= head->getScaleLeapMotion();	
	Vec3f rotAngle		= head->getRotAngle();	
	ColorA alphaColor	= head->getAlphaColor();	
			
	onLightsForHead(head); 	
	
	gl::pushMatrices();
				
		set3DLensShift(head);	
		gl::setMatrices(mMayaCam.getCamera());		
		glClear(  GL_DEPTH_BUFFER_BIT );
		gl::enableDepthRead();			
	
		gl::scale(scaleStandart);
		gl::scale(scaleLeap);
	
		gl::rotate(Vec3f(270,0,0));	
		gl::rotate(rotHeadAngle);				
		gl::rotate(rotAngle);
		//gl::enableAlphaBlending();
		//gl::color(alphaColor);
		SkinningRenderer::draw( *head->getHeadMesh());
		//gl::disableAlphaBlending();

	gl::popMatrices();

	offLightsForHead(); 
}

void Head3D::onLightsForHead(HeadObject* head)
{	
	GLfloat no_mat[]		  = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat no_shininess[]    = { 0.0  };
	GLfloat mat_shininess[]	  = { 28.0 };

	Vec4f vec = head->getAmbientLight();
	GLfloat mat_ambient[]	  = {vec.x, vec.y, vec.z, vec.w};

	vec = head->getDiffuseLight();
	GLfloat mat_diffuse	[]	  = {vec.x, vec.y, vec.z, vec.w};

	vec = head->getSpecularLight();
	GLfloat mat_specular[]	  = {vec.x, vec.y, vec.z, vec.w};

	vec = head->getEmissionLight();
	GLfloat mat_emission[]	  = {vec.x, vec.y, vec.z, vec.w};

	vec = head->getLightPosition();
	GLfloat light_position[]  = {vec.x, vec.y, vec.z, vec.w};

	bool DIFFUSE = true, AMBIENT = true, SPECULAR = true, EMISSIVE = true;

	gl::enable( GL_LIGHTING );
	gl::enable( GL_NORMALIZE );
	glEnable( GL_LIGHT0 );

	if( DIFFUSE ){
		glMaterialfv( GL_FRONT, GL_DIFFUSE,	mat_diffuse );
	} else {
		glMaterialfv( GL_FRONT, GL_DIFFUSE,	no_mat );
	}
					
	if( AMBIENT )
		glMaterialfv( GL_FRONT, GL_AMBIENT,	mat_ambient );
	else
		glMaterialfv( GL_FRONT, GL_AMBIENT,	no_mat );
					
	if( SPECULAR ){
		glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular );
		glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess );
	} else {
		glMaterialfv( GL_FRONT, GL_SPECULAR, no_mat );
		glMaterialfv( GL_FRONT, GL_SHININESS, no_shininess );
	}
				
	if( EMISSIVE )
		glMaterialfv( GL_FRONT, GL_EMISSION, mat_emission );
	else
		glMaterialfv( GL_FRONT, GL_EMISSION, no_mat );	

	glLightfv( GL_LIGHT0, GL_POSITION, light_position );	

}

void Head3D::offLightsForHead()
{
	gl::disable( GL_LIGHTING );
	glDisable( GL_LIGHT0 );
	gl::disable( GL_NORMALIZE );
	ci::gl::disable( GL_CULL_FACE );	
}

void Head3D::setUpCamera()
{
	Vec3f mEye        = Vec3f( 0,0,500);
	Vec3f mCenter     = Vec3f( 0,0,0);
	Vec3f mUp         = Vec3f::yAxis();

	CameraPersp cam = mMayaCam.getCamera();
	cam.setPerspective( 60.0f, getWindowAspectRatio(), 50.0f, -1000.0f );	
	cam.setAspectRatio( getWindowAspectRatio() );
	cam.setEyePoint( mEye);
	cam.setCenterOfInterestPoint( mCenter);
	cam.setWorldUp(mUp);
	mMayaCam.setCurrentCam( cam );
}

void Head3D::set3DLensShift(HeadObject* head )
{
	Rectf worldCoords	 = Rectf(0.0f, 0.0f, float(getWindowWidth()), float(getWindowHeight()));
	Rectf kinectCoords	 = kinect->getColorResolutionRectf();	

	float _xPercent = ((head->getFaceCenter().x/kinectCoords.getWidth())*kinect->viewWidth+kinect->viewShiftX)/ worldCoords.getWidth();
	float _xInWorld = _xPercent*worldCoords.getWidth();
	float lensSdvigX;

	if (_xInWorld< worldCoords.getWidth()*0.5)
	{
		lensSdvigX = 1 -float(_xInWorld/(worldCoords.getWidth()*0.5));		
	}
	else
	{
		lensSdvigX = - float((_xInWorld- worldCoords.getWidth()*0.5)/(worldCoords.getWidth()*0.5));
	}

	float scalePercent = head->calcScalePercent();
	float _y = head->getShift(scalePercent).y;
	float _yPercent = (((head->getFaceCenter().y + _y)/kinectCoords.getHeight())*kinect->viewHeight +kinect->viewShiftY)/worldCoords.getHeight();
	float _yInWorld = _yPercent*worldCoords.getHeight();

	float lensSdvigY;

	if (_yInWorld< worldCoords.getHeight()*0.5)
	{
		lensSdvigY = float(_yInWorld/(worldCoords.getHeight()*0.5))-1;
	}
	else
	{
		lensSdvigY =  float((_yInWorld- worldCoords.getHeight()*0.5)/(worldCoords.getHeight()*0.5));
	}

	CameraPersp cam = mMayaCam.getCamera();
	cam.setLensShift(Vec2f(lensSdvigX, lensSdvigY));
	mMayaCam.setCurrentCam( cam );	
}

void Head3D::save()
{

}

void Head3D::changeItem()
{
	HeadObject *head;

	int idToChanage = -1;

	if (kinect->isFaceDetected(0) && kinect->isFaceDetected(1))
	{
		idToChanage = kinect->getLeftFaceID();
	}
	else if (kinect->isFaceDetected(0))
	{
		idToChanage = 0;
	}
	else if (kinect->isFaceDetected(1))
	{
		idToChanage = 1;
	}

	if(idToChanage!=-1)
	{
		MsKinect::Face face = kinect->getFace(idToChanage);
		headsController.killHeadObject(idToChanage);
		head	= headsController.getHeadObject(idToChanage, face);
	}
}


void Head3D::changeSize(std::string value)
{

	HeadObject *head;

	int idToChanage = -1;

	if (kinect->isFaceDetected(0) && kinect->isFaceDetected(1))
	{
		idToChanage = kinect->getLeftFaceID();
	}
	else if (kinect->isFaceDetected(0))
	{
		idToChanage = 0;
	}
	else if (kinect->isFaceDetected(1))
	{
		idToChanage = 1;
	}

	if(idToChanage!=-1)
	{
		MsKinect::Face face = kinect->getFace(idToChanage);
		head	= headsController.getHeadObject(idToChanage, face);

		head->setScaleLeap(value);
	}

}