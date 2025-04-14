#include "VulkanWindow.h"
#include "RenderWindow.h"
#include <QKeyEvent>
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>

VulkanWindow::VulkanWindow() : mRenderWindow(nullptr)
{
    setTitle("Cube Collection Game - 0/6 collected");
    
    connect(&mUpdateTimer, &QTimer::timeout, this, &VulkanWindow::updateUI);
    mUpdateTimer.start(500); // Update every 500ms
}

QVulkanWindowRenderer* VulkanWindow::createRenderer()
{
    mRenderWindow = new RenderWindow(this, true);
    return mRenderWindow;
}

void VulkanWindow::updateUI()
{
    if (mRenderWindow) {
        int collected = mRenderWindow->getCollectedCount();
        int total = mRenderWindow->getTotalCollectibles();
        setTitle(QString("Cube Collection Game - %1/%2 collected").arg(collected).arg(total));
        
        if (collected == total && total > 0) {
            setTitle(QString("Cube Collection Game - All %1 collected! Congratulations!").arg(total));
        }
    }
}

void VulkanWindow::updateGameStatus(GameStatus status)
{
    switch (status) {
    case GameStatus::Lost:
        setTitle("!!! YOU LOSE !!! Enemy collision! Press R to restart");
        
        qDebug() << "\n*******************************************";
        qDebug() << "***    GAME OVER - ENEMY COLLISION     ***";
        qDebug() << "***      Press R to restart game       ***";
        qDebug() << "*******************************************\n";
        
        break;
    case GameStatus::Won:
        setTitle("YOU WIN! All collectibles gathered! Congratulations!");
        
        qDebug() << "\n*******************************************";
        qDebug() << "***            VICTORY!                 ***";
        qDebug() << "*** All collectibles have been gathered ***";
        qDebug() << "***        CONGRATULATIONS!             ***";
        qDebug() << "*******************************************\n";
        
        break;
    case GameStatus::Playing:
        updateUI();
        break;
    }
}

void VulkanWindow::keyPressEvent(QKeyEvent *e)
{
    const float moveSpeed = 0.5f;  // Moderate speed for controlled movement

    switch (e->key()) {
    case Qt::Key_W:
        if (mRenderWindow) {
            mRenderWindow->moveCube(QVector3D(0.0f, 0.0f, moveSpeed)); // Forward is +Z
        }
        break;
    case Qt::Key_S:
        if (mRenderWindow) {
            mRenderWindow->moveCube(QVector3D(0.0f, 0.0f, -moveSpeed));  // Backward is -Z
        }
        break;
    case Qt::Key_A:
        if (mRenderWindow) {
            mRenderWindow->moveCube(QVector3D(-moveSpeed, 0.0f, 0.0f)); // Left is -X
        }
        break;
    case Qt::Key_D:
        if (mRenderWindow) {
            mRenderWindow->moveCube(QVector3D(moveSpeed, 0.0f, 0.0f));  // Right is +X
        }
        break;
    case Qt::Key_Space:
        if (mRenderWindow) {
            mRenderWindow->moveCube(QVector3D(-mRenderWindow->getPlayerPosition().x(), 
                                           0.0f, 
                                           -mRenderWindow->getPlayerPosition().z()));
        }
        break;
    case Qt::Key_R:
        if (mRenderWindow) {
            // Reset player position
            mRenderWindow->moveCube(QVector3D(-mRenderWindow->getPlayerPosition().x(), 
                                           0.0f, 
                                           -mRenderWindow->getPlayerPosition().z()));
            mRenderWindow->initializeCollectibles();
            updateGameStatus(GameStatus::Playing);
            mRenderWindow->resetGameState();
        }
        break;
    case Qt::Key_E:
        if (mRenderWindow) {
            mRenderWindow->tryExitHouse();
        }
        break;
    case Qt::Key_Escape:
        QCoreApplication::quit();
        break;
    default:
        QVulkanWindow::keyPressEvent(e);
        break;
    }
}
