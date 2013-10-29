#include "cinder/app/AppNative.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/Fbo.h"
#include "cinder/qtime/QuickTimeGl.h"
#include "AudioAurora.h"
#include "StreakingStars.h"
#include "Constellations.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class PlanetariumAudioApp : public AppNative
{

public:

    void prepareSettings(cinder::app::AppBasic::Settings *settings);
    
    void loadAudio();
    void loadFBO();
    void loadShaders();
	void setup();

	void mouseDown( MouseEvent event );
	
    void update();
	
    void clearFBO(gl::FboRef & fbo, const float alpha);
    void draw();
    void renderAudioReaction(bool useFBO);
    void renderAudioHeightmap();
    void renderStreakingStars(bool useFBO);
    void renderConstellations(bool useFBO);
    void renderDomeRing();
    void drawFBO(gl::FboRef & fbo);
    
    float               mPrevFFTData[kNumFFTChannels];

    gl::GlslProgRef     mFisheyeShader;
    gl::GlslProgRef     mTextureShader;
    
    gl::TextureRef      mParticleTexture;
    
    ci::gl::VaoRef      mScreenVao;
    ci::gl::VboRef      mScreenVbo;
    
    ci::gl::FboRef      mFboAurora;
    ci::gl::FboRef      mFboStars;
    ci::gl::FboRef      mFboConstellations;
    
    qtime::MovieGlRef   mSoundtrack;
    AudioAurora         mAurora;
    StreakingStars      mStreakingStars;
    Constellations      mConstellations;
    
};

void PlanetariumAudioApp::prepareSettings(cinder::app::AppBasic::Settings *settings)
{
    settings->setFullScreen(true);
    /*
#ifdef DEBUG
    settings->setWindowSize(kScreenDimension, kScreenDimension);
#else
    settings->setFullScreen(true);
#endif
    */
}

void PlanetariumAudioApp::loadShaders()
{
    gl::GlslProg::Format shaderFormat;
    shaderFormat.vertex( ci::app::loadResource( "fisheye.vert" ) )
    .fragment( ci::app::loadResource( "fisheye.frag" ) );
    mFisheyeShader = ci::gl::GlslProg::create( shaderFormat );
    
    shaderFormat.vertex( ci::app::loadResource( "texture.vert" ) )
    .fragment( ci::app::loadResource( "texture.frag" ) );
    mTextureShader = ci::gl::GlslProg::create( shaderFormat );
}

