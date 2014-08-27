#include "ChangeModeButton.h"

void ChangeModeButton::draw()
{
	int size = 300;
	gl::pushMatrices();
		gl::translate(getWindowWidth() - size, 0);
		gl::color(color);
		gl::drawSolidRect(Rectf(0,0,size,size));
		gl::color(Color::white());
	gl::popMatrices();

}
bool ChangeModeButton::isDown(Vec2f coords)
{
	int size = 300;
	int sdvig = getWindowWidth() - size;
	Rectf zone(sdvig,0,sdvig + size,size);

	return zone.contains(coords);
}

void ChangeModeButton::setState(std::string state)
{
	if (state ==  "HEAD")
		color = Color::hex(0xff0000);
	else
		color = Color::hex(0xffff00);
}