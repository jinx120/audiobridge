#include "../include/mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets/QMessageBox>
#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

/**
 * @brief Constructor for MainWindow.
 * @param parent The parent widget.
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , audioManager(new AudioManager(this))
    , networkManager(new NetworkManager(this))
    , settings(new QSettings(this))
    , audioLevelTimer(new QTimer(this))
    , isRunning(false)
    , isSenderMode(true)
{
    // Set up the UI
    ui->setupUi(this);
    setupUi();
    
    // Load settings
    loadSettings();
    
    // Populate audio devices
    populateAudioDevices();
    
    // Connect signals and slots
    connect(ui->senderRadioButton, &QRadioButton::toggled, this, &MainWindow::onModeChanged);
    connect(audioManager, &AudioManager::audioLevelChanged, this, &MainWindow::updateAudioLevel);
    connect(audioManager, &AudioManager::error, [this](const QString &errorMessage) {
        QMessageBox::critical(this, tr("Audio Error"), errorMessage);
    });
    
    connect(networkManager, &NetworkManager::connectionStatusChanged, this, &MainWindow::updateConnectionStatus);
    connect(networkManager, &NetworkManager::latencyChanged, this, &MainWindow::updateLatency);
    connect(networkManager, &NetworkManager::audioDataReceived, audioManager, &AudioManager::processIncomingAudio);
    connect(networkManager, &NetworkManager::error, [this](const QString &errorMessage) {
        QMessageBox::critical(this, tr("Network Error"), errorMessage);
    });
    
    connect(audioManager, &AudioManager::audioDataReady, networkManager, &NetworkManager::sendAudioData);
    
    // Set up audio level timer
    audioLevelTimer->setInterval(100);
    connect(audioLevelTimer, &QTimer::timeout, [this]() {
        if (!isRunning) {
            updateAudioLevel(0);
        }
    });
    audioLevelTimer->start();
    
    // Update UI for initial mode
    updateUiForMode();
}

/**
 * @brief Destructor for MainWindow.
 */
MainWindow::~MainWindow()
{
    // Save settings
    saveSettings();
    
    // Stop the audio bridge if running
    if (isRunning) {
        stopBridge();
    }
    
    // Clean up
    delete ui;
}

/**
 * @brief Handles the start/stop button click.
 */
void MainWindow::on_startStopButton_clicked()
{
    if (isRunning) {
        stopBridge();
    } else {
        startBridge();
    }
}

/**
 * @brief Handles the sender/receiver mode change.
 */
void MainWindow::onModeChanged()
{
    isSenderMode = ui->senderRadioButton->isChecked();
    updateUiForMode();
}

/**
 * @brief Updates the connection status.
 * @param connected Whether the connection is established.
 * @param message Status message to display.
 */
void MainWindow::updateConnectionStatus(bool connected, const QString &message)
{
    ui->statusLabel->setText(message);
    
    if (connected) {
        ui->statusIconLabel->setPixmap(QPixmap(":/icons/connect_icon.png"));
    } else {
        ui->statusIconLabel->setPixmap(QPixmap(":/icons/disconnect_icon.png"));
    }
}

/**
 * @brief Updates the audio level meter.
 * @param level The current audio level (0-100).
 */
void MainWindow::updateAudioLevel(int level)
{
    ui->audioLevelProgressBar->setValue(level);
}

/**
 * @brief Updates the latency display.
 * @param latencyMs The current latency in milliseconds.
 */
void MainWindow::updateLatency(int latencyMs)
{
    ui->latencyLabel->setText(tr("Latency: %1 ms").arg(latencyMs));
}

/**
 * @brief Handles theme change.
 * @param index The index of the selected theme.
 */
void MainWindow::on_themeComboBox_currentIndexChanged(int index)
{
    QString themeName = ui->themeComboBox->currentText().toLower();
    applyTheme(themeName);
}

/**
 * @brief Handles transmission mode change.
 * @param index The index of the selected transmission mode.
 */
