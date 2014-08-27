#pragma once

#include "cinder/app/AppBasic.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"

class Clone {
public:
	void setup(int width, int height);
	void setStrength(int strength);
	void update(ci::gl::Texture& src, ci::gl::Texture& dst, ci::gl::Texture& mask);
	void draw(float x, float y);
	
protected:
	void maskedBlur(ci::gl::Texture& tex, ci::gl::Texture& mask, ci::gl::Fbo& result);
	ci::gl::Fbo buffer, srcBlur, dstBlur;
	ci::gl::GlslProg maskBlurShader, cloneShader;
	int strength;
};