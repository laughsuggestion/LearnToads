#include "PrecompiledHeader.h"
#include "LTAsset.h"
#include "LTVKDevice.h"

void LTAssetManager::Initialize(LTVKDevice* ltvkDevice)
{
    m_LTVKDevice = ltvkDevice;

    InitializeContentLookup();

    m_ContentThread = new std::thread(&LTAssetManager::ContentThread, this);
}

void LTAssetManager::ContentThread()
{
    while (true)
    {
        LTAssetJob* next = nullptr;

        {
            std::scoped_lock lock(m_AssetMutex);

            if (m_AssetJobs.size())
            {
                next = &m_AssetJobs.front();
                m_AssetJobs.pop();
            }
        }

        if (!next)
        {
            continue;
        }

        printf("asset: %s, type: %d \n",
            next->m_Asset->GetFileName().c_str(),
            next->m_JobType);

        switch (next->m_JobType)
        {
            case LTAssetJobType::LT_ASSET_JOB_TYPE_LOAD:
            {
                // an asset could have been finished loading right as the 
                // load job was being queued; so we reject those jobs here
                if (next->m_Asset->GetAssetState() == LTAssetState::LT_ASSET_STATE_LOADED)
                {
                    continue;
                }

                LoadAsset(*next);
            }
            break;
        }
    }
}

bool LTAssetManager::LoadAsset_Shader(
    LTAssetJob& assetJob,
    uint8_t* fileBuffer,
    size_t fileSize)
{
    LTShader* shaderAsset = (LTShader*)assetJob.m_Asset;

    // Vulkan shader creation info
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = fileSize;
    createInfo.pCode = reinterpret_cast<const uint32_t*>(fileBuffer);

    VkDevice device = m_LTVKDevice->GetDevice();

    // create the shader module
    if (vkCreateShaderModule(
        device,
        &createInfo,
        nullptr,
        &shaderAsset->GetShaderModule()) != VK_SUCCESS)
    {
        assetJob.m_Result = LTAssetJobResult::LT_ASSET_JOB_RESULT_FAILURE;
        return false;
    }

    assetJob.m_Result = LTAssetJobResult::LT_ASSET_JOB_RESULT_SUCCESS;
    return true;
}

bool LTAssetManager::LoadAsset_ByType(
    LTAssetJob& assetJob,
    uint8_t* fileBuffer,
    size_t fileSize)
{
    switch (assetJob.m_Asset->GetAssetType())
    {
        case LTAssetType::LT_ASSET_TYPE_SHADER:
        {
            return LoadAsset_Shader(assetJob, fileBuffer, fileSize);
        }
    }

    assetJob.m_Result = LTAssetJobResult::LT_ASSET_JOB_RESULT_FAILURE;
    return false;
}

bool LTAssetManager::LoadAsset(LTAssetJob& assetJob)
{
    // load the asset file
    std::ifstream file;
    uint8_t* fileBuffer;
    size_t fileSize;

    if (!LoadAsset_File(
        assetJob.m_Asset->GetFileName(),
        file,
        fileBuffer,
        fileSize))
    {
        assetJob.m_Result = LTAssetJobResult::LT_ASSET_JOB_RESULT_FAILURE;
        return false;
    }

    // load the asset according to its type
    bool success = LoadAsset_ByType(assetJob, fileBuffer, fileSize);

    // deallocate file buffer
    eastl::GetDefaultAllocator()->deallocate(fileBuffer, fileSize);

    // close asset file
    if (file.is_open())
    {
        file.close();
    }

    if (success)
    {
        assetJob.m_Asset->SetAssetState(LTAssetState::LT_ASSET_STATE_LOADED);
    }

    return success;
}

bool LTAssetManager::LoadAsset_File(
    const std::string& fileName,
    std::ifstream& file,
    uint8_t*& outBuffer,
    size_t& outSize)
{
    auto openOptions = std::ios_base::in | std::ios_base::binary | std::ios_base::ate;

    file.open(fileName, openOptions);

    if (!file.is_open())
    {
        return false;
    }

    size_t size = file.tellg();

    if (size <= 0)
    {
        file.close();
        return false;
    }

    outBuffer = (uint8_t*)eastl::GetDefaultAllocator()->allocate(size);

    if (!outBuffer)
    {
        file.close();
        return false;
    }

    file.seekg(0, std::ios::beg);
    file.read((char*)outBuffer, size);

    outSize = size;

    return true;
}

static void FetchCsvCell(
    size_t& start,
    size_t& end,
    const std::string& csvLine,
    const std::string& delimeter,
    std::string& outCellString)
{
    outCellString = csvLine.substr(start, end - start);

    start = end + delimeter.length();
    end = csvLine.find(delimeter, end + delimeter.length());
}

void LTAssetManager::InitializeContentLookup()
{
    // content lookup file
    std::ifstream clf("Build/Content/content.csv");
    std::string csvLine;

    // read each line of the content.csv and create a lookup for
    // content by ID
    if (clf.is_open())
    {
        std::string delimeter = ",";

        while (getline(clf, csvLine))
        {
            size_t start = 0;
            size_t end = csvLine.find(delimeter);

            LTAssetID assetID;
            LTAssetType assetType;
            std::string assetPath;

            LTAsset* asset = (LTAsset*)eastl::GetDefaultAllocator()->allocate(sizeof(LTAsset));

            m_Assets.push_back(asset);

            std::string cellString;

            // asset id
            FetchCsvCell(start, end, csvLine, delimeter, cellString);
            assetID = std::stoi(cellString);

            // asset path
            FetchCsvCell(start, end, csvLine, delimeter, cellString);
            assetPath = cellString;

            // asset type
            FetchCsvCell(start, end, csvLine, delimeter, cellString);
            assetType = (LTAssetType)std::stoi(cellString);

            switch (assetType)
            {
                case LTAssetType::LT_ASSET_TYPE_SHADER:
                {
                    new(asset) LTShader(assetID, assetType, assetPath);
                }
                break;
                case LTAssetType::LT_ASSET_TYPE_TEXTURE:
                {

                }
                break;
                case LTAssetType::LT_ASSET_TYPE_MODEL:
                {

                }
                break;
                case LTAssetType::LT_ASSET_TYPE_UNKNOWN: break;
                default: break;
            }
        }

        clf.close();
    }
}

bool LTAssetManager::Get(LTAssetID assetID, LTAsset*& outAsset)
{
    assert(assetID >= 0 && assetID < m_Assets.size());

    outAsset = m_Assets[assetID];

    return outAsset->GetAssetState() == LTAssetState::LT_ASSET_STATE_LOADED;
}

bool LTAssetManager::Load(LTAsset& asset)
{
    // if the asset has already been loaded, early out
    if (asset.GetAssetState() == LTAssetState::LT_ASSET_STATE_LOADED)
    {
        return true;
    }

    // lock for queuing jobs
    std::scoped_lock lock(m_AssetMutex);

    // queue up load asset job
    m_AssetJobs.push(LTAssetJob(&asset, LTAssetJobType::LT_ASSET_JOB_TYPE_LOAD));

    return true;
}

bool LTAssetManager::GetLoad(LTAssetID assetID, LTAsset*& outAsset)
{
    return Get(assetID, outAsset) || Load(*outAsset);
}

VkShaderModule& LTShader::GetShaderModule()
{
    return m_ShaderModule;
}
