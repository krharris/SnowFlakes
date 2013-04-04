//
//  IResourceLoader.hpp
//  SnowFlakes
//
//  Created by Kevin Harris on 4/3/13.
//  Copyright (c) 2013 Kevin Harris. All rights reserved.
//

#pragma once

#include <string>
using std::string;

enum TextureFormat
{
    TextureFormatGray,
    TextureFormatGrayAlpha,
    TextureFormatRgb,
    TextureFormatRgba,
    TextureFormatPvrtcRgb2,
    TextureFormatPvrtcRgba2,
    TextureFormatPvrtcRgb4,
    TextureFormatPvrtcRgba4,
    TextureFormat565,
    TextureFormat5551,
};

struct TextureDescription
{
    TextureFormat Format;
    int BitsPerComponent;
    int Width;
    int Height;
    int MipCount;
};

// The ResourceLoader class is written in C++, but it does make use of some
// data types from Foundation.framework when it loads .png files from the app's
// resources. Because of this, it can cause build issues when certain headers
// are imported/included in the wrong order. To work around this, we can use
// an Interface to postpone building of the actual ResourceLoader class until
// certain imports/includes are complete.
struct IResourceLoader
{
    virtual ~IResourceLoader() {}

    virtual string GetResourcePath() const = 0;
    virtual TextureDescription LoadPngImage( const string& filename ) = 0;
    virtual TextureDescription LoadImage( const string& filename ) = 0;
    virtual void* GetImageData() = 0;
    virtual void UnloadImage() = 0;
};

IResourceLoader* CreateResourceLoader();
