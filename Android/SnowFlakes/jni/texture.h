#ifndef _GUILDHALL_TEXTURE_H_
#define _GUILDHALL_TEXTURE_H_

#include "resource.h"
#include "types.h"

#include <android_native_app_glue.h>
#include <GLES/gl.h>
#include <png.h>

namespace guildhall {

class Texture
{
public:

	Texture( android_app* pApplication, const char* pPath );

	const char* getPath();
	int32_t getHeight();
	int32_t getWidth();

	status load();
	void unload();
	void apply();

protected:

	uint8_t* loadImage();

private:

	static void callback_read( png_structp pStruct, png_bytep pData, png_size_t pSize );

private:

	Resource m_resource;
	GLuint m_textureId;
	int32_t m_width, m_height;
	GLint m_format;
};

}
#endif // _GUILDHALL_TEXTURE_H_
