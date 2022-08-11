#pragma once

#include "PrecompiledHeader.h"

/**
 * Specifies the kind of asset.
 */
enum class LTAssetType
{
    LT_ASSET_TYPE_UNKNOWN = 0x0,
    LT_ASSET_TYPE_SHADER = 0x1,
    LT_ASSET_TYPE_TEXTURE = 0x2,
    LT_ASSET_TYPE_MODEL = 0x3,
};

/**
 * Specifies the current state of the asset in-memory.
 */
enum class LTAssetState
{
    LT_ASSET_STATE_NOT_LOADED = 0x1,
    LT_ASSET_STATE_LOADED = 0x2,
};

/**
 * Specifies the kinds of job types for dealing with assets.
 */
enum class LTAssetJobType
{
    LT_ASSET_JOB_TYPE_LOAD = 0x1,
    LT_ASSET_JOB_TYPE_UNLOAD = 0x2
};

/**
 * Specifies the kinds of success/failure results that occur for jobs.
 */
enum class LTAssetJobResult
{
    LT_ASSET_JOB_RESULT_NONE = 0x0,
    LT_ASSET_JOB_RESULT_SUCCESS = 0x1,
    LT_ASSET_JOB_RESULT_FAILURE = 0x2
};

/**
 * The ID used to uniquely identify assets.
 */
using LTAssetID = uint32_t;

/**
 * LTAsset is meant to be inherited (e.g. LTTexture, LTModel, LTShader, LTAudio, etc.)
 * The base class for all assets in the game.
 */
class LTAsset
{
    /**
     * Fields
     */
private:
    /**
     * Unique identifier for the asset.
     */
    LTAssetID m_AssetID;

    /**
     * The current state of the asset.
     */
    std::atomic<LTAssetState> m_AssetState;

    /**
     * The type of asset.
     */
    LTAssetType m_AssetType;

    /**
     * The location on disk where the asset was loaded.
     */
    std::string m_FileName;

    /**
     * The reference count to this asset.
     * Because assets are reference counted, they should be accessed
     * via the LTAssetHandle
     */
    std::atomic<uint32_t> m_RefCount;

    /**
     * The intrusive next pointer for the LRU
     */
    LTAsset* m_LRUNext;

    /**
     * The intrusive prev pointer for the LRU
     */
    LTAsset* m_LRUPrev;

    /**
     * Constructors
     */
public:
    LTAsset() :
        m_AssetID(0),
        m_AssetState(LTAssetState::LT_ASSET_STATE_NOT_LOADED),
        m_AssetType(LTAssetType::LT_ASSET_TYPE_UNKNOWN),
        m_FileName(""),
        m_RefCount(0),
        m_LRUNext(nullptr),
        m_LRUPrev(nullptr) {}

    LTAsset(LTAssetID assetID, LTAssetType assetType) :
        m_AssetID(assetID),
        m_AssetState(LTAssetState::LT_ASSET_STATE_NOT_LOADED),
        m_AssetType(assetType),
        m_FileName(""),
        m_RefCount(0),
        m_LRUNext(nullptr),
        m_LRUPrev(nullptr) {}

    LTAsset(LTAssetID assetID, LTAssetType assetType, const std::string& fileName) :
        m_AssetID(assetID),
        m_AssetState(LTAssetState::LT_ASSET_STATE_NOT_LOADED),
        m_AssetType(assetType),
        m_FileName(fileName),
        m_RefCount(0),
        m_LRUNext(nullptr),
        m_LRUPrev(nullptr) {}

    virtual ~LTAsset()
    {
    }

    /**
     * Deleted Methods
     */
     // LTAsset(const LTAsset& copy) = delete;
    LTAsset& operator=(const LTAsset& other) = delete;

    /**
     * Private Methods
     */
private:
    /**
     * Sets the current state of the asset.
     */
    inline void SetAssetState(LTAssetState state)
    {
        m_AssetState = state;
    }

    /**
     * Public Methods
     */
public:
    /**
     * Gets the asset ID.
     */
    inline const uint32_t GetAssetID() const
    {
        return m_AssetID;
    }

    /**
     * Gets the asset state.
     */
    inline const LTAssetState GetAssetState() const
    {
        return m_AssetState;
    }

    /**
     * Gets the file name.
     */
    inline const std::string& GetFileName() const
    {
        return m_FileName;
    }

    /**
     * Gets the asset type.
     */
    inline const LTAssetType GetAssetType() const
    {
        return m_AssetType;
    }

    /**
     * Determines if asset is valid (loaded).
     */
    inline const bool IsValid() const
    {
        return m_AssetState == LTAssetState::LT_ASSET_STATE_LOADED;
    }

