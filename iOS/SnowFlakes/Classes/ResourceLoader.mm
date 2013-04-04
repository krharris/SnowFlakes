//
//  ResourceLoader.mm
//  SnowFlakes
//
//  Created by Kevin Harris on 4/3/13.
//  Copyright (c) 2013 Kevin Harris. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "IResourceLoader.hpp"

// The ResourceLoader class is used to load .png files from the app's resources.
class ResourceLoader : public IResourceLoader
{
public:
    
    ResourceLoader() :
        m_imageData( 0 )
    {}

    string GetResourcePath() const
    {
        NSString* bundlePath =[[NSBundle mainBundle] resourcePath];
        return [bundlePath UTF8String];
    }

    TextureDescription LoadImage( const string& file )
    {
        NSString* basePath = [NSString stringWithUTF8String:file.c_str()];
        NSString* resourcePath = [[NSBundle mainBundle] resourcePath];
        NSString* fullPath = [resourcePath stringByAppendingPathComponent:basePath];
        UIImage* uiImage = [UIImage imageWithContentsOfFile:fullPath];
                
        TextureDescription description;
        description.Width = CGImageGetWidth( uiImage.CGImage );
        description.Height = CGImageGetHeight( uiImage.CGImage );
        description.BitsPerComponent = 8;
        description.Format = TextureFormatRgba;
        description.MipCount = 1;

        int bpp = description.BitsPerComponent / 2;
        int byteCount = description.Width * description.Height * bpp;
        unsigned char* data = ( unsigned char* ) calloc( byteCount, 1 );
        
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        CGBitmapInfo bitmapInfo = kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big;
        CGContextRef context = CGBitmapContextCreate( data,
                                                      description.Width,
                                                      description.Height,
                                                      description.BitsPerComponent,
                                                      bpp * description.Width,
                                                      colorSpace,
                                                      bitmapInfo );
        CGColorSpaceRelease( colorSpace );
        CGRect rect = CGRectMake( 0, 0, description.Width, description.Height );
        CGContextDrawImage( context, rect, uiImage.CGImage );
        CGContextRelease( context );
        
        m_imageData = [NSData dataWithBytesNoCopy:data length:byteCount freeWhenDone:YES];
        
        return description;
    }
    
    TextureDescription LoadPngImage( const string& file )
    {
        NSString* basePath = [NSString stringWithUTF8String:file.c_str()];
        NSString* resourcePath = [[NSBundle mainBundle] resourcePath];
        NSString* fullPath = [resourcePath stringByAppendingPathComponent:basePath];

        NSLog( @"Loading PNG image %@", fullPath );

        UIImage* uiImage = [UIImage imageWithContentsOfFile:fullPath];
        CGImageRef cgImage = uiImage.CGImage;

        CFDataRef dataRef = CGDataProviderCopyData( CGImageGetDataProvider( cgImage ) );
        m_imageData = [NSData dataWithData:(NSData*) dataRef];
        CFRelease( dataRef );
        
        TextureDescription description;
        description.Width = CGImageGetWidth( cgImage );
        description.Height = CGImageGetHeight( cgImage );
        bool hasAlpha = CGImageGetAlphaInfo( cgImage ) != kCGImageAlphaNone;
        CGColorSpaceRef colorSpace = CGImageGetColorSpace( cgImage );
        
        switch( CGColorSpaceGetModel( colorSpace ) )
        {
            case kCGColorSpaceModelMonochrome:
                description.Format = hasAlpha ? TextureFormatGrayAlpha : TextureFormatGray;
                break;
                
            case kCGColorSpaceModelRGB:
                description.Format = hasAlpha ? TextureFormatRgba : TextureFormatRgb;
                break;
                
            default:
                assert( !"Unsupported color space." );
                break;
        }
        
        description.BitsPerComponent = CGImageGetBitsPerComponent( cgImage );

        return description;
    }
    
    void* GetImageData()
    {
        return (void*)[m_imageData bytes];
    }
    
    void UnloadImage()
    {
        m_imageData = 0;
    }
    
private:
    
    NSData* m_imageData;
};

IResourceLoader* CreateResourceLoader()
{
    return new ResourceLoader();
}
