#include "PrecompiledHeader.h"

#include "LTContent.h"
#include "LTGameWindow.h"
#include "LTAsset.h"
#include "LTVKDevice.h"
#include "LTVKPipeline.h"

int main()
{
    printf("sizeof(LTAssetState): %zu,\n", sizeof(LTAssetState));
    printf("sizeof(std::atomic<LTAssetState>): %zu,\n", sizeof(std::atomic<LTAssetState>));

    LTGameWindow gameWindow;
    gameWindow.Initialize();

    LTVKDevice graphicsDevice(gameWindow);

    if (!graphicsDevice.Initialize())
    {
        return 0;
    }

    LTAssetManager& assetManager = LTAssetManager::GetInstance();
    assetManager.Initialize(&graphicsDevice);

    LTAssetHandle simpleVertShaderAsset = Content::VertexShaders::GetSimple();
    LTAssetHandle simpleFragShaderAsset = Content::FragmentShaders::GetSimple();

    LTVKPipeline pipeline(
        &graphicsDevice,
        (LTShader*)simpleVertShaderAsset.GetAsset(),
        (LTShader*)simpleFragShaderAsset.GetAsset());

    LTVKPipelineConfig config;
    pipeline.GetDefaultPipelineConfig(
        config,
        gameWindow.GetWidth(),
        gameWindow.GetHeight());

    pipeline.Initialize(config);

    //if (GetMouseDown(left))
    //{
    //    LTAsset* cubeAsset = Content::Models::GetCubePtr();
    //    FireCube(cubeAsset); // <-- create the cube and give it velocity
    //    // p = Create Projectile
    //    // p.SetMesh(cubeAsset); <-- renderer can use cube when its ready
    //    // 0. have already loaded the smallest version of cube
    //    // 1. render nothing
    //    // 2. sync wait for *something*  (small scale)
    //    // 3. async wait for full scale
    //}

    while (!gameWindow.ShouldClose())
    {
        gameWindow.Update();
    }

    graphicsDevice.Destroy();
    gameWindow.Destroy();
}