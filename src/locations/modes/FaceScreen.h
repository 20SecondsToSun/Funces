#pragma once
#include "ModeBaseKinect.h"
#include "AssetsManager.h"
#include "FaceController.h"

#include "cinder/gl/Fbo.h"
#include "cinder/Camera.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/params/Params.h"


using namespace ci;
using namespace ci::app;
using namespace gl;
using namespace std;

namespace myFaceScreen{

class Face : public ModeBaseKinect
{
	public :
		

		void draw();
		void update();
		void init();
		void clean();
		void save();
		void setup() ;
		void changeItem();
		void changeSize(std::string value);

		static int lowTri;
		static int highTri;


	private:
	
		bool isFacesExist();
		
		TriMesh2d meshsave;
	
		//gl::GlslProgRef	mShader;
	
		params::InterfaceGlRef	mParams;	

		gl::Texture faceGradientMask;

		FaceController faceController;

		void			drawFaceMask(FaceObject* faceMask);

		ci::gl::GlslProg    maskBlurShader;
};
}