void MainWindow::on_transmissionModeComboBox_currentIndexChanged(int index)
{
    TransmissionMode mode = (index == 0) ? TransmissionMode::Raw : TransmissionMode::Opus;
    audioManager->setTransmissionMode(mode);
}

/**
 * @brief Handles the "About" action.
 */
void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, tr("About AudioBridge"),
                      tr("<h2>AudioBridge</h2>"
                         "<p>Version %1</p>"
                         "<p>A cross-platform audio streaming application that allows you to "
                         "stream audio between computers.</p>"
                         "<p>Created by AudioBridge Team</p>").arg(QApplication::applicationVersion()));
}

/**
 * @brief Handles the "Exit" action.
 */
void MainWindow::on_actionExit_triggered()
{
    close();
}

/**
 * @brief Sets up the UI components.
 */
void MainWindow::setupUi()
{
    // Set window title
    setWindowTitle(tr("AudioBridge"));
    
    // Set window icon
    setWindowIcon(QIcon(":/icons/app_icon.png"));
    
    // Set minimum window size
    setMinimumSize(640, 480);
}

/**
 * @brief Loads the application settings.
 */
void MainWindow::loadSettings()
{
    // Load IP address and port
    ui->ipAddressLineEdit->setText(settings->value("network/ipAddress", "192.168.1.100").toString());
    ui->portSpinBox->setValue(settings->value("network/port", 8000).toInt());
    
    // Load audio settings
    int sampleRateIndex = settings->value("audio/sampleRate", 1).toInt(); // Default to 48000 Hz
    ui->sampleRateComboBox->setCurrentIndex(sampleRateIndex);
    
    int bufferSizeIndex = settings->value("audio/bufferSize", 1).toInt(); // Default to 256
    ui->bufferSizeComboBox->setCurrentIndex(bufferSizeIndex);
    
    int transmissionModeIndex = settings->value("audio/transmissionMode", 0).toInt(); // Default to Raw
    ui->transmissionModeComboBox->setCurrentIndex(transmissionModeIndex);
    
    // Load theme
    int themeIndex = settings->value("appearance/theme", 0).toInt(); // Default to Light
    ui->themeComboBox->setCurrentIndex(themeIndex);
    
    // Load mode
    bool senderMode = settings->value("general/senderMode", true).toBool();
    ui->senderRadioButton->setChecked(senderMode);
    ui->receiverRadioButton->setChecked(!senderMode);
    isSenderMode = senderMode;
}

/**
 * @brief Saves the application settings.
 */
void MainWindow::saveSettings()
{
    // Save IP address and port
    settings->setValue("network/ipAddress", ui->ipAddressLineEdit->text());
    settings->setValue("network/port", ui->portSpinBox->value());
    
    // Save audio settings
    settings->setValue("audio/sampleRate", ui->sampleRateComboBox->currentIndex());
    settings->setValue("audio/bufferSize", ui->bufferSizeComboBox->currentIndex());
    settings->setValue("audio/transmissionMode", ui->transmissionModeComboBox->currentIndex());
    
    // Save theme
    settings->setValue("appearance/theme", ui->themeComboBox->currentIndex());
    
    // Save mode
    settings->setValue("general/senderMode", ui->senderRadioButton->isChecked());
}

/**
 * @brief Populates the audio device combo boxes.
 */
void MainWindow::populateAudioDevices()
{
    // Get input devices
    QStringList inputDevices = audioManager->getInputDevices();
    ui->inputDeviceComboBox->clear();
    ui->inputDeviceComboBox->addItems(inputDevices);
    
    // Get output devices
    QStringList outputDevices = audioManager->getOutputDevices();
    ui->outputDeviceComboBox->clear();
    ui->outputDeviceComboBox->addItems(outputDevices);
    
    // Select default devices
    QString defaultInputDevice = settings->value("audio/inputDevice", "").toString();
    if (!defaultInputDevice.isEmpty() && inputDevices.contains(defaultInputDevice)) {
        ui->inputDeviceComboBox->setCurrentText(defaultInputDevice);
    }
    
    QString defaultOutputDevice = settings->value("audio/outputDevice", "").toString();
    if (!defaultOutputDevice.isEmpty() && outputDevices.contains(defaultOutputDevice)) {
        ui->outputDeviceComboBox->setCurrentText(defaultOutputDevice);
    }
}

