#pragma once

#include "cinder/app/AppNative.h"
#include "cinder/Json.h"
#include "Curl.h"
#include "cinder/ImageIo.h"
#include "cinder/Base64.h"
#include "FaceController.h"
#include "ScreenshotHolder.h"

namespace  serverParams 
{
	const std::string    serverURL		 =  "http://funces.ru/_bkp/save.php";
	const std::string    savePhotoName   =  "photo.jpg";
}

class Server
{
	public:
		void										sendPhoto();
		void										sendToServerImage();
		std::shared_ptr<std::thread>				serverThread;

		bool										CONNECTION_PROBLEM;

		boost::signals2::signal<void(void)>			serverHandler;

		std::string									getBuffer(){return buffer;};
		std::string									getLink(){return link;};
		bool										isResponseOK(){return !CONNECTION_PROBLEM;};

	private:
		std::string									buffer, link;
		
};