#include "FaceScreen.h"

using namespace myFaceScreen;

int Face::lowTri = 0;
int Face::highTri = 120;





void Face::init() 
{


	 char * maskBlurShaderSource =
"#extension GL_ARB_texture_rectangle : enable\n"
"uniform sampler2DRect tex, mask;\
uniform vec2 direction;\
uniform int k;\
void main() {\
	vec2 pos = gl_TexCoord[0].st;\
	vec4 sum = texture2DRect(tex, pos);\
	int i;\
	for(i = 1; i < k; i++) {\
		vec2 curOffset = float(i) * direction;\
		vec4 leftMask = texture2DRect(mask, pos - curOffset);\
		vec4 rightMask = texture2DRect(mask, pos + curOffset);\
		bool valid = leftMask.r == 1. && rightMask.r == 1.;\
		if(valid) {\
			sum +=\
				texture2DRect(tex, pos + curOffset) +\
				texture2DRect(tex, pos - curOffset);\
		} else {\
			break;\
		}\
	}\
	int samples = 1 + (i - 1) * 2;\
	gl_FragColor = sum / float(samples);\
}";
char *vertexShaderGlsl =
"void main()\n"
"{\n"
"gl_TexCoord[0] = gl_MultiTexCoord0;\n"
"	gl_Position = ftransform();\n"
"}\n";











	calculateAspects();
	tracker.setup();
	loadFace();

	gl::Fbo::Format format;
	format.setSamples( 4 ); // uncomment this to enable 4x antialiasing
	//format.enableColorBuffer();
	//format.setColorInternalFormat( GL_RGB32F );
	srcFbo = gl::Fbo( kinect->getColorResolutionRectf().x2, kinect->getColorResolutionRectf().y2, format );
	maskFbo = gl::Fbo( 640, 480 );
	
	isSave = false;

	m_Cam.lookAt(Vec3f(0, 0, 200), Vec3f::zero(), Vec3f(0, 1, 0));	

	blurEdgeShader = gl::GlslProg( loadAsset( "shader/edge_blending_vert.glsl"),  loadAsset( "shader/edge_blending_frag.glsl") );	

	mShader = gl::GlslProg::create( loadAsset( "shader/passThru_vert.glsl"),  loadAsset( "shader/gaussianBlur_frag.glsl"));


	maskBlurShader = gl::GlslProg ( vertexShaderGlsl, maskBlurShaderSource );

	blankTex = *AssetManager::getInstance()->getTexture("face.png");


	mParams = params::InterfaceGl::create( getWindow(), "App parameters", toPixels( Vec2i( 200, 400 ) ) );
		// Setup the parameters
	mParams->addParam( "lowTri", &lowTri ).min( 1 ).max( 206 ).step(1 );	
	mParams->addSeparator();
	mParams->addParam( "highTri", &highTri ).min( 1 ).max( 206 ).step(1 );	


	savetex = *AssetManager::getInstance()->getTexture("face.png");
}



void Face::clean() 
{

}

void Face::update() 
{
	gl::Texture tex  = kinect->getColorTex();


	if ( isSave)
	{
		renderSceneToFboAutoFace();
	}

}

void Face::renderMaskToFBO()
{
	gl::SaveFramebufferBinding bindingSaver;

	maskFbo.bindFramebuffer();
	Area saveView = getViewport();
	gl::setViewport(maskFbo.getBounds() );

	gl::pushMatrices();
    gl::setMatricesWindow( maskFbo.getSize(), false );

	gl::Texture tex = kinect->getColorTex();

	imageFace.enableAndBind();
	imageFace.bind(1);

	tex.enableAndBind();
	tex.bind(0);
		
	maskBlurShader.bind();
	maskBlurShader.uniform("tex", 0);
	maskBlurShader.uniform("mask", 1);
	maskBlurShader.uniform("direction", Vec2f(1, 0));
	maskBlurShader.uniform("k", 10);
	gl::enableAlphaBlending();
//	gl::drawSolidRect(Rectf(0, 0, 640, 480));
	gl::draw(tex);
	gl::disableAlphaBlending();
	maskBlurShader.unbind();
   
	
	gl::setViewport(saveView);
}

void Face::renderSceneToFboAutoFace()
{
	gl::SaveFramebufferBinding bindingSaver;

	srcFbo.bindFramebuffer();
	Area saveView = getViewport();
	gl::setViewport(srcFbo.getBounds() );

	gl::pushMatrices();
    gl::setMatricesWindow( srcFbo.getSize(), false );
    gl::clear( ColorA( 0, 0, 0, 0 ) );
	gl::enableAlphaBlending();
	
	MsKinect::Face face1 = kinect->getFace(0);

	gl::Texture tex = kinect->getColorTex();
	TriMesh2d mesh1 = 	 face1.getMesh2d();
	
	  for (int i = 0; i < mesh1.getNumVertices(); i++)	  
		  mesh1.appendTexCoord(Vec2f(meshsave.getVertices()[i].x/ kinect->getColorResolutionRectf().x2, meshsave.getVertices()[i].y/ kinect->getColorResolutionRectf().y2)); 

	gl::Texture tex1 = gl::Texture(640, 480);
	gl::draw( tex);
	
	gl::pushMatrices();
		
		if (imageFace)
		{
			imageFace.enableAndBind();
				gl::draw( mesh1);
			imageFace.unbind();
		}
		else
		{
			savetex.enableAndBind();
				gl::draw( mesh1);
			savetex.unbind();
		}
	gl::popMatrices();	

			




	gl::popMatrices();		
	gl::setViewport(saveView);
}



