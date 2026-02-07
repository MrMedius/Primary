#pragma once
#pragma comment(lib, "Xaudio2.lib")

#include <wrl/client.h>
#include <windows.h>
#include <xaudio2.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "SoundPaths.h"

class SoundCodex
{
public:
    // Global Singleton
    static SoundCodex& Get() noexcept
    {
        static SoundCodex inst;
        return inst;
    }

    SoundCodex(const SoundCodex&) = delete;
    SoundCodex& operator=(const SoundCodex&) = delete;

    // Explicit initialization / completion
    bool Init(HWND hWnd);
    void UnInit();

    // Per-frame call: Recycle the played SE voice
    void Update();

    // BGM
    bool PlayBGM(std::string path, int loopCount = -1);
    void StopBGM();

    // BUS volume
    void SetMasterVolume(float vol); // 0..1
    void SetBgmVolume(float vol);    // 0..1
    void SetSeVolume(float vol);     // 0..1

    // SE
    bool PlaySE(std::string path, int loopCount = 0, float volume = 1.0f);
    void StopAllSE();

private:
    SoundCodex() = default;
    ~SoundCodex() = default;

private:
    struct AudioClip
    {
        WAVEFORMATEX fmt{};
        std::vector<BYTE> data;
    };

    struct SeVoiceSlot
    {
        IXAudio2SourceVoice* voice = nullptr;
        WAVEFORMATEX fmt{}; // format in which the voice was created
        bool inUse = false;
        float volume = 1.0f;
        std::shared_ptr<AudioClip> clip; // keep only during playback
        uint64_t lastUseTick = 0;
    };

private:
    // get cache (if not, load and store)
    std::shared_ptr<AudioClip> GetOrLoadClip_(std::string path);
    // wav import
    std::shared_ptr<AudioClip> LoadWavClip_(std::string path);

    // voice create / destroy
    // ------------------------------------------
    // [C-02] CreateSourceVoice_ に "送信先(バス)" を指定できるようにする
    IXAudio2SourceVoice* CreateSourceVoice_(const WAVEFORMATEX& fmt, IXAudio2Voice* dstBus);
    void DestroySourceVoice_(IXAudio2SourceVoice*& v);
    // ------------------------------------------
    
    // slot acquire (free, addition, or replacement)
    SeVoiceSlot* AcquireSeSlot_(const WAVEFORMATEX& fmt);
    
    // wav Chunk loading
    HRESULT CheckChunk_(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition);
    HRESULT ReadChunkData_(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);
    
    // safer format compare than memcmp (WAVEFORMATEX)
    static bool FormatEquals_(const WAVEFORMATEX& a, const WAVEFORMATEX& b);
private:
    // XAudio2 core
    Microsoft::WRL::ComPtr<IXAudio2> XAudio2;
    //BUS (SubmixVoice)
    IXAudio2MasteringVoice* masterVoice = nullptr;
    IXAudio2SubmixVoice* bgmBus = nullptr;
    IXAudio2SubmixVoice* seBus  = nullptr;

    // bus volumes
    float masterVolume = 1.0f;
    float bgmBusVolume = 1.0f;
    float seBusVolume  = 1.0f;

    // cache => key = path string
    std::unordered_map<std::string, std::shared_ptr<AudioClip>> clipCache;

    // BGM
    IXAudio2SourceVoice* bgmVoice = nullptr;
    std::shared_ptr<AudioClip> bgmClip;
    float bgmVolume = 1.0f;

    // SE pool
    std::vector<SeVoiceSlot> sePool;

    // SE slot limit + tick counter
    static constexpr size_t kMaxSeVoices = 64;
    uint64_t m_Tick = 1;

	// initialization flag
    bool inited = false;
};
