#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/Fbo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const float kScreenDimension = 800;

class ShaderTestApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
    
    gl::GlslProgRef     mFisheyeShader;
    gl::GlslProgRef     mTextureShader;
    gl::TextureRef      mParticleTexture;
    ci::gl::VaoRef      mScreenVao;
    ci::gl::VboRef      mScreenVbo;
    ci::gl::FboRef      mFbo;
};

void ShaderTestApp::setup()
{
    setWindowSize(kScreenDimension, kScreenDimension);
    setFrameRate(30);
    
    mParticleTexture = gl::Texture::create(loadImage(getResourcePath("particle.png")));

    gl::GlslProg::Format shaderFormat;
    shaderFormat.vertex( ci::app::loadResource( "fisheye.vert" ) )
    .fragment( ci::app::loadResource( "fisheye.frag" ) );
    mFisheyeShader = ci::gl::GlslProg::create( shaderFormat );
    
    shaderFormat.vertex( ci::app::loadResource( "texture.vert" ) )
    .fragment( ci::app::loadResource( "texture.frag" ) );
    mTextureShader = ci::gl::GlslProg::create( shaderFormat );
    
    mFbo = ci::gl::Fbo::create(kScreenDimension, kScreenDimension);

    GLfloat data[8+8+16]; // verts, texCoords, colors
    GLfloat *verts = data, *texCoords = data + 8, *color = data + 16;
    const float r = 1.f, g = 1.f, b = 1.f, a = 1.f;
    for (int i = 0; i < 4; ++i)
    {
        color[i*4+0] = r;
        color[i*4+1] = g;
        color[i*4+2] = b;
        color[i*4+3] = a;
    }
    verts[0*2+0] = 1.0f;
    verts[0*2+1] = 0.0f;
    texCoords[0*2+0] = 1.0;
    texCoords[0*2+1] = 1.0;
    
    verts[1*2+0] = 0.0f;
    verts[1*2+1] = 0.0f;
    texCoords[1*2+0] = 0.0;
    texCoords[1*2+1] = 1.0;
    
    verts[2*2+0] = 1.0f;
    verts[2*2+1] = 1.0f;
    texCoords[2*2+0] = 1.0;
    texCoords[2*2+1] = 0.0;
    
    verts[3*2+0] = 0.0f;
    verts[3*2+1] = 1.0f;
    texCoords[3*2+0] = 0.0;
    texCoords[3*2+1] = 0.0;
    
    mScreenVao = gl::Vao::create();
    mScreenVao->bind();
    mScreenVbo = gl::Vbo::create( GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW );
    mScreenVbo->bind();
    
    int posLoc = mFisheyeShader->getAttribSemanticLocation( geom::Attrib::POSITION );
    gl::enableVertexAttribArray( posLoc );
    gl::vertexAttribPointer( posLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );
    
    int texLoc = mFisheyeShader->getAttribSemanticLocation( geom::Attrib::TEX_COORD_0 );
    gl::enableVertexAttribArray( texLoc );
    gl::vertexAttribPointer( texLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)*8) );
    
    int colorLoc = mFisheyeShader->getAttribSemanticLocation( geom::Attrib::COLOR );
    gl::enableVertexAttribArray( colorLoc );
    gl::vertexAttribPointer( colorLoc, 4, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)*16) );
    
    mScreenVao->unbind();
    mScreenVbo->unbind();
}

void ShaderTestApp::mouseDown( MouseEvent event )
{
}

void ShaderTestApp::update()
{
}

void ShaderTestApp::draw()
{
    mFbo->bindFramebuffer();
    
	gl::clear( Color( 0, 0.1, 0.2 ) );
    gl::pushMatrices();
    
    gl::enableAdditiveBlending();
   
    float circleRad = 4.0f;

    gl::color(1, 1, 1, 0.5);
    
    const int kNumParticles = 500;
    for (int i = 0; i < kNumParticles; i++)
    {
        float x = cos((abs((int)(kNumParticles - (getElapsedFrames() % kNumParticles)))) * 0.5 + (i * 0.5));
        float y = sin((abs((int)(kNumParticles - (getElapsedFrames() % kNumParticles)))) * 0.5 + (i * 0.5));
        float xTwerk = 0.0001f;
        float yTwerk = 0.0001f;
        x = cos((getElapsedFrames() + i) % kNumParticles) + (getElapsedFrames() * xTwerk);
        y = sin((getElapsedFrames() + i) % kNumParticles) + (getElapsedFrames() * yTwerk);
        
        float dist = ((getElapsedFrames() + i + (int)(getElapsedFrames() * 4.1)) % (int)(kScreenDimension * 0.5));
        
        Vec2f pSpot = Vec2f(kScreenDimension * 0.5, kScreenDimension * 0.5) + (Vec2f(x,y) * dist);
        gl::draw(mParticleTexture, Rectf(pSpot.x - circleRad, pSpot.y - circleRad, pSpot.x + circleRad, pSpot.y + circleRad));
    }
    
    gl::popMatrices();
    mFbo->unbindFramebuffer();
    
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
    gl::pushMatrices();

    gl::scale(kScreenDimension, kScreenDimension);
    
    mFisheyeShader->bind();
    mFbo->bindTexture();
    
    mFisheyeShader->uniform("uAperture", 180.0f);
    
    mScreenVao->bind();
    mScreenVbo->bind();
    
    gl::setDefaultShaderVars();
    gl::drawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    
    mScreenVao->unbind();
    mScreenVbo->unbind();
    
    mFbo->unbindTexture();
    mFisheyeShader->unbind();
    
    gl::popMatrices();
}

CINDER_APP_NATIVE( ShaderTestApp, RendererGl )
