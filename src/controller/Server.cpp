#include "Server.h"

using namespace mndl::curl;

std::string   Server::sendPhoto()
{
	return "";
}

/*
void Server::SendToServerImage(string _name )
{	
	Params::link = "none";

	fs::path path = Params::getPhotosStorageDirectory();
	DataSourceRef urlRequest =	loadFile( path / fs::path( _name ));	

	Buffer bf = Buffer(urlRequest);
	string strj = toBase64(bf) + to_string(BIG_PHOTO_HEIGHT);

	string jSonstr =  Curl::postImage( SERVER"/save.php", strj);
	
	 if (jSonstr.length()>=1)
	 {
		JsonTree jTree;
		try 
		{
			jTree = JsonTree(jSonstr);
			string success = jTree.getChild("success").getValue();
		
			if (success=="1")
			{
				console()<<"SERVER SEND ALL FUCKING DATA"<<std::endl;
			
				Params::sessionId = jTree.getChild("id").getValue();	
				Params::link = jTree.getChild("link").getValue();
			
				Buffer buff = fromBase64(jTree.getChild("data").getValue());
				qrCodeBlock.setTextureString(jTree.getChild("data").getValue());	

				qrCodeBlock.setLink(jTree.getChild("link").getValue(), dayFont);
				qrCodeBlock.isReady = true;
				PhotoUploadReady = true;
				CONNECTION_PROBLEM = false;
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
}*/

