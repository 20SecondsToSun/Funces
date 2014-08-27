#include "FaceScreen.h"

using namespace myFaceScreen;

int Face::lowTri  = 0;
int Face::highTri = 120;

void Face::setup() 
{
	mParams = params::InterfaceGl::create( getWindow(), "App parameters", toPixels( Vec2i( 200, 400 ) ) );
		// Setup the parameters
	mParams->addParam( "lowTri", &lowTri ).min( 1 ).max( 206 ).step(1 );	
	mParams->addSeparator();
	mParams->addParam( "highTri", &highTri ).min( 1 ).max( 206 ).step(1 );	

	kinect->calculateAspects();	

	faceGradientMask = *AssetManager::getInstance()->getTexture("mask.png");

	char *vertexShaderGlsl =
		"void main()\n"
		"{\n"
		"gl_TexCoord[0] = gl_MultiTexCoord0;\n"
		"	gl_Position = ftransform();\n"
		"}\n";

    char * alphaShaderSource =
    "#extension GL_ARB_texture_rectangle : enable\n"  
    "uniform sampler2D mask;"  
    "uniform sampler2D tex;"  
    "void main()"  
    "{"  
    "	vec3 col = texture2D(tex, gl_TexCoord[0].st).rgb;"  
    "	float alpha = texture2D(mask, gl_TexCoord[0].st).a;"  
    "	gl_FragColor = vec4(col, alpha);"  
    "}"; 

	maskBlurShader = gl::GlslProg ( vertexShaderGlsl, alphaShaderSource );	
}

void Face::init() 
{
	faceController.initFaces();
}

void Face::clean() 
{
	faceController.cleanFaces();	
}


void Face::update() 
{

}

void Face::draw() 
{
	kinect->drawKinectCameraColorSurface();

	for (int i = 0, maxfaces = kinect->maxFaces(); i < maxfaces; i++)
	{		
		if (kinect->isFaceDetected(i))
		{
			MsKinect::Face face		= kinect->getFace(i);
			FaceObject *faceMask	= faceController.getFaceObject(i, face);

			if (faceMask != NULL) 
			{
				faceMask->setMesh2D(face.getMesh2d());
				drawFaceMask(faceMask);
			}		
		}
		else
		{
			FaceObject *faceMask	= faceController.getFaceDyingObject(i);

			if (faceMask != NULL)	drawFaceMask(faceMask);
		}
	}

	// Draw the interface

	#ifdef DEBUG
		mParams->draw();
	#endif
}

void Face::drawFaceMask(FaceObject* faceMask)
{
	vector<Vec2f> currentCoords = faceMask->getPoints();
	TriMesh2d mesh = faceMask->getMesh2D();
	for (int i = 0; i < mesh.getNumVertices(); i++)	  
		 mesh.appendTexCoord(currentCoords[i]); 

	faceGradientMask.enableAndBind();
	faceGradientMask.bind(1);

	faceMask->getTexture().enableAndBind();
	faceMask->getTexture().bind(0);

	maskBlurShader.bind();
	maskBlurShader.uniform("tex", 0);
	maskBlurShader.uniform("mask", 1);

	gl::pushMatrices();			
		gl::translate(kinect->viewShiftX, kinect->viewShiftY);
		gl::scale(kinect->headScale, kinect->headScale );
		//
		gl::enableAlphaBlending();
			gl::draw( mesh);
		gl::disableAlphaBlending();
		//faceMask->getTexture().unbind();
	gl::popMatrices();		
	
	maskBlurShader.unbind();

	faceMask->getTexture().unbind();
	faceGradientMask.unbind();
}


void Face::save()
{

}

void Face::changeItem()
{
	FaceObject *head;

	int idToChanage= -1;

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

	if (idToChanage!=-1)
	{
		MsKinect::Face face = kinect->getFace(idToChanage);
		faceController.killFaceObject(idToChanage);
		head	= faceController.getFaceObject(idToChanage, face);
	}
}

void Face::changeSize(std::string value)
{

}



/*
void Face::resize()
{
	float w = 1000;
	float h = (w/1920.0)*1080.0;
	int _h = floor(h);

	screenshotSmall = ci::Surface( w, _h , false ); 
	ci::ip::resize( screenshot, screenshot.getBounds(), &screenshotSmall, screenshotSmall.getBounds(), ci::FilterBox() );	
}*/