    /**
     * Atomically gets the ref count.
     */
    inline const uint32_t GetRefCount() const
    {
        return m_RefCount.load(std::memory_order::memory_order_acquire);
    }

    /**
     * Asset manager needs access to private functions on this class.
     */
    friend class LTAssetManager;

    /**
     * The LTAssetHandle needs direct access to private reference count data on this class.
     */
    friend class LTAssetHandle;
};

/**
 * The LTAssetHandle is a *mostly* thread-safe ref counting wrapper around LTAsset.
 * - For caching/storage, this should be used instead of LTAsset*
 * - Allocation should never be done using placement-new since constructors
 *   under placement-new are not thread-safe; the exception is
 *   if allocation itself is being done in a thread-safe manner.
 *
 * Thread Safety:
 * Construction and destruction (and therefore passing into functions) is thread-safe.
 * Copy and move assignment operators are NOT thread-safe. It is safe to do local assignments of
 * asset handles, but passing a pointer to an asset handle across threads and assigning into that pointer
 * is undefined.
 *
 * For the above reason, it's *probably* best to not use pointers
 * to LTAssetHandle -- especially across threads.
 */
class LTAssetHandle
{
    /**
     * Fields
     */
private:
    /**
     * A pointer to the asset being wrapped by the handle. The LTAsset class
     * always exists for every asset (even if that asset is not loaded yet),
     * so the reference count will never be deleted.
     */
    LTAsset* m_Asset;

    /**
     * Constructors
     */
public:
    LTAssetHandle() : m_Asset(nullptr) {}

    LTAssetHandle(LTAsset* asset)
    {
        if (asset)
        {
            // atomic increment the ref count
            asset->m_RefCount.fetch_add(1, std::memory_order::memory_order_release);
        }

        m_Asset = asset;
    }

    LTAssetHandle(const LTAssetHandle& other)
        : m_Asset(other.m_Asset)
    {
        if (m_Asset)
        {
            // atomic increment the reference count
            m_Asset->m_RefCount.fetch_add(1, std::memory_order::memory_order_release);
        }
    }

    LTAssetHandle(LTAssetHandle&& other) noexcept
        : m_Asset(other.m_Asset)
    {
        other.m_Asset = nullptr;
    }

    LTAssetHandle& operator=(const LTAssetHandle& other)
    {
        if (m_Asset == other.m_Asset)
        {
            return *this;
        }

        if (m_Asset)
        {
            // atomic decrement the reference count
            m_Asset->m_RefCount.fetch_sub(1, std::memory_order::memory_order_release);
        }

        m_Asset = other.m_Asset;

        if (m_Asset)
        {
            // atomic increment the ref count
            m_Asset->m_RefCount.fetch_add(1, std::memory_order::memory_order_release);
        }

        return *this;
    }

    LTAssetHandle& operator=(LTAssetHandle&& other) noexcept
    {
        if (m_Asset)
        {
            // atomic decrement the reference count
            m_Asset->m_RefCount.fetch_sub(1, std::memory_order::memory_order_release);
        }

        m_Asset = other.m_Asset;
        other.m_Asset = nullptr;

        return *this;
    }

    ~LTAssetHandle()
    {
        if (m_Asset)
        {
            // atomic decrement the reference count
            m_Asset->m_RefCount.fetch_sub(1, std::memory_order::memory_order_release);
        }
    }

    /**
     * Methods
     */
public:
    /**
     * Get the asset wrapped by this handle.
     */
    inline LTAsset* GetAsset() const
    {
        return m_Asset;
    }
};

/**
 * Asset type for shaders (spv/glsl)
 */
class LTShader : public LTAsset
{
    /**
     * Fields
     */
private:

    /**
     * Vulkan internal shader module.
     */
    VkShaderModule m_ShaderModule;

    /**
     * Constructors
     */
public:
    LTShader() :
        m_ShaderModule()
    {
    }

    LTShader(LTAssetID assetID, LTAssetType assetType) :
        LTAsset(assetID, LTAssetType::LT_ASSET_TYPE_SHADER),
        m_ShaderModule()
    {
    }

    LTShader(LTAssetID assetID, const std::string& fileName) :
        LTAsset(assetID, LTAssetType::LT_ASSET_TYPE_SHADER, fileName),
        m_ShaderModule()
    {
    }

    ~LTShader()
    {
    }

    /**
     * Methods
     */
public:

    /**
     * Gets the Vulkan shader module.
     */
    VkShaderModule& GetShaderModule();
};

/**
 * Asset type for models (fbx, obj).
 */
class LTModel : public LTAsset
{
    /**
     * Fields
     */
private:

