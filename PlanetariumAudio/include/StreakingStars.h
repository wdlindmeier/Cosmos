//
//  StreakingStars.h
//  PlanetariumAudio
//
//  Created by William Lindmeier on 10/28/13.
//
//

#pragma once

#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/Vbo.h"
#include "cinder/Camera.h"

class StreakingStars
{
    
public:
    
    StreakingStars() :
    mTimer(0), mNumStars(0){};
    virtual ~StreakingStars(){};
    
    void load();
    virtual void render();
    void update();
    
private:
    
    void                    loadMesh();
    
    ci::gl::GlslProgRef     mShaderStreaks;

    ci::gl::VaoRef          mStarsVao;
    ci::gl::VboRef          mStarsVbo;
    
    ci::CameraPersp         mCam;
    float                   mTimer;
    
    int                     mNumStars;
    
};