#include "GameManager.h"
#include "RenderWindow.h"
#include <QRandomGenerator>
#include <QDebug>

GameManager::GameManager(RenderWindow* renderWindow)
    : mRenderWindow(renderWindow)
    , mCurrentScene(1)
    , mGameOver(false)
    , mGameWon(false)
    , mDoorOpen(false)
    , mCollectedPickups(0)
    , mDoorOpenDistance(2.0f)
{
    initializeScenes();
}

GameManager::~GameManager()
{
}

void GameManager::initializeScenes()
{
    // Initialize Scene 1
    initializeScene1();
    
    // Initialize Scene 2
    initializeScene2();
}

void GameManager::initializeScene1()
{
    // Create 7 pickups in random positions
    for (int i = 0; i < 7; ++i) {
        float x = QRandomGenerator::global()->generateDouble() * 8.0f - 4.0f;  // Random between -4 and 4
        float z = QRandomGenerator::global()->generateDouble() * 8.0f - 4.0f;  // Random between -4 and 4
        mPickups.append(Pickup(QVector3D(x, 0.5f, z)));
    }

    // Create 2 NPCs with random patrol points
    for (int i = 0; i < 2; ++i) {
        float x1 = QRandomGenerator::global()->generateDouble() * 8.0f - 4.0f;  // Random between -4 and 4
        float z1 = QRandomGenerator::global()->generateDouble() * 8.0f - 4.0f;  // Random between -4 and 4
        float x2 = QRandomGenerator::global()->generateDouble() * 8.0f - 4.0f;  // Random between -4 and 4
        float z2 = QRandomGenerator::global()->generateDouble() * 8.0f - 4.0f;  // Random between -4 and 4
        
        QVector3D pos(x1, 0.5f, z1);
        QVector3D p1(x1, 0.5f, z1);
        QVector3D p2(x2, 0.5f, z2);
        
        mNPCs.append(NPC(pos, p1, p2));
    }

    // Set house position (fixed for now)
    mHousePosition = QVector3D(0.0f, 0.0f, 3.0f);
    mDoorPosition = mHousePosition + QVector3D(0.0f, 0.0f, -1.0f);
}

void GameManager::initializeScene2()
{
    // Create one pickup inside the house
    mHousePickup = Pickup(QVector3D(0.0f, 0.5f, 0.0f), true);
}

void GameManager::updatePlayerPosition(const QVector3D& newPos)
{
    if (mGameOver || mGameWon) return;

    checkCollisions();
    checkDoorProximity(newPos);
}

void GameManager::checkCollisions()
{
    if (mGameOver || mGameWon) return;

    QVector3D playerPos = mRenderWindow->getPlayerPosition();

    // Check NPC collisions
    checkNPCCollisions(playerPos);

    // Check pickup collisions
    checkPickupCollisions(playerPos);
}

void GameManager::checkNPCCollisions(const QVector3D& playerPos)
{
    float collisionDistance = 1.0f; // Adjust this value as needed

    for (const NPC& npc : mNPCs) {
        float distance = (playerPos - npc.position).length();
        if (distance < collisionDistance) {
            qDebug() << "You lost!";
            mGameOver = true;
            return;
        }
    }
}

void GameManager::checkPickupCollisions(const QVector3D& playerPos)
{
    float pickupDistance = 1.0f; // Adjust this value as needed

    // Check scene 1 pickups
    for (int i = 0; i < mPickups.size(); ++i) {
        if (!mPickups[i].isCollected) {
            float distance = (playerPos - mPickups[i].position).length();
            if (distance < pickupDistance) {
                collectPickup(mPickups[i].position);
                mPickups[i].isCollected = true;
                qDebug() << "Pickup collected:" << mCollectedPickups << "of" << mPickups.size();
                
                // Check win condition
                if (mCollectedPickups == mPickups.size()) {
                    qDebug() << "You Won!";
                    mGameWon = true;
                }
            }
        }
    }

    // Check house pickup if in scene 2
    if (mCurrentScene == 2 && !mHousePickup.isCollected) {
        float distance = (playerPos - mHousePickup.position).length();
        if (distance < pickupDistance) {
            collectPickup(mHousePickup.position);
            mHousePickup.isCollected = true;
            qDebug() << "House pickup collected!";
        }
    }
}

void GameManager::collectPickup(const QVector3D& pickupPos)
{
    mCollectedPickups++;
}

void GameManager::updateNPCs()
{
    if (mGameOver || mGameWon) return;

    for (NPC& npc : mNPCs) {
        updateNPCPosition(npc);
    }
}

void GameManager::updateNPCPosition(NPC& npc)
{
    QVector3D targetPos = npc.movingToPoint1 ? npc.patrolPoint1 : npc.patrolPoint2;
    QVector3D direction = (targetPos - npc.position).normalized();
    
    // Move NPC
    npc.position += direction * npc.moveSpeed;
    
    // Check if reached target
    float distance = (npc.position - targetPos).length();
    if (distance < 0.1f) {
        npc.movingToPoint1 = !npc.movingToPoint1;
    }
}

void GameManager::checkDoorProximity(const QVector3D& playerPos)
{
    float distance = (playerPos - mDoorPosition).length();
    if (distance < mDoorOpenDistance && !mDoorOpen) {
        toggleDoor();
    }
}

void GameManager::toggleDoor()
{
    mDoorOpen = !mDoorOpen;
    if (mDoorOpen) {
        qDebug() << "Door opened!";
    } else {
        qDebug() << "Door closed!";
    }
}

void GameManager::switchToScene2()
{
    if (mDoorOpen && mCurrentScene == 1) {
        mCurrentScene = 2;
        qDebug() << "Switched to Scene 2";
    }
}

void GameManager::switchToScene1()
{
    if (mCurrentScene == 2) {
        mCurrentScene = 1;
        qDebug() << "Switched to Scene 1";
    }
}
