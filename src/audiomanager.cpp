#include "../include/audiomanager.h"
#include <QtCore/QDebug>
#include <cmath>
#include <algorithm>

/**
 * @brief Constructor for AudioManager.
 * @param parent The parent object.
 */
AudioManager::AudioManager(QObject *parent)
    : QObject(parent)
    , inputStream(nullptr)
    , outputStream(nullptr)
    , sampleRate(48000)
    , bufferSize(256)
    , channels(2)
    , transmissionMode(TransmissionMode::Raw)
    , isInitialized(false)
    , isRunning(false)
    , opusEncoder(nullptr)
    , opusDecoder(nullptr)
{
}

/**
 * @brief Destructor for AudioManager.
 */
AudioManager::~AudioManager()
{
    stop();
    
    if (isInitialized) {
        Pa_Terminate();
        isInitialized = false;
    }
}

/**
 * @brief Initializes the audio system.
 * @return True if initialization was successful, false otherwise.
 */
bool AudioManager::initialize()
{
    if (isInitialized) {
        return true;
    }
    
    PaError err = Pa_Initialize();
    if (err != paNoError) {
        emit error(tr("PortAudio initialization failed: %1").arg(Pa_GetErrorText(err)));
        return false;
    }
    
    isInitialized = true;
    return true;
}

/**
 * @brief Starts audio capture and playback.
 * @param inputDeviceName The name of the input device.
 * @param outputDeviceName The name of the output device.
 * @param sampleRate The sample rate to use.
 * @param bufferSize The buffer size to use.
 * @param mode The transmission mode (Raw or Opus).
 * @return True if started successfully, false otherwise.
 */
bool AudioManager::start(const QString &inputDeviceName, const QString &outputDeviceName,
                        int sampleRate, int bufferSize, TransmissionMode mode)
{
    if (isRunning) {
        stop();
    }
    
    if (!isInitialized && !initialize()) {
        return false;
    }
    
    this->sampleRate = sampleRate;
    this->bufferSize = bufferSize;
    this->transmissionMode = mode;
    
    // Find input device
    int inputDeviceIndex = Pa_GetDefaultInputDevice();
    int numDevices = Pa_GetDeviceCount();
    for (int i = 0; i < numDevices; i++) {
        const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo && QString(deviceInfo->name) == inputDeviceName && deviceInfo->maxInputChannels > 0) {
            inputDeviceIndex = i;
            break;
        }
    }
    
    // Find output device
    int outputDeviceIndex = Pa_GetDefaultOutputDevice();
    for (int i = 0; i < numDevices; i++) {
        const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo && QString(deviceInfo->name) == outputDeviceName && deviceInfo->maxOutputChannels > 0) {
            outputDeviceIndex = i;
            break;
        }
    }
    
    // Set up input stream parameters
    PaStreamParameters inputParams;
    inputParams.device = inputDeviceIndex;
    inputParams.channelCount = channels;
    inputParams.sampleFormat = paFloat32;
    inputParams.suggestedLatency = Pa_GetDeviceInfo(inputDeviceIndex)->defaultLowInputLatency;
    inputParams.hostApiSpecificStreamInfo = nullptr;
    
    // Set up output stream parameters
    PaStreamParameters outputParams;
    outputParams.device = outputDeviceIndex;
    outputParams.channelCount = channels;
    outputParams.sampleFormat = paFloat32;
    outputParams.suggestedLatency = Pa_GetDeviceInfo(outputDeviceIndex)->defaultLowOutputLatency;
    outputParams.hostApiSpecificStreamInfo = nullptr;
    
    // Open input stream
    PaError err = Pa_OpenStream(&inputStream,
                               &inputParams,
                               nullptr,
                               sampleRate,
                               bufferSize,
                               paClipOff,
                               AudioManager::inputCallback,
                               this);
    
    if (err != paNoError) {
        emit error(tr("Failed to open input stream: %1").arg(Pa_GetErrorText(err)));
        return false;
    }
    
    // Open output stream
    err = Pa_OpenStream(&outputStream,
                       nullptr,
                       &outputParams,
                       sampleRate,
                       bufferSize,
                       paClipOff,
                       AudioManager::outputCallback,
                       this);
    
    if (err != paNoError) {
        Pa_CloseStream(inputStream);
        inputStream = nullptr;
        emit error(tr("Failed to open output stream: %1").arg(Pa_GetErrorText(err)));
        return false;
    }
    
    // Start streams
    err = Pa_StartStream(inputStream);
    if (err != paNoError) {
        Pa_CloseStream(inputStream);
        Pa_CloseStream(outputStream);
        inputStream = nullptr;
        outputStream = nullptr;
        emit error(tr("Failed to start input stream: %1").arg(Pa_GetErrorText(err)));
        return false;
    }
    
    err = Pa_StartStream(outputStream);
    if (err != paNoError) {
        Pa_StopStream(inputStream);
        Pa_CloseStream(inputStream);
        Pa_CloseStream(outputStream);
        inputStream = nullptr;
        outputStream = nullptr;
        emit error(tr("Failed to start output stream: %1").arg(Pa_GetErrorText(err)));
        return false;
    }
    
    // Initialize Opus codec if needed
    if (transmissionMode == TransmissionMode::Opus) {
        // In a real implementation, we would initialize the Opus encoder and decoder here
        // For this example, we'll just simulate it
        opusEncoder = (void*)1; // Placeholder
        opusDecoder = (void*)1; // Placeholder
    }
    
    isRunning = true;
    return true;
}

