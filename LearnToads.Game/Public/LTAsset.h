#pragma once

#include "PrecompiledHeader.h"

// types of assets: textures, models, audio, scenes, arbitrary data, etc.
// assets can have references to other assets, this forms a dependency graph
// asset dependency graphs should support interdependency
// asset loading should be asynchronous always, and parallel if possible
// asset memory should be controlled through various appropriate allocators we specify

enum class LTAssetType
{
    LT_ASSET_TYPE_UNKNOWN = 0x0,
    LT_ASSET_TYPE_SHADER = 0x1,
    LT_ASSET_TYPE_TEXTURE = 0x2,
    LT_ASSET_TYPE_MODEL = 0x3,
};

enum class LTAssetState
{
    LT_ASSET_STATE_NOT_LOADED = 0x1,
    LT_ASSET_STATE_LOADED = 0x2,
};

enum class LTAssetJobType
{
    LT_ASSET_JOB_TYPE_LOAD = 0x1,
    LT_ASSET_JOB_TYPE_UNLOAD = 0x2
};

enum class LTAssetJobResult
{
    LT_ASSET_JOB_RESULT_NONE = 0x0,
    LT_ASSET_JOB_RESULT_SUCCESS = 0x1,
    LT_ASSET_JOB_RESULT_FAILURE = 0x2
};

using LTAssetID = uint32_t;

class LTAsset
{
private:
    LTAssetID m_AssetID;

    // The asset state is volatile because it is accessed
    // from both the game and content threads. It is checked
    // by both categories of threads and written by content threads.
    // Being 'volatile' ensures that when the value is used, it 
    // is always fresh and not a local out-of-date copy.
    volatile LTAssetState m_AssetState;
    
    LTAssetType m_AssetType;
    std::string m_FileName;

public:
    LTAsset() :
        m_AssetID(0),
        m_AssetState(LTAssetState::LT_ASSET_STATE_NOT_LOADED),
        m_AssetType(LTAssetType::LT_ASSET_TYPE_UNKNOWN)
    {
    }

    LTAsset(LTAssetID assetID, LTAssetType assetType) :
        m_AssetID(assetID),
        m_AssetState(LTAssetState::LT_ASSET_STATE_NOT_LOADED),
        m_AssetType(assetType)
    {
    }

    LTAsset(LTAssetID assetID, LTAssetType assetType, const std::string& fileName) :
        m_AssetID(assetID),
        m_AssetState(LTAssetState::LT_ASSET_STATE_NOT_LOADED),
        m_AssetType(assetType),
        m_FileName(fileName)
    {
    }

    virtual ~LTAsset()
    {
    }

    // LTAsset(const LTAsset& copy) = delete;
    LTAsset& operator=(const LTAsset& other) = delete;
private:
    inline void SetAssetState(LTAssetState state)
    {
        m_AssetState = state;
    }

public:
    inline const uint32_t GetAssetID() const
    {
        return m_AssetID;
    }

    inline const LTAssetState GetAssetState() const
    {
        return m_AssetState;
    }

    inline const std::string& GetFileName() const
    {
        return m_FileName;
    }

    inline const LTAssetType GetAssetType() const
    {
        return m_AssetType;
    }

    inline const bool IsValid() const
    {
        return m_AssetState == LTAssetState::LT_ASSET_STATE_LOADED;
    }

    friend class LTAssetManager;
};

class LTShader : public LTAsset
{
private:
    VkShaderModule m_ShaderModule;

public:
    LTShader() : 
        m_ShaderModule()
    {
    }

    LTShader(LTAssetID assetID, LTAssetType assetType) :
        LTAsset(assetID, assetType),
        m_ShaderModule()
    {
    }

    LTShader(LTAssetID assetID, LTAssetType assetType, const std::string& fileName) :
        LTAsset(assetID, assetType, fileName),
        m_ShaderModule()
    {
    }

    ~LTShader()
    {
    }

    VkShaderModule& GetShaderModule();
};

class LTModel : public LTAsset
{
public:
    LTModel()
    {
    }

    LTModel(LTAssetID assetID, LTAssetType assetType) :
        LTAsset(assetID, assetType)
    {
    }

    LTModel(LTAssetID assetID, LTAssetType assetType, const std::string& fileName) :
        LTAsset(assetID, assetType, fileName)
    {
    }

    ~LTModel()
    {
    }
};


struct LTAssetJob
{
    LTAsset* m_Asset;
    LTAssetJobType m_JobType;
    LTAssetJobResult m_Result;

    LTAssetJob(LTAsset* asset, LTAssetJobType type) : 
        m_Asset(asset), 
        m_JobType(type), 
        m_Result(LTAssetJobResult::LT_ASSET_JOB_RESULT_NONE)
    {
    }
};

class LTAssetManager
{
private:
    eastl::vector<LTAsset*> m_Assets;
    std::thread* m_ContentThread;
    eastl::queue<LTAssetJob> m_AssetJobs;
    std::mutex m_AssetMutex;
    class LTVKDevice* m_LTVKDevice;

private:
    LTAssetManager() : 
        m_ContentThread(nullptr),
        m_LTVKDevice(nullptr) 
    {
    }

private:
    bool LoadAsset(LTAssetJob& assetJob);
    bool LoadAsset_File(
        const std::string& fileName,
        std::ifstream& file,
        uint8_t*& outBuffer,
        size_t& outSize);
    bool LoadAsset_ByType(
        LTAssetJob& assetJob,
        uint8_t* fileBuffer,
        size_t fileSize);
    bool LoadAsset_Shader(LTAssetJob& assetJob,
        uint8_t* fileBuffer,
        size_t fileSize);

    void InitializeContentLookup();
    void ContentThread();

public:
    static LTAssetManager& GetInstance()
    {
        static LTAssetManager manager;
        return manager;
    }

    void Initialize(class LTVKDevice* ltvkDevice);
    bool Get(LTAssetID assetID, LTAsset*& outAsset);
    bool Load(LTAsset& asset);
    bool GetLoad(LTAssetID assetID, LTAsset*& outAsset);
};
