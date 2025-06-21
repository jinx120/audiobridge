#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtCore/QSettings>
#include <QtCore/QTimer>
#include "audiomanager.h"
#include "networkmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief The MainWindow class represents the main application window.
 * 
 * This class is responsible for setting up the UI, handling user interactions,
 * managing the application state, and connecting to the AudioManager and NetworkManager.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for MainWindow.
     * @param parent The parent widget.
     */
    MainWindow(QWidget *parent = nullptr);
    
    /**
     * @brief Destructor for MainWindow.
     */
    ~MainWindow();

private slots:
    /**
     * @brief Handles the start/stop button click.
     */
    void on_startStopButton_clicked();
    
    /**
     * @brief Handles the sender/receiver mode change.
     */
    void onModeChanged();
    
    /**
     * @brief Updates the connection status.
     * @param connected Whether the connection is established.
     * @param message Status message to display.
     */
    void updateConnectionStatus(bool connected, const QString &message);
    
    /**
     * @brief Updates the audio level meter.
     * @param level The current audio level (0-100).
     */
    void updateAudioLevel(int level);
    
    /**
     * @brief Updates the latency display.
     * @param latencyMs The current latency in milliseconds.
     */
    void updateLatency(int latencyMs);
    
    /**
     * @brief Handles theme change.
     * @param index The index of the selected theme.
     */
    void on_themeComboBox_currentIndexChanged(int index);
    
    /**
     * @brief Handles transmission mode change.
     * @param index The index of the selected transmission mode.
     */
    void on_transmissionModeComboBox_currentIndexChanged(int index);
    
    /**
     * @brief Handles the "About" action.
     */
    void on_actionAbout_triggered();
    
    /**
     * @brief Handles the "Exit" action.
     */
    void on_actionExit_triggered();

private:
    /**
     * @brief Sets up the UI components.
     */
    void setupUi();
    
    /**
     * @brief Loads the application settings.
     */
    void loadSettings();
    
    /**
     * @brief Saves the application settings.
     */
    void saveSettings();
    
    /**
     * @brief Populates the audio device combo boxes.
     */
    void populateAudioDevices();
    
    /**
     * @brief Applies the selected theme.
     * @param themeName The name of the theme to apply.
     */
    void applyTheme(const QString &themeName);
    
    /**
     * @brief Updates the UI based on the current mode (sender/receiver).
     */
    void updateUiForMode();
    
    /**
     * @brief Starts the audio bridge.
     */
    void startBridge();
    
    /**
     * @brief Stops the audio bridge.
     */
    void stopBridge();

    Ui::MainWindow *ui;
    AudioManager *audioManager;
    NetworkManager *networkManager;
    QSettings *settings;
    QTimer *audioLevelTimer;
    bool isRunning;
    bool isSenderMode;
};

#endif // MAINWINDOW_H
