

#include "PrecompiledHeader.h"
#include "LTContent.h"
#include "LTAsset.h"

LTModel* Content::Models::m_Cube(nullptr);

uint32_t Content::Models::GetCubeID()
{
    return 0;
}

LTModel* Content::Models::GetCubeNoLoad()
{
    if (!m_Cube)
    {
        LTAssetManager::GetInstance().Get(/* asset id = */ 0, (LTAsset*&)m_Cube);
    }

    return m_Cube;
}

LTModel* Content::Models::GetCube()
{
    if (!m_Cube)
    {
        LTAssetManager::GetInstance().GetLoad(/* asset id = */ 0, (LTAsset*&)m_Cube);
    }

    return m_Cube;
}

LTShader* Content::FragmentShaders::m_Simple(nullptr);

uint32_t Content::FragmentShaders::GetSimpleID()
{
    return 1;
}

LTShader* Content::FragmentShaders::GetSimpleNoLoad()
{
    if (!m_Simple)
    {
        LTAssetManager::GetInstance().Get(/* asset id = */ 1, (LTAsset*&)m_Simple);
    }

    return m_Simple;
}

LTShader* Content::FragmentShaders::GetSimple()
{
    if (!m_Simple)
    {
        LTAssetManager::GetInstance().GetLoad(/* asset id = */ 1, (LTAsset*&)m_Simple);
    }

    return m_Simple;
}

LTShader* Content::VertexShaders::m_Simple(nullptr);

uint32_t Content::VertexShaders::GetSimpleID()
{
    return 2;
}

LTShader* Content::VertexShaders::GetSimpleNoLoad()
{
    if (!m_Simple)
    {
        LTAssetManager::GetInstance().Get(/* asset id = */ 2, (LTAsset*&)m_Simple);
    }

    return m_Simple;
}

LTShader* Content::VertexShaders::GetSimple()
{
    if (!m_Simple)
    {
        LTAssetManager::GetInstance().GetLoad(/* asset id = */ 2, (LTAsset*&)m_Simple);
    }

    return m_Simple;
}
