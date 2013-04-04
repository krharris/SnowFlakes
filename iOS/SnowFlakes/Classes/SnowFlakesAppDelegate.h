//
//  SnowFlakesAppDelegate.h
//  SnowFlakes
//
//  Created by Kevin Harris on 4/2/13.
//  Copyright (c) 2013 Kevin Harris. All rights reserved.
//

#include "GLView.h"

@interface SnowFlakesAppDelegate : NSObject <UIApplicationDelegate>
{
@private
    
    UIWindow* m_window;
    GLView* m_view;
}

@end

