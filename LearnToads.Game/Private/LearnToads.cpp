#include "PrecompiledHeader.h"

#include "LTContent.h"
#include "LTGameWindow.h"
#include "LTAsset.h"
#include "LTVKDevice.h"

int main()
{
    LTGameWindow gameWindow;
    gameWindow.Initialize();

    LTVKDevice graphicsDevice(gameWindow);
    
    if (!graphicsDevice.Initialize())
    {
        return 0;
    }

    LTAssetManager& assetManager = LTAssetManager::GetInstance();
    assetManager.Initialize(&graphicsDevice);

    LTShader* simpleVertShaderAsset = Content::VertexShaders::GetSimple();
    LTShader* simpleFragShaderAsset = Content::FragmentShaders::GetSimple();

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