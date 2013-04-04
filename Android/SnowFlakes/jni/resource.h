#ifndef _GUILDHALL_RESOURCE_H_
#define _GUILDHALL_RESOURCE_H_

#include "types.h"

#include <android_native_app_glue.h>

namespace guildhall {

class Resource
{
public:

	Resource( android_app* pApplication, const char* pPath );

	status open();
	void close();
	status read( void* pBuffer, size_t pCount );

	const char* getPath();

private:

	const char* m_path;
	AAssetManager* m_assetManager;
	AAsset* m_asset;
};

}
#endif //_GUILDHALL_RESOURCE_H_
