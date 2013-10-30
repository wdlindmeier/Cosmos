//
//  Constellations.cpp
//  PlanetariumAudio
//
//  Created by William Lindmeier on 10/29/13.
//
//

#include "Constellations.h"
#include "Asterisms.h"

using namespace ci;
using namespace ci::app;

Constellations::Constellations() : mTimer(0)
{
}

void Constellations::load()
{
    gl::GlslProg::Format shaderFormat;
    shaderFormat.vertex( ci::app::loadResource( "constellation.vert" ) )
    .fragment( ci::app::loadResource( "constellation.frag" ) );
    mShaderConstellations = ci::gl::GlslProg::create( shaderFormat );
    
    mCam.setPerspective(45.0f, // smaller is less perspective
                        (float)getWindowWidth() / getWindowHeight(),
                        .01, 10000 );
    
    mCam.lookAt( Vec3f( 0, 0, -100 ), Vec3f( 0, 0, 0 ) );

    mTimer = 0;
    
    loadAsterisms();
}

void Constellations::addConstellation(const float *linePoints, const int numPoints)
{
    mNumConstellations++;
    mNumVerts.push_back(numPoints);
    mAlphas.push_back(1.0f);
    
    gl::VaoRef vao = gl::Vao::create();
    vao->bind();
    gl::VboRef vbo = gl::Vbo::create(GL_ARRAY_BUFFER,
                                     sizeof(float) * numPoints * 3,
                                     linePoints,
                                     GL_STATIC_DRAW);
    vbo->bind();
    
    int posLoc = mShaderConstellations->getAttribSemanticLocation( geom::Attrib::POSITION );
    gl::enableVertexAttribArray( posLoc );
    gl::vertexAttribPointer( posLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
    
    vao->unbind();
    vbo->unbind();
    
    mVaos.push_back(vao);
    mVbos.push_back(vbo);
}

void Constellations::loadAsterisms()
{
    mNumConstellations = 0;
    mNumVerts.clear();

    addConstellation(Constellation0, 22);
    addConstellation(Constellation1, 26);
    addConstellation(Constellation2, 18);
    addConstellation(Constellation3, 32);
    addConstellation(Constellation4, 36);
    addConstellation(Constellation5, 22);
    addConstellation(Constellation6, 16);
    addConstellation(Constellation7, 48);
    addConstellation(Constellation8, 6);
    addConstellation(Constellation9, 8);
    addConstellation(Constellation10, 12);
    addConstellation(Constellation11, 40);
    addConstellation(Constellation12, 28);
    addConstellation(Constellation13, 40);
    addConstellation(Constellation14, 10);
    addConstellation(Constellation15, 30);
    addConstellation(Constellation16, 2);
    addConstellation(Constellation17, 30);
    addConstellation(Constellation18, 16);
    addConstellation(Constellation19, 30);
    addConstellation(Constellation20, 12);
    addConstellation(Constellation21, 10);
    addConstellation(Constellation22, 10);
    addConstellation(Constellation23, 12);
    addConstellation(Constellation24, 32);
    addConstellation(Constellation25, 70);
    addConstellation(Constellation26, 20);
    addConstellation(Constellation27, 14);
    addConstellation(Constellation28, 2);
    addConstellation(Constellation29, 14);
    addConstellation(Constellation30, 14);
    addConstellation(Constellation31, 40);
    addConstellation(Constellation32, 16);
    addConstellation(Constellation33, 4);
    addConstellation(Constellation34, 22);
    addConstellation(Constellation35, 44);
    addConstellation(Constellation36, 34);
    addConstellation(Constellation37, 26);
}

void Constellations::update(float *fftData)
{
    int fftUnit = kNumFFTChannels / mNumConstellations;
    for (int i = 0; i < mNumConstellations; ++i)
    {
        int fftIdx = i * fftUnit;
        float alpha = fftData[fftIdx];
        mAlphas[i] = 0.5 + alpha;
    }
    
    mTimer += 0.01;
}

void Constellations::render()
{
    // Draw a standard grid
    gl::pushMatrices();
    
    // Slowly spins the visualizer
    //gl::rotate(mTimer, 0, 0, 1);
    gl::setMatrices( mCam );
    
    gl::rotate(mTimer*2, 0, 0, 1);

//    float scale = getWindowHeight();
    
    float transX = 0.5;
    float transY = 0.5;
    gl::translate(Vec2f(transX * getWindowHeight(), //getWindowWidth(),
                        transY * getWindowHeight()));

    gl::scale(getWindowHeight() * -1,
              getWindowHeight() * -1,
              getWindowHeight());
    
    mShaderConstellations->bind();
    
    for (int i = 0; i < mNumConstellations; ++i)
    {
        
        const static float kTimerFullProgress = 100.0f;
        
        int numVerts = mNumVerts[i];
        // Generic draw
        //gl::drawArrays(GL_LINES, 0, numVerts);
        
        // Draw range
        int framesVisibleMulti = 6;
        int frameSpan = numVerts * framesVisibleMulti;
        if (getElapsedFrames() % (frameSpan * 2) < frameSpan)
        {
            
            float progress = std::min<float>(mTimer / kTimerFullProgress, 1.0f);
            
            // This fades them in-and-out one-by-one
            int numShowing = 3;
            int modMe = mNumConstellations / numShowing;
            
            // The duration speeds up w/ progress
            float targetFrame = (getElapsedFrames() * 0.005 * progress);
            float modAlpha = 1.0 - (std::max<float>(fabs((i % modMe) - fmod(targetFrame, modMe)), 0.0f));
            
            // This is FFT data
            float fftAlpha = mAlphas[i];
            
            
            float alpha = modAlpha * fftAlpha * progress;
            
            // float iScalar = (float)i/(float)mNumConstellations;
            //ColorAf cColor(ColorModel::CM_HSV, iScalar, 1.0, 0.5, alpha);
            ColorAf cColor(1.0f, 1.0f, 1.0f, alpha * 0.5);
            
            mShaderConstellations->uniform("uColor", cColor);
            ci::gl::VaoRef & vao = mVaos[i];
            ci::gl::VboRef & vbo = mVbos[i];
            vao->bind();
            vbo->bind();
            
            gl::setDefaultShaderVars();

            int mod = (int)(getElapsedFrames() * progress) % frameSpan;
            int offset = std::max<int>(mod - (frameSpan - numVerts), 0);
            int count = std::min<int>(numVerts - offset, std::min<int>(mod, numVerts));
            offset -= offset % 2;
            count -= count % 2;
            gl::drawArrays(GL_LINES, offset, count);
            
            vbo->unbind();
            vao->unbind();
        }
    }
    
    mShaderConstellations->unbind();
    
    gl::popMatrices();
}