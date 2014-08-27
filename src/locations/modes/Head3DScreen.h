#pragma once
#include "ModeBaseKinect.h"
#include "cinder/MayaCamUI.h"
#include "ModelSourceAssimp.h"
#include "SkinnedMesh.h"
#include "SkinnedVboMesh.h"
#include "SkinningRenderer.h"

#include "HeadObject.h"
#include "HeadsController.h"

using namespace ci;
using namespace ci::app;
using namespace gl;
using namespace std;

class Head3D : public ModeBaseKinect
{
	public:
		void update() ;
		void draw() ;
		void init() ;
		void clean() ;
		void save() ;
		void setup() ;
		void changeItem();
		void changeSize(std::string value);

	private:		
		void						setUpCamera();
		void						drawHeads3D();
		void						drawDyingHeads3D();
		void						drawHead(HeadObject* head);

		void						set3DLensShift(HeadObject* head );
		float						calcScalePercent(float z );
		void						onLightsForHead(HeadObject* head);
		void						offLightsForHead();

		
		int							detectedPeopleInFrameNum;	

		ci::MayaCamUI				mMayaCam;

		HeadsController             headsController;
};