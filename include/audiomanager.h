#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <QtCore/QObject>
#include <QtMultimedia/QAudioInput>
#include <QtMultimedia/QAudioOutput>
#include <QtCore/QBuffer>
#include <QtCore/QByteArray>
#include <QtMultimedia/QAudioFormat>
#include <QtMultimedia/QAudioDeviceInfo>
#include <QtCore/QTimer>
#include <QtCore/QMutex>
#include <portaudio.h>

/**
 * @brief Enum representing the audio transmission mode.
 */
enum class TransmissionMode {
    Raw,    ///< Raw audio data (lowest latency)
    Opus    ///< Opus-encoded audio (compressed)
};

/**
 * @brief The AudioManager class handles audio capture and playback.
 * 
 * This class is responsible for managing audio devices, capturing audio from
 * the input device, playing audio to the output device, and processing audio
 * data (raw or with Opus encoding/decoding).
 */
class AudioManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for AudioManager.
     * @param parent The parent object.
     */
    explicit AudioManager(QObject *parent = nullptr);
    
    /**
     * @brief Destructor for AudioManager.
     */
    ~AudioManager();
    
    /**
     * @brief Initializes the audio system.
     * @return True if initialization was successful, false otherwise.
     */
    bool initialize();
    
    /**
     * @brief Starts audio capture and playback.
     * @param inputDeviceName The name of the input device.
     * @param outputDeviceName The name of the output device.
     * @param sampleRate The sample rate to use.
     * @param bufferSize The buffer size to use.
     * @param mode The transmission mode (Raw or Opus).
     * @return True if started successfully, false otherwise.
     */
    bool start(const QString &inputDeviceName, const QString &outputDeviceName,
               int sampleRate, int bufferSize, TransmissionMode mode);
    
    /**
     * @brief Stops audio capture and playback.
     */
    void stop();
    
    /**
     * @brief Gets a list of available input devices.
     * @return A list of input device names.
     */
    QStringList getInputDevices() const;
    
    /**
     * @brief Gets a list of available output devices.
     * @return A list of output device names.
     */
    QStringList getOutputDevices() const;
    
    /**
     * @brief Processes incoming audio data.
     * @param data The audio data to process.
     */
    void processIncomingAudio(const QByteArray &data);
    
    /**
     * @brief Sets the transmission mode.
     * @param mode The transmission mode to use.
     */
    void setTransmissionMode(TransmissionMode mode);

signals:
    /**
     * @brief Signal emitted when audio data is ready to be sent.
     * @param data The audio data to send.
     */
    void audioDataReady(const QByteArray &data);
    
    /**
     * @brief Signal emitted when the audio level changes.
     * @param level The current audio level (0-100).
     */
    void audioLevelChanged(int level);
    
    /**
     * @brief Signal emitted when an error occurs.
     * @param errorMessage The error message.
     */
    void error(const QString &errorMessage);

private:
    /**
     * @brief Callback function for PortAudio input stream.
     * @param inputBuffer The input buffer.
     * @param outputBuffer The output buffer.
     * @param framesPerBuffer The number of frames per buffer.
     * @param timeInfo Time information.
     * @param statusFlags Status flags.
     * @param userData User data (pointer to AudioManager instance).
     * @return Whether to continue the stream.
     */
    static int inputCallback(const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo *timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData);
    
    /**
     * @brief Callback function for PortAudio output stream.
     * @param inputBuffer The input buffer.
     * @param outputBuffer The output buffer.
     * @param framesPerBuffer The number of frames per buffer.
     * @param timeInfo Time information.
     * @param statusFlags Status flags.
     * @param userData User data (pointer to AudioManager instance).
     * @return Whether to continue the stream.
     */
    static int outputCallback(const void *inputBuffer, void *outputBuffer,
                             unsigned long framesPerBuffer,
                             const PaStreamCallbackTimeInfo *timeInfo,
                             PaStreamCallbackFlags statusFlags,
                             void *userData);
    
    /**
     * @brief Calculates the audio level from raw audio data.
     * @param data The audio data.
     * @param size The size of the audio data.
     * @return The audio level (0-100).
     */
    int calculateAudioLevel(const float *data, unsigned long size) const;
    
    /**
     * @brief Encodes raw audio data using Opus.
     * @param rawData The raw audio data.
     * @return The encoded audio data.
     */
    QByteArray encodeAudio(const QByteArray &rawData);
    
    /**
     * @brief Decodes Opus-encoded audio data.
     * @param encodedData The encoded audio data.
     * @return The decoded audio data.
     */
    QByteArray decodeAudio(const QByteArray &encodedData);

    PaStream *inputStream;
    PaStream *outputStream;
    QByteArray inputBuffer;
    QByteArray outputBuffer;
    QMutex inputMutex;
    QMutex outputMutex;
    int sampleRate;
    int bufferSize;
    int channels;
    TransmissionMode transmissionMode;
    bool isInitialized;
    bool isRunning;
    
    // Opus codec state (would be implemented with actual Opus types)
    void *opusEncoder;
    void *opusDecoder;
};

#endif // AUDIOMANAGER_H
