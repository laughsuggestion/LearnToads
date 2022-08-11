

#include "PrecompiledHeader.h"
#include "LTContent.h"
#include "LTAsset.h"


uint32_t Content::Models::GetCubeID()
{
    return 0;
}

LTAssetHandle Content::Models::GetCubeNoLoad()
{
    LTAssetHandle assetHandle;
    LTAssetManager::GetInstance().Get(/* asset id = */ 0, assetHandle);
    return assetHandle;
}

LTAssetHandle Content::Models::GetCube()
{
    LTAssetHandle assetHandle;
    LTAssetManager::GetInstance().GetLoad(/* asset id = */ 0, assetHandle);
    return assetHandle;
}


uint32_t Content::FragmentShaders::GetSimpleID()
{
    return 1;
}

LTAssetHandle Content::FragmentShaders::GetSimpleNoLoad()
{
    LTAssetHandle assetHandle;
    LTAssetManager::GetInstance().Get(/* asset id = */ 1, assetHandle);
    return assetHandle;
}

LTAssetHandle Content::FragmentShaders::GetSimple()
{
    LTAssetHandle assetHandle;
    LTAssetManager::GetInstance().GetLoad(/* asset id = */ 1, assetHandle);
    return assetHandle;
}


uint32_t Content::VertexShaders::GetSimpleID()
{
    return 2;
}

LTAssetHandle Content::VertexShaders::GetSimpleNoLoad()
{
    LTAssetHandle assetHandle;
    LTAssetManager::GetInstance().Get(/* asset id = */ 2, assetHandle);
    return assetHandle;
}

LTAssetHandle Content::VertexShaders::GetSimple()
{
    LTAssetHandle assetHandle;
    LTAssetManager::GetInstance().GetLoad(/* asset id = */ 2, assetHandle);
    return assetHandle;
}