    /**
     * Constructors
     */
public:
    LTModel()
    {
    }

    LTModel(LTAssetID assetID) :
        LTAsset(assetID, LTAssetType::LT_ASSET_TYPE_MODEL)
    {
    }

    LTModel(LTAssetID assetID, const std::string& fileName) :
        LTAsset(assetID, LTAssetType::LT_ASSET_TYPE_MODEL, fileName)
    {
    }

    ~LTModel()
    {
    }

    /**
     * Methods
     */
private:
public:
};

/**
 * Asset type for textures (png, tga, bmp, etc.).
 */
class LTTexture : public LTAsset
{
    /**
     * Fields
     */
private:

    /**
     * Constructors
     */
public:
    LTTexture()
    {
    }

    LTTexture(LTAssetID assetID) :
        LTAsset(assetID, LTAssetType::LT_ASSET_TYPE_MODEL)
    {
    }

    LTTexture(LTAssetID assetID, const std::string& fileName) :
        LTAsset(assetID, LTAssetType::LT_ASSET_TYPE_MODEL, fileName)
    {
    }

    ~LTTexture()
    {
    }

    /**
     * Methods
     */
private:
public:
};

/**
 * Data for an asset job.
 */
struct LTAssetJob
{
    /**
     * Fields
     */

     /**
      * The asset that this job is operating on.
      */
    LTAssetHandle assetHandle;

    /**
     * The type of job being performed.
     */
    LTAssetJobType jobType;

    /**
     * The result of the job when it has terminated.
     */
    LTAssetJobResult result;


    /**
     * Constructors
     */
    LTAssetJob(LTAssetHandle assetHandle, LTAssetJobType type) :
        assetHandle(assetHandle),
        jobType(type),
        result(LTAssetJobResult::LT_ASSET_JOB_RESULT_NONE)
    {
    }
};

/**
 * The manager of all assets in the game. Responsible for loading and unloading assets.
 */
class LTAssetManager
{
    /**
     * Fields
     */
private:
    /**
     * List of all assets in game -- these may or may not be loaded.
     */
    eastl::vector<LTAsset*> m_Assets;

    /**
     * The thread used to process asset jobs.
     */
    std::thread* m_ContentThread;

    /**
     * The queue of all asset jobs to be processed.
     */
    eastl::queue<LTAssetJob> m_AssetJobs;

    /**
     * The mutex for controlling access to the asset jobs queue.
     */
    std::mutex m_AssetMutex;

    /**
     * The wrapper around the Vulkan graphics device.
     */
    class LTVKDevice* m_LTVKDevice;

    /**
     * Constructors
     */
private:
    LTAssetManager() :
        m_ContentThread(nullptr),
        m_LTVKDevice(nullptr)
    {
    }

    /**
     * Methods
     */
private:

    /**
     * Loads the asset.
     */
    bool LoadAsset(LTAssetJob& assetJob);

    /**
     * Loads the asset file from disk.
     */
    bool LoadAsset_File(
        const std::string& fileName,
        std::ifstream& file,
        uint8_t*& outBuffer,
        size_t& outSize);

    /**
     * Loads the asset according to its type.
     */
    bool LoadAsset_ByType(
        LTAssetJob& assetJob,
        uint8_t* fileBuffer,
        size_t fileSize);

    /**
     * Loads a shader asset.
     */
    bool LoadAsset_Shader(LTAssetJob& assetJob,
        uint8_t* fileBuffer,
        size_t fileSize);

    /**
     * Initializes the content lookup from a csv file on disk.
     */
    void InitializeContentLookup();

    /**
     * Handles the logic for asset jobs.
     */
    void ContentThread();

public:
    /**
     * Singleton pattern accessor
     */
    static LTAssetManager& GetInstance()
    {
        static LTAssetManager manager;
        return manager;
    }

    /**
     * Initializes the manager
     */
    void Initialize(class LTVKDevice* ltvkDevice);

    /**
     * Gets an asset, but does not load it.
     */
    bool Get(LTAssetID assetID, LTAssetHandle& outAsset);

    /**
     * Loads an asset -- this is asynchronous, the asset is not loaded immediately upon return to the caller.
     */
    bool Load(LTAssetHandle& asset);

    /**
     * Gets the asset and then loads it -- this is asynchronous, the asset is not loaded immediately upon return to the caller.
     */
    bool GetLoad(LTAssetID assetID, LTAssetHandle& outAsset);

    ///**
    // * Debugging only -- used to inspect assets
    // */
    //inline eastl::vector<LTAsset*> GetAssets() const
    //{
    //    return m_Assets;
    //}
};
