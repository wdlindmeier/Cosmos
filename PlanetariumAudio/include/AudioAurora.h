//
//  AudioAurora.h
//  PlanetariumAudio
//
//  Created by William Lindmeier on 10/28/13.
//
//

#pragma once

#include "cinder/Cinder.h"
#include "cinder/Surface.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/Vbo.h"
#include "cinder/Camera.h"
#include "cinder/qtime/QuickTimeGl.h"

class AudioAurora
{
    
public:
    
    AudioAurora(){};
    AudioAurora(const int numFFTChannels);
    virtual ~AudioAurora(){}
    
    void update(const float *fftData);
    void render();
    ci::gl::TextureRef & getTexture();
    
private:
    
    void                    loadMesh();
    
    ci::Surface             mFFTSurface;
    ci::gl::TextureRef      mFFTTexture;
    
    ci::gl::GlslProgRef     mShaderAurora;
    int                     mNumFFTChannels;

    ci::gl::VaoRef          mMeshVao;
    ci::gl::VboRef          mMeshVbo;
    
    ci::CameraPersp         mCam;
    float                   mTimer;

};