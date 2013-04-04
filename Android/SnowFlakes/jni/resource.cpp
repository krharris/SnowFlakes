#include "resource.h"
#include "log.h"

namespace guildhall {

Resource::Resource( android_app* pApplication, const char* pPath ) :
		m_path( pPath ),
		m_assetManager( pApplication->activity->assetManager ),
		m_asset( NULL )
{
}

status Resource::open()
{
	m_asset = AAssetManager_open( m_assetManager, m_path, AASSET_MODE_UNKNOWN );
	return (m_asset != NULL) ? STATUS_OK : STATUS_ERROR;
}

void Resource::close()
{
	if( m_asset != NULL )
	{
		AAsset_close( m_asset );
		m_asset = NULL;
	}
}

status Resource::read( void* pBuffer, size_t pCount )
{
	int32_t lReadCount = AAsset_read( m_asset, pBuffer, pCount );
	return (lReadCount == pCount) ? STATUS_OK : STATUS_ERROR;
}

const char* Resource::getPath()
{
	return m_path;
}

}
