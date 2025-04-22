#pragma once

#include <QVulkanWindow>
#include <QVector>
#include "GameManager.h"
#include "ObjLoader.h"
#include "HeightMap.h"

// Structure to represent collectible objects
struct Collectible {
    QVector3D position;
    bool isCollected;

    Collectible(const QVector3D& pos) : position(pos), isCollected(false) {}
};

// Structure to represent a triangle in 3D space for heightmap terrain following
struct Triangle {
    QVector3D v0, v1, v2;  // Positions of the three vertices
};

// Structure to represent NPC enemies that patrol
struct PatrolEnemy {
    QVector3D position;     // Current position
    QVector3D pointA;       // First patrol point
    QVector3D pointB;       // Second patrol point
    bool movingToB;         // Direction flag (true = moving to B, false = moving to A)
    float speed;            // Movement speed

    PatrolEnemy(const QVector3D& startPos, const QVector3D& endPos, float moveSpeed = 0.05f) 
        : position(startPos), pointA(startPos), pointB(endPos), movingToB(true), speed(moveSpeed) {}
    
    // Update NPC position between patrol points
    void updatePosition() {
        QVector3D targetPoint = movingToB ? pointB : pointA;
        QVector3D direction = targetPoint - position;
        
        // Check if we've passed or reached the target point in the main direction of movement
        bool reachedTarget = false;
        
        // For horizontal patrol (X-axis primary movement)
        if (abs(pointB.x() - pointA.x()) > abs(pointB.z() - pointA.z())) {
            if (movingToB && position.x() >= targetPoint.x() - 0.1f) {
                reachedTarget = true;
            }
            else if (!movingToB && position.x() <= targetPoint.x() + 0.1f) {
                reachedTarget = true;
            }
        }
        // For vertical patrol (Z-axis primary movement)
        else {
            if (movingToB && position.z() >= targetPoint.z() - 0.1f) {
                reachedTarget = true;
            }
            else if (!movingToB && position.z() <= targetPoint.z() + 0.1f) {
                reachedTarget = true;
            }
        }
        
        // If we've reached the target, switch direction
        if (reachedTarget) {
            movingToB = !movingToB;
            targetPoint = movingToB ? pointB : pointA;
            direction = targetPoint - position;
        }
        
        // Normalize direction and apply speed
        if (direction.length() > 0.0001f) { // Prevent division by zero
            direction.normalize();
            position += direction * speed;
        }
    }
};

// Structure for lighting data to match fragment shader
struct LightingInfo {
    float lightPosition[3];
    float padding1;  // Explicit padding for std140 layout
    float viewPosition[3];
    float padding2;  // Explicit padding for std140 layout
    float lightColor[3];
    float ambientStrength;
    float specularStrength;
    float shininess;
    float padding3[2];  // Padding to ensure proper alignment (16-byte)
};

class RenderWindow : public QVulkanWindowRenderer
{
public:
    RenderWindow(QVulkanWindow *w, bool msaa = false);
    ~RenderWindow() override;

    //Initializes the Vulkan resources needed,
    // the buffers
    // vertex descriptions for the shaders
    // making the shaders, etc
    void initResources() override;

    //Set up resources - only MVP-matrix for now:
    void initSwapChainResources() override;

    //Empty for now - needed since we implement QVulkanWindowRenderer
    void releaseSwapChainResources() override;

    //Release Vulkan resources when program ends
    //Called by Qt
    void releaseResources() override;

    //Render the next frame
    void startNextFrame() override;

    // Player movement
    void moveForward(float distance);
    void moveRight(float distance);
    void rotate(float yawDelta, float pitchDelta);
    void moveCube(const QVector3D& movement);

    // Player position getter
    QVector3D getPlayerPosition() const { return mPlayerPosition; }

    // Collectible management
    void initializeCollectibles();
    void adjustCollectibleHeights();
    void checkCollectibleCollisions();
    int getCollectedCount() const { return mCollectedCount; }
    int getTotalCollectibles() const { return mCollectibles.size() + 1; } // +1 for indoor collectible

    // Legacy movement function for GameManager compatibility
    void movePlayer(const QVector3D& delta) { 
        if (!mGameLost) { // Only move if game is not lost
            moveForward(delta.z()); 
            moveRight(delta.x()); 
        }
    }

    //Get Vulkan info - just for fun
    void getVulkanHWInfo();

    // NPC methods
    void initializeNPCs();
    void updateNPCs();
    bool checkNPCCollision();

    // Game state management
    void resetGameState() { 
        mGameLost = false; 
        mGameWon = false;
    }
    bool isGameLost() const { return mGameLost; }

    // Door state management
    void checkDoorProximity();
    void updateDoorState(bool open);
    
    // Scene transitions
    void checkHouseEntry(const QVector3D& doorPosition);
    void transitionToScene2();
    void transitionToScene1();
    
    // Exit function for Scene 2
    void tryExitHouse();

