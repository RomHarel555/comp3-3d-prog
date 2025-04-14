#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include <QVector3D>
#include <QVector>
#include <QPair>

// Forward declarations
class RenderWindow;

// Pickup class to represent collectible items
class Pickup {
public:
    QVector3D position;
    bool isCollected;
    bool isHousePickup;  // To distinguish between scene 1 and scene 2 pickups

    // Default constructor
    Pickup() : position(0.0f, 0.0f, 0.0f), isCollected(false), isHousePickup(false) {}

    Pickup(const QVector3D& pos, bool housePickup = false)
        : position(pos), isCollected(false), isHousePickup(housePickup) {}
};

// NPC class to represent patrolling enemies
class NPC {
public:
    QVector3D position;
    QVector3D patrolPoint1;
    QVector3D patrolPoint2;
    float moveSpeed;
    bool movingToPoint1;

    NPC(const QVector3D& pos, const QVector3D& p1, const QVector3D& p2, float speed = 0.1f)
        : position(pos), patrolPoint1(p1), patrolPoint2(p2), moveSpeed(speed), movingToPoint1(true) {}
};

class GameManager : public QObject
{
    Q_OBJECT

public:
    explicit GameManager(RenderWindow* renderWindow);
    ~GameManager();

    // Scene management
    void initializeScenes();
    void switchToScene2();
    void switchToScene1();

    // Game state
    bool isGameOver() const { return mGameOver; }
    bool isGameWon() const { return mGameWon; }
    bool isInScene2() const { return mCurrentScene == 2; }

    // Player interaction
    void updatePlayerPosition(const QVector3D& newPos);
    void checkCollisions();
    void collectPickup(const QVector3D& pickupPos);

    // NPC management
    void updateNPCs();

    // House interaction
    void checkDoorProximity(const QVector3D& playerPos);
    void toggleDoor();

    // Getters
    const QVector<Pickup>& getPickups() const { return mPickups; }
    const QVector<NPC>& getNPCs() const { return mNPCs; }
    bool isDoorOpen() const { return mDoorOpen; }
    int getCollectedPickups() const { return mCollectedPickups; }
    int getTotalPickups() const { return mPickups.size(); }
    const QVector3D& getHousePosition() const { return mHousePosition; }
    const QVector3D& getDoorPosition() const { return mDoorPosition; }

private:
    RenderWindow* mRenderWindow;
    int mCurrentScene;
    bool mGameOver;
    bool mGameWon;
    bool mDoorOpen;
    int mCollectedPickups;

    // Scene 1 elements
    QVector<Pickup> mPickups;
    QVector<NPC> mNPCs;
    QVector3D mHousePosition;
    QVector3D mDoorPosition;
    float mDoorOpenDistance;

    // Scene 2 elements
    Pickup mHousePickup;

    // Helper functions
    void initializeScene1();
    void initializeScene2();
    void checkNPCCollisions(const QVector3D& playerPos);
    void checkPickupCollisions(const QVector3D& playerPos);
    void updateNPCPosition(NPC& npc);
};

#endif // GAMEMANAGER_H
