//
//  GLView.h
//  SnowFlakes
//
//  Created by Kevin Harris on 4/2/13.
//  Copyright (c) 2013 Kevin Harris. All rights reserved.
//

#import <QuartzCore/QuartzCore.h>
#import "GL11Render.hpp"

// GLView assists us by hosting our OpenGL rendering code, which is written in C++,
// in a custom UIView written in Objective-C. It is our bridge into Cocoa/UIKit.
@interface GLView : UIView
{
@private

    EAGLContext* m_context;
    GL11Renderer* m_renderingEngine;
    IResourceLoader* m_resourceLoader;
    float m_timestamp;
}

- (void) drawView: (CADisplayLink*) displayLink;

@end
