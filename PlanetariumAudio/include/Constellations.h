//
//  Constellations.h
//  PlanetariumAudio
//
//  Created by William Lindmeier on 10/29/13.
//
//

#pragma once

#include "cinder/Cinder.h"
#include "cinder/Surface.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/Vbo.h"
#include "cinder/Camera.h"

class Constellations
{
    
public:
    
    Constellations();
    virtual ~Constellations(){};
    
    void render();
    void update(float *fftData);
    void load();

private:
    
    void loadAsterisms();
    void addConstellation(const float *linePoints, const int numPoints);
    
    std::vector<int>            mNumVerts;
    std::vector<float>          mAlphas;
    std::vector<ci::gl::VaoRef> mVaos;
    std::vector<ci::gl::VboRef> mVbos;
    ci::gl::GlslProgRef         mShaderConstellations;
    
    ci::CameraPersp             mCam;
    int                         mNumConstellations;
    float                       mTimer;

};