    // Collectible handling
    void checkIndoorCollectibleCollision();
    void checkGameWinCondition();
    
    // Texture handling
    void createDefaultTexture(VkDevice device);
    void createTextureImage();
    void createTextureImageView();
    void createTextureSampler();
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                    VkImageUsageFlags usage, VkMemoryPropertyFlags properties, 
                    VkImage& image, VkDeviceMemory& imageMemory);
    VkImageView createImageView(VkImage image, VkFormat format);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    // Lighting methods
    void updateLightingData();
    
    // Model loading
    void loadNPCModel();
    void loadCrateCubeModel();

    // HeightMap related methods
    void initializeHeightMap();

private:
    VkShaderModule createShader(const QString &name);
    void updateUniformBuffer(VkDescriptorBufferInfo &bufferInfo, const QMatrix4x4 &matrix);
    
    // Heightmap terrain following using barycentric coordinates
    Triangle findTriangleAtPosition(float x, float z);
    bool isPointInTriangle2D(const QVector3D& p, const QVector3D& v0, const QVector3D& v1, const QVector3D& v2);
    QVector3D calculateBarycentric(const QVector3D& p, const QVector3D& a, const QVector3D& b, const QVector3D& c);
    float getHeightAtPosition(float x, float z);
    
    // Buffer management helpers
    void createBuffer(
        VkDeviceSize size,
        VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkBuffer& buffer,
        VkDeviceMemory& bufferMemory
    );
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    
    // Scene drawing functions
    void drawOutdoorScene(VkDevice dev, VkCommandBuffer cb, quint8* GPUmemPointer, VkResult& err);
    void drawIndoorScene(VkDevice dev, VkCommandBuffer cb, quint8* GPUmemPointer, VkResult& err);
    
    // Resource initialization
    void createIndoorSceneResources();
    
    QVulkanWindow *mWindow;
    QVulkanDeviceFunctions *mDeviceFunctions;

    // Player state
    QVector3D mPlayerPosition;    // Player position
    float mYaw = -90.0f;         // Horizontal rotation (start looking along -Z)
    QMatrix4x4 mProjectionMatrix;
    QMatrix4x4 mViewMatrix;
    float mAspectRatio = 1.0f;
    int mFrameCount = 0;

    // Game state
    GameManager* mGameManager;
    QVector<Collectible> mCollectibles;
    int mCollectedCount = 0;
    bool mGameLost = false;  // Track if player has lost
    bool mGameWon = false;   // Track if player has won
    
    // Door and house state
    bool mDoorOpen = false;
    QVector3D mHousePosition = QVector3D(0.0f, 0.0f, -12.0f);
    float mDoorOpenDistance = 3.0f; // Distance at which door opens
    
    // Scene management
    int mCurrentScene = 1; // Start in Scene 1 (outdoor)
    QVector3D mScene1PlayerPosition; // Save position when transitioning
    
    // Indoor collectible
    Collectible mIndoorCollectible = Collectible(QVector3D(2.0f, 0.0f, -2.0f)); // Special collectible inside the house

    // House buffers and memory
    VkBuffer mHouseWallsBuffer = VK_NULL_HANDLE;
    VkDeviceMemory mHouseWallsBufferMemory = VK_NULL_HANDLE;
    VkBuffer mHouseDoorBuffer = VK_NULL_HANDLE;
    VkDeviceMemory mHouseDoorBufferMemory = VK_NULL_HANDLE;
    VkBuffer mHouseRoofBuffer = VK_NULL_HANDLE;
    VkDeviceMemory mHouseRoofBufferMemory = VK_NULL_HANDLE;
    VkDescriptorSet mHouseDescriptorSet[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];
    VkDescriptorBufferInfo mHouseUniformBufferInfo[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];

    // Indoor scene resources
    VkBuffer mIndoorWallsBuffer = VK_NULL_HANDLE;
    VkDeviceMemory mIndoorWallsBufferMemory = VK_NULL_HANDLE;
    VkBuffer mExitDoorBuffer = VK_NULL_HANDLE;
    VkDeviceMemory mExitDoorBufferMemory = VK_NULL_HANDLE;
    VkDescriptorBufferInfo mIndoorUniformBufferInfo[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];
    VkDescriptorSet mIndoorDescriptorSet[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];
    
    // Vulkan resources
    VkBuffer mBuffer = VK_NULL_HANDLE;
    VkDeviceMemory mBufferMemory = VK_NULL_HANDLE;
    VkDescriptorBufferInfo mUniformBufferInfo[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];
    VkDescriptorBufferInfo mPlayerUniformBufferInfo[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];
    VkDescriptorBufferInfo mCollectibleUniformBufferInfo[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];
    
    // Separate uniform buffer info for each NPC
    VkDescriptorBufferInfo mNPCUniformBufferInfo1[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];
    VkDescriptorBufferInfo mNPCUniformBufferInfo2[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];
    VkDescriptorBufferInfo mNPCUniformBufferInfo3[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];
    
    VkDescriptorPool mDescriptorPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout mDescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorSet mDescriptorSet[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];
    VkDescriptorSet mPlayerDescriptorSet[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];
    VkDescriptorSet mCollectibleDescriptorSet[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];
    
    // Separate descriptor sets for each NPC
    VkDescriptorSet mNPCDescriptorSet1[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];
    VkDescriptorSet mNPCDescriptorSet2[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];
    VkDescriptorSet mNPCDescriptorSet3[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];
    
    VkPipelineCache mPipelineCache = VK_NULL_HANDLE;
    VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
    VkPipeline mPipeline = VK_NULL_HANDLE;

    // BUFFER RESOURCES
    VkBuffer mGroundBuffer = VK_NULL_HANDLE;
    VkDeviceMemory mGroundBufferMemory = VK_NULL_HANDLE;
    
    VkBuffer mPlayerBuffer = VK_NULL_HANDLE;
    VkDeviceMemory mPlayerBufferMemory = VK_NULL_HANDLE;
    
    VkBuffer mCollectibleBuffer = VK_NULL_HANDLE;
    VkDeviceMemory mCollectibleBufferMemory = VK_NULL_HANDLE;
    
    // NPC resources - separate buffers for different colored NPCs
    VkBuffer mNPCBuffer1 = VK_NULL_HANDLE;     // Red NPC buffer
    VkDeviceMemory mNPCBufferMemory1 = VK_NULL_HANDLE;
    
    VkBuffer mNPCBuffer2 = VK_NULL_HANDLE;     // Green NPC buffer
    VkDeviceMemory mNPCBufferMemory2 = VK_NULL_HANDLE;
    
    VkBuffer mNPCBuffer3 = VK_NULL_HANDLE;     // Blue NPC buffer
    VkDeviceMemory mNPCBufferMemory3 = VK_NULL_HANDLE;
    
    // Old buffer for backward compatibility - to be removed later
    VkBuffer mNPCBuffer = VK_NULL_HANDLE;
    VkDeviceMemory mNPCBufferMemory = VK_NULL_HANDLE;

    // Model resources for the NPCs
    VkBuffer mNPCModelBuffer = VK_NULL_HANDLE;
    VkDeviceMemory mNPCModelBufferMemory = VK_NULL_HANDLE;
    VkBuffer mNPCModelIndexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory mNPCModelIndexBufferMemory = VK_NULL_HANDLE;
    uint32_t mNPCModelIndexCount = 0;
    bool mUseNPCModel = false; // Flag to control whether to use model or basic cube

    // CrateCube model resources for NPCs
    VkBuffer mCrateCubeBuffer = VK_NULL_HANDLE;
    VkDeviceMemory mCrateCubeBufferMemory = VK_NULL_HANDLE;
    VkBuffer mCrateCubeIndexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory mCrateCubeIndexBufferMemory = VK_NULL_HANDLE;
    uint32_t mCrateCubeIndexCount = 0;

    QVector<PatrolEnemy> mNPCs;
    
    // Overlay resources for game over screen
    VkPipeline mOverlayPipeline = VK_NULL_HANDLE;
    VkPipelineLayout mOverlayPipelineLayout = VK_NULL_HANDLE;
    VkShaderModule mOverlayVertShader = VK_NULL_HANDLE;
    VkShaderModule mOverlayFragShader = VK_NULL_HANDLE;

    // Texture resources
    VkImage mTextureImage = VK_NULL_HANDLE;
    VkDeviceMemory mTextureImageMemory = VK_NULL_HANDLE;
    VkImageView mTextureImageView = VK_NULL_HANDLE;
    VkSampler mTextureSampler = VK_NULL_HANDLE;
    VkCommandPool mTempCommandPool = VK_NULL_HANDLE;
    
    // Lighting resources
    LightingInfo mLightingData;
    VkBuffer mLightingBuffer = VK_NULL_HANDLE;
    VkDeviceMemory mLightingBufferMemory = VK_NULL_HANDLE;
    VkDescriptorBufferInfo mLightingBufferInfo[QVulkanWindow::MAX_CONCURRENT_FRAME_COUNT];

    // No-texture pipeline resources
    VkPipeline mNoTexturePipeline = VK_NULL_HANDLE;
    VkShaderModule mNoTextureVertShader = VK_NULL_HANDLE;
    VkShaderModule mNoTextureFragShader = VK_NULL_HANDLE;

    // HeightMap data
    HeightMap mHeightMap;
    VkBuffer mHeightMapVertexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory mHeightMapVertexBufferMemory = VK_NULL_HANDLE;
    VkBuffer mHeightMapIndexBuffer = VK_NULL_HANDLE;
    VkDeviceMemory mHeightMapIndexBufferMemory = VK_NULL_HANDLE;
    bool mUseHeightMap = true; // Whether to use heightmap or flat ground
};
