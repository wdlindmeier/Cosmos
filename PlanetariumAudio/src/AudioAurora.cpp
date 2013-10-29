//
//  AudioAurora.cpp
//  PlanetariumAudio
//
//  Created by William Lindmeier on 10/28/13.
//
//

#include "AudioAurora.h"

using namespace ci;
using namespace ci::app;

AudioAurora::AudioAurora(const int numFFTChannels) :
mNumFFTChannels(numFFTChannels),
mTimer(0)
{
    mFFTSurface = ci::Surface( mNumFFTChannels, mNumFFTChannels, false);
    // Clear surface
    for (int x = 0; x < numFFTChannels; ++x)
    {
        for (int y = 0; y < numFFTChannels; ++y)
        {
            mFFTSurface.setPixel(Vec2i(x,y), Color8u(0,0,0));
        }
    }
    mFFTTexture = ci::gl::TextureRef( new gl::Texture(mNumFFTChannels, mNumFFTChannels) );
    
    gl::GlslProg::Format shaderFormat;
    shaderFormat.vertex( ci::app::loadResource( "aurora.vert" ) )
    .fragment( ci::app::loadResource( "aurora.frag" ) );
    mShaderAurora = ci::gl::GlslProg::create( shaderFormat );
    
    loadMesh();
    
    mCam.setPerspective(45.0f, // smaller is less perspective
                        (float)getWindowWidth() / getWindowHeight(),
                        .01, 10000 );
    //mCam.lookAt( Vec3f( 0, 0, -171 ), Vec3f( 0, 0, 0 ) );
    mCam.lookAt( Vec3f( 0, 0, -171 ), Vec3f( 0, 0, 0 ) );

};

// NOTE: The coords need to be scalar to map to the
// height map. The matrix is then scaled up in draw
// to fit the scene.
const float kMeshPlaneDimension = 1.0f;
const float kMeshPlaneUnitScale = 1.0f / kNumFFTChannels;
// Arbitrary
const float kMeshPlaneScale = 100.0f; // Was 1000
//const float kGroundDepthScale = 0.075f;
const float kMeshPlaneUnit = kMeshPlaneDimension * kMeshPlaneUnitScale;
const int   kNumMeshUnitsWide = kMeshPlaneDimension / kMeshPlaneUnit;
const int   kNumMeshUnitsHigh = kMeshPlaneDimension / kMeshPlaneUnit;
const float kMeshY = 0.0f;
const int   kNumPlots = kNumMeshUnitsWide * kNumMeshUnitsHigh;
// NOTE: This could be changed for a different geometry.
// Splitting each plot into 2 triangles for now.
const int   kVertsPerPlot = 4;
const int   kNumVerts = kNumPlots * kVertsPerPlot;

void AudioAurora::loadMesh()
{
    GLfloat verts[kNumPlots * kVertsPerPlot * 3];
    
    for (int y = 0; y < kNumMeshUnitsHigh; ++y)
    {
        for (int x = 0; x < kNumMeshUnitsWide; ++x)
        {
            int plotIndex = (y * kNumMeshUnitsWide) + x;
            int idxOffset = plotIndex * kVertsPerPlot * 3; //xyz
            
            //1: TL
            verts[idxOffset + (0 * 3 + 0)] = (x * kMeshPlaneUnit);
            verts[idxOffset + (0 * 3 + 1)] = kMeshY;
            verts[idxOffset + (0 * 3 + 2)] = (y * kMeshPlaneUnit);
            
            //2: TR
            verts[idxOffset + (1 * 3 + 0)] = ((x + 1) * kMeshPlaneUnit);
            verts[idxOffset + (1 * 3 + 1)] = kMeshY;
            verts[idxOffset + (1 * 3 + 2)] = (y * kMeshPlaneUnit);
            
            //3: BL
            verts[idxOffset + (2 * 3 + 0)] = (x * kMeshPlaneUnit);
            verts[idxOffset + (2 * 3 + 1)] = kMeshY;
            verts[idxOffset + (2 * 3 + 2)] = ((y + 1) * kMeshPlaneUnit);
            
            //4: BR
            verts[idxOffset + (3 * 3 + 0)] = ((x + 1) * kMeshPlaneUnit);
            verts[idxOffset + (3 * 3 + 1)] = kMeshY;
            verts[idxOffset + (3 * 3 + 2)] = ((y + 1) * kMeshPlaneUnit);
        }
    }
    
    mMeshVao = gl::Vao::create();
    mMeshVao->bind();
    mMeshVbo = gl::Vbo::create( GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW );
    mMeshVbo->bind();
    
    int posLoc = mShaderAurora->getAttribSemanticLocation( geom::Attrib::POSITION );
    gl::enableVertexAttribArray( posLoc );
    gl::vertexAttribPointer( posLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
    
    mMeshVbo->unbind();
    mMeshVao->unbind();
}

void AudioAurora::update(const float *fftData)
{
    Surface::Iter iter = mFFTSurface.getIter();
    
    // Bump all of the values UP one.
    // The new values will go at the bottom.
    while( iter.line() )
    {
        while( iter.pixel() )
        {
            if (iter.y() < mNumFFTChannels - 1)
            {
                Vec2i curPos(iter.x(), iter.y());
                Vec2i nextPos = curPos;
                nextPos.y += 1;
                mFFTSurface.setPixel(curPos, mFFTSurface.getPixel(nextPos));
            }
            else
            {
                // Set the current value
                float val = fftData[iter.x()];
                // NOTE: Should val be multiplied?
                // val *= 2;
                
                iter.r() = iter.g() = iter.b() = ci::math<int>::clamp(val * 255, 0, 255);
            }
        }
    }
    mTimer += 0.01f;
    float camZ = -130 + (sin(mTimer) * 25.0f);
    mCam.lookAt( Vec3f( 0, 0, camZ), Vec3f( 0, 0, 0 ) );
    
    mFFTTexture->update(mFFTSurface);
}

void AudioAurora::render()
{
    // Draw a standard grid
    gl::pushMatrices();
    
    gl::setMatrices( mCam );
    // Slowly spins the visualizer
    gl::rotate(mTimer * 20, 0, 0, 1);
    
    // Center the ground
    gl::translate(Vec3f(-0.5f * kMeshPlaneScale,
                        0,
                        -0.5f * kMeshPlaneScale));
    
    
    // 0..1 -> 0..5000
    gl::scale(kMeshPlaneScale,
              kMeshPlaneScale,
              kMeshPlaneScale);
    
    mShaderAurora->bind();
    
    // Set the height texture
    mFFTTexture->bind(0);
    mShaderAurora->uniform("uTex0", 0);
    
    mMeshVao->bind();
    mMeshVbo->bind();
    
    gl::setDefaultShaderVars();
    //gl::drawArrays(GL_LINE_STRIP_ADJACENCY, 0, kNumVerts);
    gl::drawArrays(GL_LINES, 0, kNumVerts);
    //gl::drawArrays(GL_LINE_LOOP, 0, kNumVerts);
    //gl::drawArrays(GL_TRIANGLE_STRIP, 0, kNumVerts);
    
    mMeshVao->unbind();
    mMeshVbo->unbind();
    
    mFFTTexture->unbind();
    mShaderAurora->unbind();
    gl::popMatrices();

}

ci::gl::TextureRef & AudioAurora::getTexture()
{
    return mFFTTexture;
}
