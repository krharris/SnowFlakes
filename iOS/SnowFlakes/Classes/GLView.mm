//
//  GLView.mm
//  SnowFlakes
//
//  Created by Kevin Harris on 4/2/13.
//  Copyright (c) 2013 Kevin Harris. All rights reserved.
//

#import "GLView.h"
#import "GL11Render.hpp"

@implementation GLView

+ (Class) layerClass
{
    return [CAEAGLLayer class];
}

- (id) initWithFrame: (CGRect) frame
{
    if( self = [super initWithFrame:frame] )
    {
        CAEAGLLayer* eaglLayer = (CAEAGLLayer*) super.layer;
        eaglLayer.opaque = YES;

        EAGLRenderingAPI api = kEAGLRenderingAPIOpenGLES1;
        
        if( api == kEAGLRenderingAPIOpenGLES1 )
        {
            NSLog( @"Using OpenGL ES 1.1" );

            m_context = [[EAGLContext alloc] initWithAPI:api];
        
            if( !m_context )
            {
                api = kEAGLRenderingAPIOpenGLES1;
                m_context = [[EAGLContext alloc] initWithAPI:api];
            }
            
            if( !m_context || ![EAGLContext setCurrentContext:m_context] )
            {
                [self release];
                return nil;
            }

            m_renderingEngine = new GL11Renderer();
        }
        else
        {
            NSLog( @"This sample does not support OpenGL ES 2.0!" );
            return self;
        }

        [m_context renderbufferStorage:GL_RENDERBUFFER fromDrawable: eaglLayer];
        
        m_renderingEngine->Initialize( CGRectGetWidth(frame), CGRectGetHeight(frame) );
        
        [self drawView: nil];
        m_timestamp = CACurrentMediaTime();

        CADisplayLink* displayLink;
        displayLink = [CADisplayLink displayLinkWithTarget:self
                                     selector:@selector(drawView:)];
        
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];

        [[UIDevice currentDevice] beginGeneratingDeviceOrientationNotifications];
    }

    return self;
}

- (void) drawView: (CADisplayLink*) displayLink
{
    if( displayLink != nil )
    {
        float elapsedSeconds = displayLink.timestamp - m_timestamp;
        m_timestamp = displayLink.timestamp;
        m_renderingEngine->Update( elapsedSeconds );
    }

    m_renderingEngine->Render();
    
    [m_context presentRenderbuffer:GL_RENDERBUFFER];
}

@end
