//
//  StreakingStars.cpp
//  PlanetariumAudio
//
//  Created by William Lindmeier on 10/28/13.
//
//

#include "StreakingStars.h"
#include "cinder/CinderMath.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;

// Derp. We cant load a shader with the default constructor
// because it tries loading too soon
void StreakingStars::load()
{
    gl::GlslProg::Format shaderFormat;
    shaderFormat.vertex( ci::app::loadResource( "streaks.vert" ) )
    .fragment( ci::app::loadResource( "streaks.frag" ) );
    mShaderStreaks = ci::gl::GlslProg::create( shaderFormat );

    mCam.setPerspective(45.0f, // smaller is less perspective
                        (float)getWindowWidth() / getWindowHeight(),
                        .01, 10000 );
    
    mCam.lookAt( Vec3f( 0, 0, -100 ), Vec3f( 0, 0, 0 ) );
    
    loadMesh();
}

void StreakingStars::loadMesh()
{
    mNumStars = 5000;
    
    GLfloat verts[mNumStars * 2 * (3 + 4)];
    
    for (int i = 0; i < mNumStars; ++i)
    {
        float dist = (float)i / mNumStars;
        // TMP / TEST
        int deg = -45 + (arc4random() % 360);//180);
        float rads = toRadians((float)deg);
        
        // Make a very short line
        float x1 = cos(rads) * dist;
        float y1 = sin(rads) * dist;
        float x2 = cos(rads+0.005) * dist;
        float y2 = sin(rads+0.005) * dist;

        float alpha = Rand::randFloat();
        float r = Rand::randFloat() * 5;
        float g = Rand::randFloat() * 5;
        float b = Rand::randFloat() * 5;
        
        //1:
        verts[i * 14 + 0] = x1;
        verts[i * 14 + 1] = y1;
        verts[i * 14 + 2] = 1;
        
        // Color 1
        verts[i * 14 + 3] = r;
        verts[i * 14 + 4] = g;
        verts[i * 14 + 5] = b;
        verts[i * 14 + 6] = alpha;
        
        //2:
        verts[i * 14 + 7] = x2;
        verts[i * 14 + 8] = y2;
        verts[i * 14 + 9] = 1;
        
        // Color 2
        verts[i * 14 + 10] = r;
        verts[i * 14 + 11] = g;
        verts[i * 14 + 12] = b;
        verts[i * 14 + 13] = alpha;
    }
    
    mStarsVao = gl::Vao::create();
    mStarsVao->bind();
    mStarsVbo = gl::Vbo::create( GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW );
    mStarsVbo->bind();
    
    int posLoc = mShaderStreaks->getAttribSemanticLocation( geom::Attrib::POSITION );
    gl::enableVertexAttribArray( posLoc );
    gl::vertexAttribPointer( posLoc, 3, GL_FLOAT, GL_FALSE, sizeof(float)*7, (void*)0 );

    int colorLoc = mShaderStreaks->getAttribSemanticLocation( geom::Attrib::COLOR );
    gl::enableVertexAttribArray( colorLoc );
    gl::vertexAttribPointer( colorLoc, 4, GL_FLOAT, GL_FALSE, sizeof(float)*7, (void*)(sizeof(float)*3) );

    mStarsVbo->unbind();
    mStarsVao->unbind();
}

void StreakingStars::update()
{
    mTimer += 0.02;//001f;
}

void StreakingStars::render()
{
    // Draw a standard grid
    gl::pushMatrices();
    
    // Slowly spins the visualizer
    //gl::rotate(mTimer, 0, 0, 1);
    gl::setMatrices( mCam );
    
    float scale = kScreenDimension;//getWindowHeight();
    
    float transX = 0;//-1;//cos(mTimer*0.05);
    float transY = 0;//1;//sin(mTimer*0.05);
    gl::translate(Vec2f(transX * scale * 0.5, transY * scale * 0.5));
    
    // 0..1 -> 0..5000
    gl::scale(scale,
              scale,
              scale);
    
    gl::rotate(mTimer, 0, 0, 1);

    mShaderStreaks->bind();

    mStarsVao->bind();
    mStarsVbo->bind();
    
    int numVerts = mNumStars * 2;

    gl::setDefaultShaderVars();
    gl::drawArrays(GL_LINES, 0, numVerts);

    mStarsVbo->unbind();
    mStarsVao->unbind();
    mShaderStreaks->unbind();

    gl::popMatrices();
    
}

