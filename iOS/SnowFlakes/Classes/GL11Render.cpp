//
//  GL11Renderer.cpp
//  SnowFlakes
//
//  Created by Kevin Harris on 4/3/13.
//  Copyright (c) 2013 Kevin Harris. All rights reserved.
//

#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include "GL11Render.hpp"
#include "IResourceLoader.hpp"

GL11Renderer::GL11Renderer() :
    m_framebuffer( 0 ),
    m_colorRenderbuffer( 0 ),
    m_depthRenderbuffer( 0 ),
    m_snowTextureId( 0 ),
    m_vertexBufferId( 0 ),
    m_colorBufferId( 0 ),
    m_pointSizeBufferId( 0 ),
    m_resourceLoader(  CreateResourceLoader() )
{
    // Create and bind the color buffer so that the caller can allocate its space.
    glGenRenderbuffersOES( 1, &m_colorRenderbuffer );
    glBindRenderbufferOES( GL_RENDERBUFFER_OES, m_colorRenderbuffer );
}

GL11Renderer::~GL11Renderer()
{
    if( m_snowTextureId )
	{
		glDeleteTextures( 1, &m_snowTextureId );
		m_snowTextureId = 0;
	}

	if( m_vertexBufferId )
	{
		glDeleteBuffers( 1, &m_vertexBufferId );
		m_vertexBufferId = 0;
	}
    
    if( m_colorBufferId )
	{
		glDeleteBuffers( 1, &m_colorBufferId );
		m_colorBufferId = 0;
	}
    
    if( m_pointSizeBufferId )
	{
		glDeleteBuffers( 1, &m_pointSizeBufferId );
		m_pointSizeBufferId = 0;
	}
	
    if( m_framebuffer )
	{
		glDeleteFramebuffersOES( 1, &m_framebuffer );
		m_framebuffer = 0;
	}
    
    if( m_depthRenderbuffer )
    {
        glDeleteFramebuffersOES( 1, &m_depthRenderbuffer );
        m_depthRenderbuffer = 0;
    }
    
	if( m_colorRenderbuffer )
	{
		glDeleteRenderbuffersOES( 1, &m_colorRenderbuffer );
		m_colorRenderbuffer = 0;
	}
}

