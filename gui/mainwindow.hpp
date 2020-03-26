#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

// add to namespace Ui
namespace Ui {
class MainWindow;
}

/**
 * @brief The MainWindow class
 *
 * This class handles the QMainWindow that is defined
 * in the mainwindow.ui
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief MainWindow constructor
     * @param parent the parent widget
     */
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

private:
    Ui::MainWindow *ui; /**< references all ui elements */

private slots:

    /*
     * Define slots to connect the ui elements with
     */

    void setAnimationSpeed(int value);

    void setRadius(double value);
    void setLoop(bool value);
    void setOnlyShowerFront(bool value);
    void setLookatShowerFront(bool value);


    void playPause();
    void setTime(int value);

    void setLevels(int value);
    void setDistance(double value);



protected:
    virtual void keyPressEvent(QKeyEvent* event);
};

#endif // MAINWINDOW_H