/**
 * @brief Applies the selected theme.
 * @param themeName The name of the theme to apply.
 */
void MainWindow::applyTheme(const QString &themeName)
{
    QString stylesheetPath;
    
    if (themeName == "dark") {
        stylesheetPath = ":/styles/dark_style.qss";
    } else {
        stylesheetPath = ":/styles/light_style.qss";
    }
    
    QFile file(stylesheetPath);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&file);
        qApp->setStyleSheet(stream.readAll());
        file.close();
    }
}

/**
 * @brief Updates the UI based on the current mode (sender/receiver).
 */
void MainWindow::updateUiForMode()
{
    if (isSenderMode) {
        ui->ipAddressLabel->setText(tr("Server IP:"));
        ui->inputDeviceLabel->setText(tr("Input Device (Send):"));
        ui->outputDeviceLabel->setText(tr("Output Device (Monitor):"));
    } else {
        ui->ipAddressLabel->setText(tr("Listen IP:"));
        ui->inputDeviceLabel->setText(tr("Input Device (Monitor):"));
        ui->outputDeviceLabel->setText(tr("Output Device (Receive):"));
    }
}

/**
 * @brief Starts the audio bridge.
 */
void MainWindow::startBridge()
{
    // Get settings
    QString ipAddress = ui->ipAddressLineEdit->text();
    int port = ui->portSpinBox->value();
    QString inputDevice = ui->inputDeviceComboBox->currentText();
    QString outputDevice = ui->outputDeviceComboBox->currentText();
    
    int sampleRate = (ui->sampleRateComboBox->currentIndex() == 0) ? 44100 : 48000;
    
    int bufferSize;
    switch (ui->bufferSizeComboBox->currentIndex()) {
        case 0: bufferSize = 128; break;
        case 1: bufferSize = 256; break;
        case 2: bufferSize = 512; break;
        case 3: bufferSize = 1024; break;
        default: bufferSize = 256;
    }
    
    TransmissionMode mode = (ui->transmissionModeComboBox->currentIndex() == 0) 
                            ? TransmissionMode::Raw 
                            : TransmissionMode::Opus;
    
    // Initialize audio
    if (!audioManager->initialize()) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to initialize audio system."));
        return;
    }
    
    // Start network
    bool networkStarted;
    if (isSenderMode) {
        networkStarted = networkManager->connectToServer(ipAddress, port);
    } else {
        networkStarted = networkManager->startServer(port);
    }
    
    if (!networkStarted) {
        QMessageBox::critical(this, tr("Error"), 
                             tr("Failed to %1.").arg(isSenderMode ? "connect to server" : "start server"));
        return;
    }
    
    // Start audio
    if (!audioManager->start(inputDevice, outputDevice, sampleRate, bufferSize, mode)) {
        networkManager->disconnect();
        QMessageBox::critical(this, tr("Error"), tr("Failed to start audio system."));
        return;
    }
    
    // Update UI
    isRunning = true;
    ui->startStopButton->setText(tr("Stop"));
    ui->tabWidget->setEnabled(false);
    ui->modeGroupBox->setEnabled(false);
}

/**
 * @brief Stops the audio bridge.
 */
void MainWindow::stopBridge()
{
    // Stop audio
    audioManager->stop();
    
    // Stop network
    networkManager->disconnect();
    
    // Update UI
    isRunning = false;
    ui->startStopButton->setText(tr("Start"));
    ui->tabWidget->setEnabled(true);
    ui->modeGroupBox->setEnabled(true);
    
    // Reset status
    updateConnectionStatus(false, tr("Disconnected"));
    updateLatency(0);
}
