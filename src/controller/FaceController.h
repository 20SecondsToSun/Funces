#pragma once
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Json.h"
#include "HeadObject.h"
#include "KinectAdapter.h"
#include "cinder/ImageIo.h"

class FaceObject;
class FaceController
{
	public:
		static const std::string  DATA_BASE_NAME;
		static const std::string  FACE_STORAGE_FOLDER;
		static const std::string  JSON_STORAGE_FOLDER;
		

		static void						loadFaces();
		static void						saveFace();

		static void						prepareFaceToSave(TriMesh2d mesh, gl::Texture surf, float headScale);

		static std::string				genericName;


		FaceObject*						getFaceObject(int id, MsKinect::Face face);
		FaceObject*						getFaceDyingObject(int id);
		void							cleanFaces();
		void							initFaces();
		void							killFaceObject(int id);



		static bool						isScanned;	
		
		

	private:
	
		FaceObject*						chooseNewFace(int id);
		static std::vector<FaceObject>	facesStoreVector;
		FaceObject*						player[kinectDefaults::FACES_MAX];
		int								playersFacesNums[kinectDefaults::FACES_MAX];

		
};


class FaceObject 
{
	public :
		FaceObject(){currentAlpha = 1.0f;};

		void	setTexName(std::string _name){ name = _name;};
		void	setPoints(std::vector<ci::Vec2f> _rawPoints){ rawPoints = _rawPoints;};
		void	setTexture(gl::Texture _tex){ texture = _tex;};
		void	setMesh2D(TriMesh2d _mesh){ mesh = _mesh;};

		std::string	getTexName()  { return name; };
		std::vector<ci::Vec2f>	getPoints(){ return rawPoints; };
		gl::Texture	getTexture()  { return texture; };
		TriMesh2d getMesh2D()  { return mesh; };

		
		bool isAlive()
		{			
			return currentAlpha> 0.f;
		}

		void addPerish()
		{
			currentAlpha -= 0.02f;
		}
		

	private:
		std::string name;
		gl::Texture texture;
		std::vector<ci::Vec2f> rawPoints;

		gl::Texture  faceGradientMask;

		TriMesh2d mesh;

		float		currentAlpha;
};