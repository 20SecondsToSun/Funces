#include "Server.h"

using namespace mndl::curl;
using namespace std;
using namespace ci;

void   Server::sendPhoto()
{
	serverThread = shared_ptr<thread>( new thread( bind( &Server::sendToServerImage, this ) ) );	
}

void Server::sendToServerImage( )
{	
	ci::ThreadSetup threadSetup; // instantiate this if you're talking to Cinder from a secondary thread	

	writeImage( getAppPath() /FaceController::FACE_STORAGE_FOLDER/serverParams::savePhotoName, ScreenshotHolder::screenshotSmall );
	DataSourceRef urlRequest =	loadFile( getAppPath()/FaceController::FACE_STORAGE_FOLDER/ fs::path( serverParams::savePhotoName ));	

	string strj				 = toBase64(Buffer(urlRequest));
	string jSonstr			 =  Curl::postImage(  serverParams::serverURL, strj);
	
	 if (jSonstr.length()>=1)
	 {
		JsonTree jTree;
		try 
		{
			jTree = JsonTree(jSonstr);
			string success = jTree.getChild("success").getValue();
		
			if (success=="1")
			{
				console()<<"SERVER SEND ALL FUCKING DATA  "<<jSonstr<< std::endl;
				link	= jTree.getChild("link").getValue();
				buffer	 = jTree.getChild("data").getValue();
				
				CONNECTION_PROBLEM = false;
				serverThread->detach();
				serverHandler();
				return;
			}
			else  CONNECTION_PROBLEM = true;

		}
		catch (... )
		{
			 CONNECTION_PROBLEM = true;
		}
	 }
	 console()<<"==========================  SERVER ERROR SORRY.....==============================="<<std::endl;
	 CONNECTION_PROBLEM = true;
	 serverThread->detach();
	 serverHandler();
}