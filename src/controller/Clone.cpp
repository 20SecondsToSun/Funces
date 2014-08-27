#include "Clone.h"

using namespace ci;

const char *vertexShaderGlsl =
"void main()\n"
"{\n"
"}\n";

const char * maskBlurShaderSource =
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

const char * cloneShaderSource = 
"#extension GL_ARB_texture_rectangle : enable\n"
"uniform sampler2DRect src, srcBlur, dstBlur;\
void main() {\
	vec2 pos = gl_TexCoord[0].st;	\
	vec4 srcColorBlur = texture2DRect(srcBlur, pos);\
	if(srcColorBlur.a > 0.) {\
		vec3 srcColor = texture2DRect(src, pos).rgb;\
		vec4 dstColorBlur = texture2DRect(dstBlur, pos);\
		vec3 offset = (dstColorBlur.rgb - srcColorBlur.rgb);\
		gl_FragColor = vec4(srcColor + offset, 1.);\
	} else {\
		gl_FragColor = vec4(0.);\
	}\
}";

void Clone::setup(int width, int height) 
{
	buffer = gl::Fbo( 640, 480 );
	srcBlur = gl::Fbo( 640, 480 );
	dstBlur = gl::Fbo( 640, 480 );

	maskBlurShader = gl::GlslProg ( vertexShaderGlsl, maskBlurShaderSource );
	cloneShader    = gl::GlslProg ( vertexShaderGlsl, cloneShaderSource );
	
	strength = 0;
}

void Clone::maskedBlur(ci::gl::Texture& tex, ci::gl::Texture& mask,  ci::gl::Fbo& result) {
	int k = strength;

	tex.bind(0);
	mask.bind(1);


	gl::SaveFramebufferBinding bindingSaver;

	buffer.bindFramebuffer();
	Area saveView = gl::getViewport();
	gl::setViewport(buffer.getBounds() );	
	
	maskBlurShader.bind();
	
	maskBlurShader.uniform("tex", 0);
	maskBlurShader.uniform("mask", 1);
	maskBlurShader.uniform("direction", Vec2f(1, 0));
	maskBlurShader.uniform("k", k);
	gl::enableAlphaBlending();
	gl::draw(tex);
	gl::disableAlphaBlending();
	maskBlurShader.unbind();

	gl::setViewport(saveView);






	result.bindFramebuffer();
	saveView = gl::getViewport();
	gl::setViewport(buffer.getBounds() );		

	maskBlurShader.bind();
	maskBlurShader.uniform("tex", 0);
	maskBlurShader.uniform("mask", 1);
	maskBlurShader.uniform("direction", Vec2f(0, 1));
	maskBlurShader.uniform("k", k);
	gl::enableAlphaBlending();
	gl::draw(buffer.getTexture());
	gl::disableAlphaBlending();
	maskBlurShader.unbind();

	gl::setViewport(saveView);
}

void Clone::setStrength(int strength) {
	this->strength = strength;
}

void Clone::update(gl::Texture& src, gl::Texture& dst, gl::Texture& mask) {
	maskedBlur(src, mask, srcBlur);
	maskedBlur(dst, mask, dstBlur);

	src.bind(0);
	srcBlur.bindTexture(1);
	dstBlur.bindTexture(2);



	gl::SaveFramebufferBinding bindingSaver;

	buffer.bindFramebuffer();
	Area saveView = gl::getViewport();
	gl::setViewport(buffer.getBounds() );	
	
	
	gl::pushMatrices();
	gl::enableAlphaBlending();
	gl::draw(dst);
	cloneShader.bind();
	cloneShader.uniform("src", 0);
	cloneShader.uniform("srcBlur", 1);
	cloneShader.uniform("dstBlur", 2);
	gl::draw(dst);
	cloneShader.unbind();
	gl::disableAlphaBlending();
	gl::popMatrices();

	gl::setViewport(saveView);
}

void Clone::draw(float x, float y) {
	
	gl::draw(buffer.getTexture());
}
