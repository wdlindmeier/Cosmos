//
//  FlickrObj.h
//  PlanetariumAudio
//
//  Created by William Lindmeier on 10/31/13.
//
//

#pragma once

#include "cinder/Cinder.h"
#include "cinder/gl/gl.h"
#include "cinder/ObjLoader.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/TriMesh.h"
#include "cinder/Rand.h"

class FlickerObj
{
    
public:
    
    FlickerObj(){};
    ~FlickerObj(){}
    
    void load(const std::string & filename);
    void setGlScaleTranslateAndRotate(const ci::Vec3f & scale,
                                      const ci::Vec3f & translate,
                                      const ci::Vec3f & rotate)
    {
        mScale = scale;
        mTranslate = translate;
        mRotate = rotate;
    }
    ci::CameraPersp & getCamera(){ return mCam; }

    void render();
    void update(float *fftData);
    
private:
    
    ci::TriMeshRef          mMesh;
    ci::gl::VboRef          mVbo;
    ci::gl::VboRef          mElementVbo;
    ci::gl::VaoRef          mVao;
    ci::gl::GlslProgRef     mShader;
    
    ci::CameraPersp         mCam;
    float                   mTimer;
    ci::Vec3f               mTranslate;
    ci::Vec3f               mScale;
    ci::Vec3f               mRotate;
    
};