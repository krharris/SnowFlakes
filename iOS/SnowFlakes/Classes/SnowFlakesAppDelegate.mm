//
//  SnowFlakesAppDelegate.mm
//  SnowFlakes
//
//  Created by Kevin Harris on 4/2/13.
//  Copyright (c) 2013 Kevin Harris. All rights reserved.
//

#import "SnowFlakesAppDelegate.h"

@implementation SnowFlakesAppDelegate

- (void) applicationDidFinishLaunching: (UIApplication*) application
{
    CGRect screenBounds = [[UIScreen mainScreen] bounds];
    
    m_window = [[UIWindow alloc] initWithFrame: screenBounds];
    m_view = [[GLView alloc] initWithFrame: screenBounds];
    
    [m_window addSubview: m_view];
    [m_window makeKeyAndVisible];
}

- (void) dealloc
{
    [m_view release];
    [m_window release];
    [super dealloc];
}

@end
