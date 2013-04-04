//
//  main.m
//  SnowFlakes
//
//  Created by Kevin Harris on 4/2/13.
//  Copyright (c) 2013 Kevin Harris. All rights reserved.
//

int main(int argc, char *argv[])
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    int retVal = UIApplicationMain( argc, argv, nil, @"SnowFlakesAppDelegate" );
    
    [pool release];
    
    return retVal;
}