void GL11Renderer::Initialize( int width, int height )
{
    //
    // Buffer Setup...
    //
    
    // Create the depth buffer.
    glGenRenderbuffersOES( 1, &m_depthRenderbuffer );
    glBindRenderbufferOES( GL_RENDERBUFFER_OES, m_depthRenderbuffer );
    glRenderbufferStorageOES( GL_RENDERBUFFER_OES,
                              GL_DEPTH_COMPONENT16_OES,
                              width,
                              height );
    
    // Create the framebuffer object; attach the depth and color buffers.
    glGenFramebuffersOES( 1, &m_framebuffer );
    glBindFramebufferOES( GL_FRAMEBUFFER_OES, m_framebuffer );
    glFramebufferRenderbufferOES( GL_FRAMEBUFFER_OES,
                                  GL_COLOR_ATTACHMENT0_OES,
                                  GL_RENDERBUFFER_OES,
                                  m_colorRenderbuffer );
    glFramebufferRenderbufferOES( GL_FRAMEBUFFER_OES,
                                  GL_DEPTH_ATTACHMENT_OES,
                                  GL_RENDERBUFFER_OES,
                                  m_depthRenderbuffer );
    
    // Bind the color buffer for rendering.
    glBindRenderbufferOES( GL_RENDERBUFFER_OES, m_colorRenderbuffer );

    //
    // General Setup...
    //
    
    glEnable( GL_TEXTURE_2D );
    glDisable( GL_LIGHTING );
    
    //
    // Point Sprites...
    //
    
    glEnable( GL_POINT_SPRITE_OES );
    glTexEnvi( GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_TRUE );
    glTexEnvi( GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_FALSE );
    //glPointSize( 10.0f );
    
    // This helps as a work around for order-dependency artifacts that can occur when sprites overlap.
    glBlendFunc( GL_SRC_ALPHA, GL_ONE );
    
    //
    // Texture Setup...
    //

    glGenTextures( 1, &m_snowTextureId );
    glBindTexture( GL_TEXTURE_2D, m_snowTextureId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    TextureDescription desc = m_resourceLoader->LoadPngImage( "snow.png" );
    GLvoid* pixels = m_resourceLoader->GetImageData();
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, desc.Width, desc.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
    m_resourceLoader->UnloadImage();

    //
    // VBO Setup for Snow Flakes...
    //
    
    for( int i = 0; i < MaxSnowFlakes; ++i )
    {
		m_pos[i][0] = RandomFloat( -ViewMaxX, ViewMaxX );
        m_pos[i][1] = RandomFloat( -ViewMaxY, ViewMaxY );
        
		m_vel[i][0] = RandomFloat( -0.004f, 0.004f ); // Flakes move side to side
		m_vel[i][1] = RandomFloat( -0.01f, -0.008f ); // Flakes fall down
        
		m_col[i][0] = 1.0f;
		m_col[i][1] = 1.0f;
		m_col[i][2] = 1.0f;
		m_col[i][3] = 1.0f; //RandomFloat( 0.6f, 1.0f ); // It seems that Doodle Jump snow does not use alpha.
        
        m_size[i] = RandomFloat( 3.0, 6.0f );
        
        // It looks strange if the flakes all turn at the same time, so
        // lets vary their turn times with a random negative value.
        m_timeSinceLastTurn[i] = RandomFloat( -5.0, 0.0f );
	}

    // VBO for vertex positions.
    glGenBuffers( 1, &m_vertexBufferId );
    glBindBuffer( GL_ARRAY_BUFFER, m_vertexBufferId );
    glBufferData( GL_ARRAY_BUFFER, sizeof(m_pos), m_pos, GL_DYNAMIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    
// TODO: Due to the possibility of cache misses, it might be a little faster to
//       combine the colors and point sizes into an interleaved array, but it
//       seems that this optimization makes less sense on newer CPUs.

    // VBO for vertex colors.
    glGenBuffers( 1, &m_colorBufferId );
    glBindBuffer( GL_ARRAY_BUFFER, m_colorBufferId );
    glBufferData( GL_ARRAY_BUFFER, sizeof(m_col), m_col, GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    
    // VBO for point sizes of point sprites.
    glGenBuffers( 1, &m_pointSizeBufferId );
    glBindBuffer( GL_ARRAY_BUFFER, m_pointSizeBufferId );
    glBufferData( GL_ARRAY_BUFFER, sizeof(m_size), m_size, GL_STATIC_DRAW );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
 
    //
    // View Settings...
    //

    glViewport( 0, 0, width, height );

    // Initialize the projection matrix to orthogrpahic and create 
    // a flat 2D game space that is 2 units wide and 3 units high.
    glMatrixMode( GL_PROJECTION );
    glOrthof( -ViewMaxX, +ViewMaxX, -ViewMaxY, +ViewMaxY, -1, 1 );
}


void GL11Renderer::Update( float timeStep )
{
	for( int i = 0; i < MaxSnowFlakes; ++i )
    {
        // Keep track of how long it has been since this flake turned
        // or changed direction.
        m_timeSinceLastTurn[i] += timeStep;

        if( m_timeSinceLastTurn[i] >= TimeTillTurn )
        {
            // Change or invert direction!
            m_vel[i][0] = -(m_vel[i][0]);
            m_timeSinceLastTurn[i] = 0.0f;
        }
        
        // Speed up the flake up as it leaves the last turn and prepares for next turn.
        float turnVelocityModifier = m_timeSinceLastTurn[i] * TimeTillTurnNormalizedUnit;
        
        // Apply some velocity to simulate gravity and wind.
		m_pos[i][0] += (m_vel[i][0] * turnVelocityModifier); // Side to side
		m_pos[i][1] += m_vel[i][1]; // Gravity
        
        // But, if the snow flake goes off the bottom or strays too far
        // left or right - respawn it back to the top.
        if( m_pos[i][1] < -(ViewMaxY + 0.2f) ||
            m_pos[i][0] < -(ViewMaxX + 0.2f) || m_pos[i][0] > (ViewMaxX + 0.2f) )
        {
            m_pos[i][0] = RandomFloat( -ViewMaxX, ViewMaxX );
            m_pos[i][1] = 3.1;
        }
	}
}

void GL11Renderer::Render() const
{
    // Doodle jump sky color (or something like it).
    glClearColor( 0.31f, 0.43f, 0.63f, 1.0f );
    
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glPushMatrix();

    //
    // Render our snow flakes as a VBO of point sprites...
    //

    // We don't care about depth for point sprites.
    glDepthMask( GL_FALSE ); // Turn off depth writes

    glEnable( GL_BLEND );
    glEnable( GL_POINT_SPRITE_OES );
    glTexEnvi( GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_TRUE );
    
    glEnableClientState( GL_VERTEX_ARRAY );
    glEnableClientState( GL_COLOR_ARRAY );
    glEnableClientState( GL_POINT_SIZE_ARRAY_OES );
    
    glBindBuffer( GL_ARRAY_BUFFER, m_vertexBufferId );
    glBufferSubData( GL_ARRAY_BUFFER, 0, MaxSnowFlakes * 2 * sizeof(float), m_pos );
    glVertexPointer( 2, GL_FLOAT, 0, 0 );

    glBindBuffer( GL_ARRAY_BUFFER, m_colorBufferId );
    glBufferSubData( GL_ARRAY_BUFFER, 0, MaxSnowFlakes * 4 * sizeof(float), m_col );
    glColorPointer( 4, GL_FLOAT, 0, 0 );

    glBindBuffer( GL_ARRAY_BUFFER, m_pointSizeBufferId );
    glBufferSubData( GL_ARRAY_BUFFER, 0, MaxSnowFlakes * sizeof(float), m_size );
    glPointSizePointerOES( GL_FLOAT, sizeof(GL_FLOAT), (GLvoid*)(sizeof(GL_FLOAT)));

    glDrawArrays( GL_POINTS, 0, MaxSnowFlakes );

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glDisableClientState( GL_VERTEX_ARRAY );
    glDisableClientState( GL_COLOR_ARRAY );
    glDisableClientState( GL_POINT_SIZE_ARRAY_OES );

    // Restore the OpenGL state:
    glDisable( GL_BLEND );
    glDisable( GL_POINT_SPRITE_OES );
    glTexEnvi( GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_FALSE );
    
    glDepthMask( GL_TRUE ); // Turn back on depth writes

    glPopMatrix();
}
