#include "FaceController.h"
#include "cinder/Rand.h"

const std::string  FaceController::DATA_BASE_NAME =  "faces.json";
const std::string  FaceController::FACE_STORAGE_FOLDER =  "faces/images";
const std::string  FaceController::JSON_STORAGE_FOLDER =  "faces/coords";


std::vector<FaceObject>	FaceController::facesStoreVector;

std::string  FaceController::genericName = "";


FaceObject*	FaceController::getFaceObject(int id, MsKinect::Face face)
{	
	if (facesStoreVector.size() == 0) return NULL;

	if (player[id] == NULL)
	{		
		player[id] = new FaceObject();	
		FaceObject* faceObj = chooseNewFace(id);		
		player[id]->setPoints(  faceObj->getPoints());
		player[id]->setTexName( faceObj->getTexName());
		player[id]->setTexture( faceObj->getTexture());

	}		
	
	return player[id];
}

FaceObject*	FaceController::getFaceDyingObject(int id)
{
	if (player[id] == NULL) return NULL;

	if (player[id]->isAlive())
	{
		player[id]->addPerish();		
	}
	else
	{
		delete player[id];
		player[id] = NULL;		
	}

	return player[id];
}

void	FaceController::killFaceObject(int id)
{
	if (player[id] != NULL)
	{
		delete player[id];
		player[id] = NULL;	
	}
}

FaceObject*	FaceController::chooseNewFace(int id)
{
	int faceID = playersFacesNums[id];	
	playersFacesNums[id]++;	
	if (playersFacesNums[id] >= facesStoreVector.size()) playersFacesNums[id] = 0;

	return &facesStoreVector[faceID];	
}

void	FaceController::cleanFaces()
{
	for (int i = 0; i < kinectDefaults::FACES_MAX; i++)	
		killFaceObject(i);
}

void FaceController::initFaces()
{
	for (int i = 0; i < kinectDefaults::FACES_MAX; i++)
	{
		player[i] = NULL;
		playersFacesNums[i] =  Rand::randInt( facesStoreVector.size());	
	}	
}

void FaceController::loadFaces()
{
	/*

	JsonTree faces( doc.getChild( "faces" ) );
	
	facesStoreVector.clear();

	for( JsonTree::ConstIter face = faces.begin(); face != faces.end(); ++face ) {
		
		FaceObject newFace;	
		vector<Vec2f> points;

		string name =  face->getChild( "texname" ).getValue<string>();
		gl::Texture tex;
		try{
			tex = loadImage(getAppPath() /FACE_STORAGE_FOLDER/name);
		}
		catch(...)
		{
			continue;
		}



		JsonTree datas =JsonTree( face->getChild( "data" ));
		for( JsonTree::ConstIter data = datas.begin(); data != datas.end(); ++data ) {
			float x =  data->getChild( "x" ).getValue<float>();
			float y =  data->getChild( "y" ).getValue<float>();

			points.push_back(Vec2f(x,y));
		}
		newFace.setPoints(points);

		
		newFace.setTexName(name);

		
		newFace.setTexture(tex);
		
		facesStoreVector.push_back(newFace);
	}	*/


	facesStoreVector.clear();

	string path = getAppPath().string() + JSON_STORAGE_FOLDER;
	fs::path p(path);

	

	for (fs::directory_iterator it(p); it != fs::directory_iterator(); ++it)
	{
		if (fs::is_regular_file(*it))
		{
			JsonTree doc;
			try{
				 doc = JsonTree(loadFile(getAppPath() / JSON_STORAGE_FOLDER / it->path().filename().string()));
			}
			catch(...)
			{
				return;
			}

			JsonTree faces( doc.getChild( "faces" ) );

			for( JsonTree::ConstIter face = faces.begin(); face != faces.end(); ++face )
			{		
				FaceObject newFace;	
				vector<Vec2f> points;

				string name =  face->getChild( "texname" ).getValue<string>();
				gl::Texture tex;
				try{
					tex = loadImage(getAppPath() /FACE_STORAGE_FOLDER/name);
				}
				catch(...)
				{
					continue;
				}

				JsonTree datas =JsonTree( face->getChild( "data" ));
				for( JsonTree::ConstIter data = datas.begin(); data != datas.end(); ++data )
				{
					float x =  data->getChild( "x" ).getValue<float>();
					float y =  data->getChild( "y" ).getValue<float>();

					points.push_back(Vec2f(x,y));
				}

				newFace.setPoints(points);		
				newFace.setTexName(name);

				newFace.setTexture(tex);		
				facesStoreVector.push_back(newFace);
			}
		}
	}

}



void FaceController::prepareFaceToSave(TriMesh2d mesh, gl::Texture surf, float headScale)
{
	genericName = "surf_"+to_string(facesStoreVector.size()) + ".png";	

	Rectf rect =	mesh.calcBoundingBox();	

	vector<ci::Vec2f> savecords;
	savecords.clear();
	for (int i = 0; i < mesh.getNumVertices(); i++)
		 savecords.push_back(Vec2f( ( mesh.getVertices()[i].x-rect.x1 )/surf.getWidth(), (mesh.getVertices()[i].y -rect.y1)/surf.getHeight())); 	
	
	FaceObject newface;
	newface.setPoints(savecords);
	newface.setTexName(genericName);
	newface.setTexture(surf);

	facesStoreVector.push_back(newface);
	writeImage( getAppPath() /FACE_STORAGE_FOLDER/genericName, surf);
}

void FaceController::saveFace()
{/*
 for (size_t i = 0, ilen= facesStoreVector.size(); i<ilen; i++)
	{
		JsonTree oneFaceJson;
		oneFaceJson.addChild( JsonTree( "texname", facesStoreVector[i].getTexName() ) );

		JsonTree facesDataJson = JsonTree::makeArray( "data" );

		auto points =  facesStoreVector[i].getPoints();
		for (size_t j = 0, ilen = points.size() ; j < ilen; j++)
		{
			JsonTree point;
			point.addChild(JsonTree( "x",points[j].x) );	
			point.addChild(JsonTree( "y",points[j].y) );	
			facesDataJson.pushBack(point);
		}
		oneFaceJson.addChild(facesDataJson );

		facesJson.pushBack(oneFaceJson);
	}

	doc.pushBack( facesJson );
	
	writeImage( getAppPath() /FACE_STORAGE_FOLDER/genericName, surf );
	doc.write( writeFile( getAppPath() / DATA_BASE_NAME ), JsonTree::WriteOptions() );	*/

	// SAVE LAST ONLY!!!!


	JsonTree doc;
	JsonTree facesJson = JsonTree::makeArray( "faces" );

	int id = facesStoreVector.size() - 1;

	JsonTree oneFaceJson;
	oneFaceJson.addChild( JsonTree( "texname", facesStoreVector[id].getTexName() ) );

	JsonTree facesDataJson = JsonTree::makeArray( "data" );

		auto points =  facesStoreVector[id].getPoints();
		for (size_t j = 0, ilen = points.size() ; j < ilen; j++)
		{
			JsonTree point;
			point.addChild(JsonTree( "x",points[j].x) );	
			point.addChild(JsonTree( "y",points[j].y) );	
			facesDataJson.pushBack(point);
		}
	oneFaceJson.addChild(facesDataJson );
	facesJson.pushBack(oneFaceJson);

	doc.pushBack( facesJson );

	string jsonName = "base_"+to_string(id) + ".json";	
	
	
	doc.write( writeFile( getAppPath() / JSON_STORAGE_FOLDER / jsonName ), JsonTree::WriteOptions() );

}