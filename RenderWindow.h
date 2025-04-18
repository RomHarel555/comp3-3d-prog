#pragma once

#include <QVulkanWindow>
#include <QVector>
#include "GameManager.h"

// Structure to represent collectible objects
struct Collectible {
    QVector3D position;
    bool collected;

    Collectible(const QVector3D& pos) : position(pos), collected(false) {}
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
        
        // If we've reached the target point (or very close to it)
        if (direction.length() < 0.1f) {
            // Switch direction
            movingToB = !movingToB;
            // Recalculate direction after switching
            targetPoint = movingToB ? pointB : pointA;
            direction = targetPoint - position;
        }
        
        // Normalize direction and apply speed
        if (direction.length() > 0) {
            direction.normalize();
            position += direction * speed;
        }
    }
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

private:
    VkShaderModule createShader(const QString &name);
    void updateUniformBuffer(VkDescriptorBufferInfo &bufferInfo, const QMatrix4x4 &matrix);
    
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
};