/**
 * @brief Stops audio capture and playback.
 */
void AudioManager::stop()
{
    if (!isRunning) {
        return;
    }
    
    // Stop and close streams
    if (inputStream) {
        Pa_StopStream(inputStream);
        Pa_CloseStream(inputStream);
        inputStream = nullptr;
    }
    
    if (outputStream) {
        Pa_StopStream(outputStream);
        Pa_CloseStream(outputStream);
        outputStream = nullptr;
    }
    
    // Clean up Opus codec if needed
    if (transmissionMode == TransmissionMode::Opus) {
        // In a real implementation, we would clean up the Opus encoder and decoder here
        opusEncoder = nullptr;
        opusDecoder = nullptr;
    }
    
    isRunning = false;
}

/**
 * @brief Gets a list of available input devices.
 * @return A list of input device names.
 */
QStringList AudioManager::getInputDevices() const
{
    QStringList devices;
    
    if (!isInitialized && !const_cast<AudioManager*>(this)->initialize()) {
        return devices;
    }
    
    int numDevices = Pa_GetDeviceCount();
    for (int i = 0; i < numDevices; i++) {
        const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo && deviceInfo->maxInputChannels > 0) {
            devices.append(QString(deviceInfo->name));
        }
    }
    
    return devices;
}

/**
 * @brief Gets a list of available output devices.
 * @return A list of output device names.
 */
QStringList AudioManager::getOutputDevices() const
{
    QStringList devices;
    
    if (!isInitialized && !const_cast<AudioManager*>(this)->initialize()) {
        return devices;
    }
    
    int numDevices = Pa_GetDeviceCount();
    for (int i = 0; i < numDevices; i++) {
        const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);
        if (deviceInfo && deviceInfo->maxOutputChannels > 0) {
            devices.append(QString(deviceInfo->name));
        }
    }
    
    return devices;
}

/**
 * @brief Processes incoming audio data.
 * @param data The audio data to process.
 */
void AudioManager::processIncomingAudio(const QByteArray &data)
{
    if (!isRunning) {
        return;
    }
    
    QByteArray processedData;
    
    // Decode if needed
    if (transmissionMode == TransmissionMode::Opus) {
        processedData = decodeAudio(data);
    } else {
        processedData = data;
    }
    
    // Add to output buffer
    QMutexLocker locker(&outputMutex);
    outputBuffer = processedData;
}

/**
 * @brief Sets the transmission mode.
 * @param mode The transmission mode to use.
 */
