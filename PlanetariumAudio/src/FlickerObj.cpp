//
//  FlickrObj.cpp
//  PlanetariumAudio
//
//  Created by William Lindmeier on 10/31/13.
//
//

#include "FlickerObj.h"


using namespace ci;
using namespace ci::app;
using namespace std;

void FlickerObj::load(const std::string & filename)
{
    gl::GlslProg::Format mFormat;
    // NOTE: These must be resorces, not assets
    mFormat.vertex( loadResource( "basic.vert" ) )
    .fragment( loadResource( "basic.frag" ) );
    mShader = gl::GlslProg::create( mFormat );
    mShader->bind();

    DataSourceRef file = loadResource(filename); // "tank.obj" //"uav.obj" // "atat.obj"
    ObjLoader loader( file );
    mMesh = TriMesh::create( loader );
    
    mVao = gl::Vao::create();
    mVao->bind();
    
    mVbo = gl::Vbo::create(GL_ARRAY_BUFFER, 3 * mMesh->getNumVertices() * sizeof(float),
                               mMesh->getVertices<3>(), GL_STATIC_DRAW );
    mVbo->bind();
    
    GLint pos = mShader->getAttribLocation( "vPosition" );
    gl::enableVertexAttribArray( pos );
    gl::vertexAttribPointer( pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
    mElementVbo = gl::Vbo::create(GL_ELEMENT_ARRAY_BUFFER,
                                      mMesh->getNumIndices() * 4,
                                      mMesh->getIndices().data());
    mElementVbo->bind();
    mElementVbo->unbind();
    
    mVbo->unbind();
    mVao->unbind();
    mShader->unbind();
    
    mCam.setPerspective(10.0f, // smaller is less perspective
                        getWindowAspectRatio(),
                        .01, 10000 );
    
    mCam.lookAt( Vec3f( 0, 0, -1000 ), Vec3f( 0, 0, 0 ) );

    mTimer = 0;
}

void FlickerObj::update(float *fftData)
{
    mTimer += 0.02;
}

void FlickerObj::render()
{
	// clear out the window with black
	// gl::clear( Color( 0, 0, 0 ) );
    
    gl::enableAlphaBlending();
    // gl::enableAdditiveBlending();
    
    gl::pushMatrices();
    
    gl::setMatrices( mCam );
    
    gl::rotate(mTimer, 0, 0, 1);
    
    gl::pushMatrices();
    
    gl::scale(mScale);
    
    // NOTE: Specific to ATAT
    //gl::rotate(-90, 0, 1, 0);
    gl::rotate(mRotate.x, 1, 0, 0);
    gl::rotate(mRotate.y, 0, 1, 0);
    gl::rotate(mRotate.z, 0, 0, 1);
    
    // NOTE: Specific to ATAT
    //gl::translate(Vec3f(40, 40, 0));
    gl::translate(mTranslate);
    
    mShader->bind();
    
    gl::setDefaultShaderVars();
    
    mVao->bind();
    mElementVbo->bind();
    
    int numIndices = mMesh->getNumIndices();
    /*
    mShader->uniform("uColor", ColorAf(0,1,1,0.05f));
    gl::drawElements(GL_LINES,
                     numIndices,
                     GL_UNSIGNED_INT,
                     0);
    */
    
    mShader->uniform("uColor", ColorAf(1,1,1,0.1f));
    
    // TODO: Use another progress number?
    float scalarCompleteness = sin(getElapsedFrames() * 0.005);
    
    // Simple way to push the transition to the edges
    if (scalarCompleteness < 0.25) scalarCompleteness = scalarCompleteness / 0.25;
    else scalarCompleteness = 1.0f;
    
    int frameCount = scalarCompleteness * numIndices;
    frameCount = frameCount - (frameCount % 2);
    int count = std::min<int>(frameCount, numIndices);
    int offset = Rand::randInt(numIndices - count);
    offset = offset - (offset % 2);
    gl::drawElements(GL_LINES,
                     count,
                     GL_UNSIGNED_INT,
                     (void*)(offset * sizeof(GLuint)));
    
    gl::popMatrices();
    
    mElementVbo->unbind();
    mVao->unbind();
    
    mShader->unbind();
    gl::popMatrices();
    
    gl::disableAlphaBlending();
}

