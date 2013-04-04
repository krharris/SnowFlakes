//
//  GL11Renderer.hpp
//  SnowFlakes
//
//  Created by Kevin Harris on 4/3/13.
//  Copyright (c) 2013 Kevin Harris. All rights reserved.
//

#pragma once

#include <vector>

using namespace std;

// The Game's view size or area is 2 units wide and 3 units high.
const float ViewMaxX = 2;
const float ViewMaxY = 3;

const int MaxSnowFlakes = 200;

// Each snow flake will wait 3 seconds - then turn or change direction.
const float TimeTillTurn = 3.0f;
const float TimeTillTurnNormalizedUnit = 1.0f / TimeTillTurn;

class IResourceLoader;

// The GL11Renderer class is home to our C++/OpenGL ES 1.1 code.
// This is where the magic happens!
class GL11Renderer
{
public:
    
    GL11Renderer();
    ~GL11Renderer();
    
    void Initialize( int width, int height );
    void Render() const;
    void Update( float timeStep );
 
private:

    GLuint m_framebuffer;
    GLuint m_colorRenderbuffer;
    GLuint m_depthRenderbuffer;
    
    GLuint m_snowTextureId;

    // Snow flake data.
    GLuint m_vertexBufferId;
    GLuint m_colorBufferId;
    GLuint m_pointSizeBufferId;
    
    float m_pos[MaxSnowFlakes][2];
    float m_vel[MaxSnowFlakes][2];
    float m_col[MaxSnowFlakes][4];
    float m_size[MaxSnowFlakes];
    float m_timeSinceLastTurn[MaxSnowFlakes];

    IResourceLoader* m_resourceLoader;
};

inline float RandomFloat( float min, float max )
{
	float r = (float)rand() / (float)RAND_MAX;
	return min + r * (max - min);
}