void AudioManager::setTransmissionMode(TransmissionMode mode)
{
    if (transmissionMode == mode) {
        return;
    }
    
    transmissionMode = mode;
    
    // If running, restart with new mode
    if (isRunning) {
        QString inputDevice = getInputDevices().at(0); // Just use default for this example
        QString outputDevice = getOutputDevices().at(0); // Just use default for this example
        stop();
        start(inputDevice, outputDevice, sampleRate, bufferSize, mode);
    }
}

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
int AudioManager::inputCallback(const void *inputBuffer, void *outputBuffer,
                               unsigned long framesPerBuffer,
                               const PaStreamCallbackTimeInfo *timeInfo,
                               PaStreamCallbackFlags statusFlags,
                               void *userData)
{
    AudioManager *self = static_cast<AudioManager*>(userData);
    
    if (!self || !inputBuffer) {
        return paContinue;
    }
    
    // Calculate audio level
    const float *samples = static_cast<const float*>(inputBuffer);
    int level = self->calculateAudioLevel(samples, framesPerBuffer * self->channels);
    emit self->audioLevelChanged(level);
    
    // Process audio data
    QByteArray data(reinterpret_cast<const char*>(inputBuffer), 
                   framesPerBuffer * self->channels * sizeof(float));
    
    // Encode if needed
    if (self->transmissionMode == TransmissionMode::Opus) {
        data = self->encodeAudio(data);
    }
    
    // Emit audio data ready signal
    emit self->audioDataReady(data);
    
    return paContinue;
}

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
int AudioManager::outputCallback(const void *inputBuffer, void *outputBuffer,
                                unsigned long framesPerBuffer,
                                const PaStreamCallbackTimeInfo *timeInfo,
                                PaStreamCallbackFlags statusFlags,
                                void *userData)
{
    AudioManager *self = static_cast<AudioManager*>(userData);
    
    if (!self || !outputBuffer) {
        return paContinue;
    }
    
    // Get output data
    QMutexLocker locker(&self->outputMutex);
    
    // Fill output buffer
    float *out = static_cast<float*>(outputBuffer);
    size_t bytesPerFrame = self->channels * sizeof(float);
    size_t bytesToCopy = framesPerBuffer * bytesPerFrame;
    
    if (self->outputBuffer.size() >= bytesToCopy) {
        // Copy data from output buffer
        memcpy(out, self->outputBuffer.constData(), bytesToCopy);
    } else {
        // Fill with silence
        memset(out, 0, bytesToCopy);
    }
    
    return paContinue;
}

/**
 * @brief Calculates the audio level from raw audio data.
 * @param data The audio data.
 * @param size The size of the audio data.
 * @return The audio level (0-100).
 */
int AudioManager::calculateAudioLevel(const float *data, unsigned long size) const
{
    if (!data || size == 0) {
        return 0;
    }
    
    // Calculate RMS (Root Mean Square)
    float sum = 0.0f;
    for (unsigned long i = 0; i < size; i++) {
        sum += data[i] * data[i];
    }
    
    float rms = std::sqrt(sum / size);
    
    // Convert to dB
    float db = 20.0f * std::log10(rms);
    
    // Normalize to 0-100 range
    // Assuming typical audio levels: -60dB (quiet) to 0dB (loud)
    int level = static_cast<int>((db + 60.0f) * 100.0f / 60.0f);
    
    // Clamp to 0-100
    return std::max(0, std::min(100, level));
}

/**
 * @brief Encodes raw audio data using Opus.
 * @param rawData The raw audio data.
 * @return The encoded audio data.
 */
QByteArray AudioManager::encodeAudio(const QByteArray &rawData)
{
    // In a real implementation, we would use the Opus encoder here
    // For this example, we'll just return the raw data
    // This is a placeholder for actual Opus encoding
    
    // Simulate compression (just to show the concept)
    QByteArray encodedData;
    encodedData.append("OPUS"); // Add a marker
    encodedData.append(rawData); // In reality, this would be compressed
    
    return encodedData;
}

/**
 * @brief Decodes Opus-encoded audio data.
 * @param encodedData The encoded audio data.
 * @return The decoded audio data.
 */
QByteArray AudioManager::decodeAudio(const QByteArray &encodedData)
{
    // In a real implementation, we would use the Opus decoder here
    // For this example, we'll just check for our marker and return the raw data
    // This is a placeholder for actual Opus decoding
    
    if (encodedData.startsWith("OPUS")) {
        // Remove our marker
        return encodedData.mid(4);
    }
    
    // If not Opus encoded, return as is
    return encodedData;
}
