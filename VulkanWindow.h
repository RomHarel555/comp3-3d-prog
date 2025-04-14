#pragma once

#include <QVulkanWindow>
#include <QTimer>

class RenderWindow;

/*The QVulkanWindow subclass reimplements the factory function QVulkanWindow::createRenderer().
This returns a new instance of the QVulkanWindowRenderer subclass.
A QVulkanWindow is a Vulkan-capable QWindow that manages a Vulkan device, a graphics queue,
a command pool and buffer, a depth-stencil image and a double-buffered FIFO swapchain, etc.
In order to be able to access various Vulkan resources via the window object,
a pointer to the window is passed and stored via the constructor.*/
class VulkanWindow : public QVulkanWindow
{
    Q_OBJECT

public:
    VulkanWindow();

    QVulkanWindowRenderer* createRenderer() override;
    
    // Game status enum
    enum class GameStatus {
        Playing,
        Lost,
        Won
    };
    
    // Update game status (used to show "You lose!" message)
    void updateGameStatus(GameStatus status);

signals:
    void frameQueued(int colorValue);

private slots:
    void updateUI();

private:
    RenderWindow* mRenderWindow; // Add a pointer to the renderer
    QTimer mUpdateTimer;         // Timer for UI updates

protected:
    //The QVulkanWindow is a QWindow that we inherit from and have these functions
    // to capture mouse and keyboard.
    // Uncomment to use (you also have to make the definitions of
    // these functions in the cpp-file to use them of course!)
    //
    //    void mousePressEvent(QMouseEvent *event) override{}
    //    void mouseMoveEvent(QMouseEvent *event) override{}
    void keyPressEvent(QKeyEvent *e) override;              //the only one we use now
    //    void keyReleaseEvent(QKeyEvent *event) override{}
    //    void wheelEvent(QWheelEvent *event) override{}

};