void PlanetariumAudioApp::loadFBO()
{
    ci::gl::Fbo::Format format;
    format.samples(8);
    mFboStars = ci::gl::Fbo::create(kScreenDimension,
                                    kScreenDimension,
                                    format);
    
    mFboAurora = ci::gl::Fbo::create(kScreenDimension,
                                     kScreenDimension,
                                     format);
    
    mFboConstellations = ci::gl::Fbo::create(kScreenDimension,
                                             kScreenDimension,
                                             format);
    
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

void PlanetariumAudioApp::setup()
{
    kScreenDimension = getWindowHeight();

    //setWindowSize(kScreenDimension, kScreenDimension);
#ifdef DEBUG
    //setWindowSize(kScreenDimension, kScreenDimension);
#else
    //setFullScreen(true);
#endif
    
    mAurora = AudioAurora(kNumFFTChannels);
    mStreakingStars.load();
    mConstellations.load();
    
    loadAudio();
    mSoundtrack->seekToStart();
    mSoundtrack->play();

    loadShaders();
    
    mParticleTexture = gl::Texture::create(loadImage(getResourcePath("particle.png")));
    
    loadFBO();
}

void PlanetariumAudioApp::loadAudio()
{
    fs::path audioPath = getResourcePath("trimmed_a_new_error.aif");
    //fs::path audioPath = getResourcePath("a_new_error.m4a");
    //fs::path audioPath = getResourcePath("gotta_have_it.mp3");
    //fs::path audioPath = getResourcePath("reich.mp3");
    if (!fs::exists(audioPath))
    {
        console() << "ERROR: No audio path found\n";
    }
    else
    {
        console() << "Audio path: " << audioPath << endl;
    }
    mSoundtrack = qtime::MovieGl::create(audioPath);
    console() << "mSoundtrack: " << mSoundtrack << endl;
    mSoundtrack->setLoop(true);
    try
    {
        mSoundtrack->setupMonoFft( kNumFFTChannels );
    }
    catch( qtime::QuickTimeExcFft & )
    {
        console() << "Unable to setup FFT" << std::endl;
    }
    console() << "FFT Channels: " << mSoundtrack->getNumFftChannels() << std::endl;
}

void PlanetariumAudioApp::mouseDown( MouseEvent event )
{
}

void PlanetariumAudioApp::update()
{
    float *fftData = mSoundtrack->getFftData();

    for (int i = 0; i < kNumFFTChannels; ++i)
    {
        mPrevFFTData[i] = fftData[i];
    }
    mAurora.update(fftData);
    mStreakingStars.update();
    mConstellations.update(fftData);
}

void PlanetariumAudioApp::renderAudioHeightmap()
{
    gl::pushMatrices();
    gl::bindStockShader(gl::ShaderDef().color());
    Vec2i texPosition = Vec2f(kScreenDimension * 0.5, kScreenDimension * 0.5);//getWindowCenter();
    texPosition.x -= kNumFFTChannels * 0.5;
    texPosition.y -= kNumFFTChannels * 0.5;
    Rectf screenRect(0, 0, kNumFFTChannels, kNumFFTChannels);
    gl::draw(mAurora.getTexture(), screenRect);
    gl::popMatrices();
}

void PlanetariumAudioApp::clearFBO(gl::FboRef & fbo, const float alpha)
{
    fbo->bindFramebuffer();
    gl::enableAlphaBlending();
    // gl::enableAdditiveBlending();
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    gl::pushMatrices();

    gl::bindStockShader(gl::ShaderDef().color());
    gl::color(0.0f, 0.0f, 0.0f, alpha);
    gl::drawSolidRect(Rectf(0,0,kScreenDimension,kScreenDimension));

    gl::popMatrices();
    gl::disableAlphaBlending();
    fbo->unbindFramebuffer();
}

void PlanetariumAudioApp::renderConstellations(bool useFBO)
{
    gl::enableAlphaBlending();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (useFBO)
    {
        mFboConstellations->bindFramebuffer();
        //gl::enableAlphaBlending();
    }
    else
    {
        //gl::enableAdditiveBlending();
    }
    gl::pushMatrices();
    mConstellations.render();
    gl::popMatrices();
    if (useFBO)
    {
        mFboConstellations->unbindFramebuffer();
    }
    
    gl::disableAlphaBlending();
}

void PlanetariumAudioApp::renderAudioReaction(bool useFBO)
{
    if (useFBO)
    {
        mFboAurora->bindFramebuffer();
        gl::enableAlphaBlending();
    }
    else
    {
        gl::enableAdditiveBlending();
    }
    gl::pushMatrices();
    mAurora.render();
    gl::popMatrices();
    if (useFBO)
    {
        mFboAurora->unbindFramebuffer();
    }

    gl::disableAlphaBlending();
}

void PlanetariumAudioApp::renderStreakingStars(bool useFBO)
{
    gl::enableAlphaBlending();
    if (useFBO)
    {
        mFboStars->bindFramebuffer();
    }
    gl::pushMatrices();
    mStreakingStars.render();
    gl::popMatrices();
    if (useFBO)
    {
        mFboStars->unbindFramebuffer();
    }
    gl::disableAlphaBlending();
}

void PlanetariumAudioApp::drawFBO(gl::FboRef & fbo)
{
    gl::pushMatrices();
    
    gl::scale(kScreenDimension, kScreenDimension);
    
    mFisheyeShader->bind();
    fbo->bindTexture();
    
    mFisheyeShader->uniform("uAperture", 180.0f);
    
    mScreenVao->bind();
    mScreenVbo->bind();
    
    gl::setDefaultShaderVars();
    gl::drawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    
    mScreenVao->unbind();
    mScreenVbo->unbind();
    
    fbo->unbindTexture();
    mFisheyeShader->unbind();
    
    gl::popMatrices();

}

void PlanetariumAudioApp::renderDomeRing()
{
    gl::bindStockShader(gl::ShaderDef().color());
    gl::color(0.2f, 0.2f, 0.2f, 1.0f);
    gl::drawSolidCircle(Vec2f(kScreenDimension*0.5,kScreenDimension*0.5), kScreenDimension * 0.5);
}

void PlanetariumAudioApp::draw()
{
    gl::clear( Color(0,0,0) );
    
    gl::disableAlphaBlending();
    
    // FBOS
    float alpha = 0.05;
    static bool DidClearBloom = false;
    if (!DidClearBloom)
    {
        alpha = 1.0f;
        DidClearBloom = true;
    }
    clearFBO(mFboAurora, alpha);
    renderAudioReaction(true);
    
    if (alpha < 1) alpha = 0.015;
    clearFBO(mFboStars, alpha);
    renderStreakingStars(true);
    
    if (alpha < 1) alpha = 0.05;
    clearFBO(mFboConstellations, alpha);
    renderConstellations(true);
    
    drawFBO(mFboAurora);
    
    gl::enableAdditiveBlending();
    drawFBO(mFboStars);
    drawFBO(mFboConstellations);

    // Live constellations
    renderConstellations(false);
    
    // Live audio
    renderAudioReaction(false);
    
    gl::color(1,1,1);
    renderAudioHeightmap();
    
}

CINDER_APP_NATIVE( PlanetariumAudioApp, RendererGl )
