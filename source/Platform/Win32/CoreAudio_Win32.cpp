#ifdef WIN32

#include <windows.h>
#include <dsound.h>
#include <vector>
#include <cassert>

struct AudioSample
{
    float left;
    float right;
};

class AudioSource
{
public:
    AudioSource() = default;
    virtual ~AudioSource() = default;
    
    virtual bool render(AudioSample *samples, size_t count) = 0;
    
    LPDIRECTSOUND8       _dsound = nullptr;
    LPDIRECTSOUNDBUFFER8 _buffer = nullptr;
    HANDLE               _thread = nullptr;
    bool                 _stop = false;
};

static DWORD WINAPI AudioThreadProc(LPVOID lpParameter)
{
    AudioSource *source = (AudioSource *)lpParameter;
    
    const DWORD bufferSize = 4096 * sizeof(AudioSample);
    const DWORD halfBuffer = bufferSize / 2;
    std::vector<AudioSample> samples(halfBuffer / sizeof(AudioSample));
    
    DWORD currentPos, writePos;
    DWORD lastPos = 0;
    
    while (!source->_stop)
    {
        source->_buffer->GetCurrentPosition(&currentPos, &writePos);
        
        DWORD toWrite = 0;
        if (currentPos < lastPos)
        {
            toWrite = (bufferSize - lastPos) + currentPos;
        }
        else
        {
            toWrite = currentPos - lastPos;
        }
        
        if (toWrite >= halfBuffer)
        {
            LPVOID buffer1, buffer2;
            DWORD size1, size2;
            
            HRESULT hr = source->_buffer->Lock(lastPos, halfBuffer, &buffer1, &size1, &buffer2, &size2, 0);
            if (SUCCEEDED(hr))
            {
                if (!source->render(samples.data(), samples.size()))
                {
                    source->_stop = true;
                    break;
                }
                
                // Convert float samples to 16-bit PCM
                int16_t* pcm = (int16_t*)buffer1;
                for (size_t i = 0; i < samples.size() && i * 4 < size1; i++)
                {
                    pcm[i * 2] = (int16_t)(samples[i].left * 32767.0f);
                    pcm[i * 2 + 1] = (int16_t)(samples[i].right * 32767.0f);
                }
                
                if (buffer2 && size2 > 0)
                {
                    memset(buffer2, 0, size2);
                }
                
                source->_buffer->Unlock(buffer1, size1, buffer2, size2);
                lastPos = (lastPos + halfBuffer) % bufferSize;
            }
        }
        
        Sleep(10);
    }
    
    return 0;
}

class BufferAudioSource : public AudioSource
{
public:
    virtual bool render(AudioSample *samples, size_t count) override
    {
        for (size_t i = 0; i < count; i++)
        {
            if (_position < _samples.size()) {
                float v = _samples[_position++];
                samples[i] = {v, v};
            } else {
                samples[i] = {0, 0};
            }
        }
        
        return (_position < _samples.size());
    }
  
    size_t             _position = 0;
    std::vector<float> _samples;
};

void AudioPlaySound(double sample_rate, const float *sample_data, size_t sample_count)
{
    BufferAudioSource *source = new BufferAudioSource();
    source->_samples.assign(sample_data, sample_data + sample_count);
    
    // Initialize DirectSound
    HRESULT hr = DirectSoundCreate8(nullptr, &source->_dsound, nullptr);
    if (FAILED(hr)) {
        delete source;
        return;
    }
    
    // Set cooperative level
    hr = source->_dsound->SetCooperativeLevel(GetDesktopWindow(), DSSCL_PRIORITY);
    if (FAILED(hr)) {
        source->_dsound->Release();
        delete source;
        return;
    }
    
    // Create buffer
    WAVEFORMATEX waveFormat = {};
    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nChannels = 2;
    waveFormat.nSamplesPerSec = (DWORD)sample_rate;
    waveFormat.wBitsPerSample = 16;
    waveFormat.nBlockAlign = waveFormat.nChannels * waveFormat.wBitsPerSample / 8;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    
    DSBUFFERDESC bufferDesc = {};
    bufferDesc.dwSize = sizeof(DSBUFFERDESC);
    bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_GLOBALFOCUS;
    bufferDesc.dwBufferBytes = 4096 * sizeof(AudioSample);
    bufferDesc.lpwfxFormat = &waveFormat;
    
    LPDIRECTSOUNDBUFFER tempBuffer;
    hr = source->_dsound->CreateSoundBuffer(&bufferDesc, &tempBuffer, nullptr);
    if (FAILED(hr)) {
        source->_dsound->Release();
        delete source;
        return;
    }
    
    hr = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&source->_buffer);
    tempBuffer->Release();
    
    if (FAILED(hr)) {
        source->_dsound->Release();
        delete source;
        return;
    }
    
    // Start playback
    source->_buffer->Play(0, 0, DSBPLAY_LOOPING);
    
    // Create audio thread
    source->_thread = CreateThread(nullptr, 0, AudioThreadProc, source, 0, nullptr);
    
    // Wait for playback to finish
    if (source->_thread) {
        WaitForSingleObject(source->_thread, INFINITE);
        CloseHandle(source->_thread);
    }
    
    // Cleanup
    if (source->_buffer) {
        source->_buffer->Stop();
        source->_buffer->Release();
    }
    if (source->_dsound) {
        source->_dsound->Release();
    }
    delete source;
}

#endif // WIN32