void Face::renderSceneToFbo()
{
	gl::SaveFramebufferBinding bindingSaver;

	srcFbo.bindFramebuffer();
	Area saveView = getViewport();
	gl::setViewport(srcFbo.getBounds() );

	gl::pushMatrices();
    gl::setMatricesWindow( srcFbo.getSize(), false );
    gl::clear( Color( 0, 0, 0 ) );
	
	MsKinect::Face face1 = kinect->getFace(0);
	MsKinect::Face face2 = kinect->getFace(1);

	gl::Texture tex = kinect->getColorTex();
	TriMesh2d mesh1 = 	 face1.getMesh2d();
	TriMesh2d mesh2 = 	 face2.getMesh2d();

	  for (int i = 0; i < mesh1.getNumVertices(); i++)	  
		  mesh1.appendTexCoord(Vec2f(mesh2.getVertices()[i].x/ 640.0f, mesh2.getVertices()[i].y/ 480.0f)); 

	   for (int i = 0; i < mesh2.getNumVertices(); i++)	  
		  mesh2.appendTexCoord(Vec2f(mesh1.getVertices()[i].x/ 640.0f, mesh1.getVertices()[i].y/ 480.0f)); 



	  
	tex.enableAndBind();
	glEnable(GL_BLEND);  	 
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	gl::draw( tex);
	gl::pushMatrices();
	//gl::scale(2,2);
	gl::draw( mesh1);
	gl::draw( mesh2);
	gl::popMatrices();
	glDisable(GL_BLEND);  
	tex.unbind();		
	gl::popMatrices();
	
	gl::setViewport(saveView);
}

void Face::draw() 
{
	drawKinectCameraColorSurface();

	drawFaceMesh() ;

	if (isSave)
	{
		gl::pushMatrices();
			gl::translate(viewShiftX, viewShiftY);	
			gl::draw( srcFbo.getTexture(), srcFbo.getTexture().getBounds(), Rectf(0, 0, float(viewWidth),float(viewHeight)));				
		gl::popMatrices();	
	}

	if (imageFace)
	{
		//renderMaskToFBO();
		//gl::draw( maskFbo.getTexture());
	
/*
		blurEdgeShader.bind();
		blurEdgeShader.uniform( "exponent", 10.0f );//uniform float exponent; // try: 2.0;
		blurEdgeShader.uniform( "gamma",ci::Vec3f(1.8f, 1.5f, 1.2f) );//uniform vec3 gamma; // try: vec3(1.8, 1.5, 1.2);
		blurEdgeShader.uniform( "luminance", 0.5f );//uniform vec3 luminance; // try: vec3(0.5);
		blurEdgeShader.uniform( "edges", ci::Vec4f(0.5f, 0.5f, 0.5f, 0.5f));//0.15f, 0.05f, 0.15f, 0.05f));//uniform vec4 edges; // try: vec4(0.4, 0.4, 0.0, 0.0);

		imageFace.enableAndBind();
		//imageFace.enableAndBind();
			gl::drawSolidRect(Rectf(0, 0, 640, 480));
		//imageFace.unbind();
		imageFace.unbind();

		blurEdgeShader.unbind();*/







	}




	// Draw the interface
	mParams->draw();
}

void Face::drawFaceMesh() 
{
	
	for (int i = 0, maxfaces = kinect->maxFaces(); i < maxfaces; i++)
	{		
		if (kinect->isFaceDetected(i))
		{
			MsKinect::Face face = kinect->getFace(i);

			gl::pushMatrices();			
			gl::translate(viewShiftX, viewShiftY);
			gl::scale(headScale, headScale );
			//gl::color( ColorAf::white() );
			gl::enableWireframe();
			//console()<<" coord  "<<face.getMesh2d().<<endl;
			//kinect->getColorTexRef()->enableAndBind();	

				//gl::setMatrices(m_Cam);
				//gl::pushModelView();

			TriMesh2d mesh = face.getMesh2d();
			
			gl::draw( face.getMesh2d());
				//gl::popModelView();



			gl::disableWireframe();

			gl::color(Color::hex(0xff0000));
			//for (int j = 68; j <77; j++)// face.getMesh2d().getVertices().size(); j++)
			//{
			//	gl::drawSolidCircle(face.getMesh2d().getVertices()[j], 5);
			//}
			
			//kinect->getColorTexRef()->unbind();
			gl::color(Color::white());
			
			gl::popMatrices();
		}
	}
}


void Face::loadFace()
{	
	//faceTexture		= *AssetManager::getInstance()->getTexture( "faces/pitt.jpg", false );	
	//tracker.update(toOcv(faceTexture));
//	vector<Vec3f> srcPoints = tracker.getImagePoints();
//	console() << "  face :::::::::::  "<<srcPoints.size()<<endl;

	
	
}
bool _load = false;
void Face::save()
{

	if (_load)
	{
		console()<<"!!!!!!!!!!!"<< _load<<std::endl;
		imageFace = loadImage("face.png");
	}

	if (isSave && !_load)
	{
		MsKinect::Face face1 = kinect->getFace(0);
		savetex = kinect->getColorTex();
		meshsave = 	 face1.getMesh2d();
		writeImage( "face.png", srcFbo.getTexture() );
		_load = true;
	}

	if (kinect->isFaceDetected(0) && !_load)
	{
		savetex = kinect->getColorTex();
		MsKinect::Face face1 = kinect->getFace(0);
		meshsave = 	 face1.getMesh2d();
		isSave = true;			
	}

	

}