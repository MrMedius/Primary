#include "SoundCodex.h"
#include <algorithm>
#include "Util.h"

bool SoundCodex::Init(HWND hWnd)
{
    if (inited) return true;

    // COM init
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr))
    {
        MessageBox(hWnd, "COM initialization failed!", "WARNINGÅI", MB_ICONWARNING);
        return false;
    }

    hr = XAudio2Create(XAudio2.GetAddressOf(), 0);
    if (FAILED(hr))
    {
        MessageBox(hWnd, "Failed to create XAudio2 object!", "WARNINGÅI", MB_ICONWARNING);
        CoUninitialize();
        return false;
    }

    hr = XAudio2->CreateMasteringVoice(&masterVoice);
    if (FAILED(hr))
    {
        MessageBox(hWnd, "Master voice generation failed!", "WARNINGÅI", MB_ICONWARNING);
        XAudio2.Reset();
        CoUninitialize();
        return false;
    }

    // Submix(Bus) init
    XAUDIO2_VOICE_DETAILS mv{};
    masterVoice->GetVoiceDetails(&mv);
    hr = XAudio2->CreateSubmixVoice(&bgmBus, mv.InputChannels, mv.InputSampleRate);
    if (FAILED(hr))
    {
        MessageBoxA(hWnd, "Failed to create BGM bus (SubmixVoice)!", "WARNING", MB_ICONWARNING);
        masterVoice->DestroyVoice();
        masterVoice = nullptr;
        XAudio2.Reset();
        CoUninitialize();
        return false;
    }
    hr = XAudio2->CreateSubmixVoice(&seBus, mv.InputChannels, mv.InputSampleRate);
    if (FAILED(hr))
    {
        MessageBoxA(hWnd, "Failed to create SE bus (SubmixVoice)!", "WARNING", MB_ICONWARNING);
        if (bgmBus) { bgmBus->DestroyVoice(); bgmBus = nullptr; }
        masterVoice->DestroyVoice();
        masterVoice = nullptr;
        XAudio2.Reset();
        CoUninitialize();
        return false;
    }
    masterVoice->SetVolume(std::clamp(masterVolume, 0.0f, 1.0f));
    bgmBus->SetVolume(std::clamp(bgmBusVolume, 0.0f, 1.0f));
    seBus->SetVolume(std::clamp(seBusVolume, 0.0f, 1.0f));

    inited = true;
    return true;
}

void SoundCodex::UnInit()
{
    if (!inited) return;

    StopBGM();
    StopAllSE();

    // SE voice discard
    for (auto& s : sePool)
    {
        DestroySourceVoice_(s.voice);
        s.inUse = false;
        s.clip.reset();
        std::memset(&s.fmt, 0, sizeof(WAVEFORMATEX));
    }
    sePool.clear();

    // discard the cache (AudioClip is a shared_ptr, so it is released here)
    clipCache.clear();

    // Bus discard
    if (seBus)  { seBus->DestroyVoice();  seBus = nullptr; }
    if (bgmBus) { bgmBus->DestroyVoice(); bgmBus = nullptr; }


    if (masterVoice)
    {
        masterVoice->DestroyVoice();
        masterVoice = nullptr;
    }

    if (XAudio2)
    {
        XAudio2.Reset();
    }

    CoUninitialize();
    inited = false;
}

void SoundCodex::Update()
{
    if (!inited) return;

    // return the SE slot to "empty" after playback is complete
    for (auto& s : sePool)
    {
        if (!s.voice || !s.inUse) continue;

        XAUDIO2_VOICE_STATE st{};
        s.voice->GetState(&st);

        if (st.BuffersQueued == 0)
        {
            // end of playback: release slot (leave voice = reuse if format matches)
            s.inUse = false;
            s.clip.reset();
            // just to be safe
            s.voice->FlushSourceBuffers();
        }
    }
}


// ------------------------------------------
// BUS volume
void SoundCodex::SetMasterVolume(float vol)
{
    masterVolume = std::clamp(vol, 0.0f, 1.0f);
    if (masterVoice) masterVoice->SetVolume(masterVolume);
}

void SoundCodex::SetBgmVolume(float vol)
{
    bgmBusVolume = std::clamp(vol, 0.0f, 1.0f);
    if (bgmBus) bgmBus->SetVolume(bgmBusVolume);
}

void SoundCodex::SetSeVolume(float vol)
{
    seBusVolume = std::clamp(vol, 0.0f, 1.0f);
    if (seBus) seBus->SetVolume(seBusVolume);
}


// ------------------------------------------
// BGM
bool SoundCodex::PlayBGM(std::string path, int loopCount)
{
    if (!inited) return false;

    auto clip = GetOrLoadClip_(path);
    if (!clip) return false;

    // stop the existing BGM and replace it
    StopBGM();

    // set BGM voice output destination to bgmBus
    bgmVoice = CreateSourceVoice_(clip->fmt, bgmBus);

    if (!bgmVoice) return false;

    bgmClip = clip;

    XAUDIO2_BUFFER buf{};
    buf.AudioBytes = static_cast<UINT32>(clip->data.size());
    buf.pAudioData = clip->data.data();
    buf.Flags = XAUDIO2_END_OF_STREAM;
    if (loopCount < 0) loopCount = XAUDIO2_LOOP_INFINITE;
    buf.LoopCount = loopCount;

    bgmVoice->SubmitSourceBuffer(&buf);
    bgmVoice->SetVolume(1.0f);
    bgmVoice->Start(0);

    return true;
}

void SoundCodex::StopBGM()
{
    if (bgmVoice)
    {
        bgmVoice->Stop(0);
        bgmVoice->FlushSourceBuffers();
        bgmVoice->DestroyVoice();
        bgmVoice = nullptr;
    }
    bgmClip.reset();
}


// ------------------------------------------
// SE
bool SoundCodex::PlaySE(std::string path, int loopCount, float volume)
{
    if (!inited) return false;

    auto clip = GetOrLoadClip_(path);
    if (!clip) return false;

    volume = std::clamp(volume, 0.0f, 1.0f);

    SeVoiceSlot* slot = AcquireSeSlot_(clip->fmt);
    if (!slot || !slot->voice) return false;

    // safety precautions when reusing
    slot->voice->Stop(0);
    slot->voice->FlushSourceBuffers();

    XAUDIO2_BUFFER buf{};
    buf.AudioBytes = static_cast<UINT32>(clip->data.size());
    buf.pAudioData = clip->data.data();
    buf.Flags = XAUDIO2_END_OF_STREAM;
    if (loopCount < 0) loopCount = XAUDIO2_LOOP_INFINITE;
    buf.LoopCount = loopCount;

    slot->clip = clip;
    slot->inUse = true;
    slot->volume = volume;
    slot->lastUseTick = m_Tick++;

    slot->voice->SubmitSourceBuffer(&buf);

    // leave the individual SE volume to voice and the overall volume to seBus
    slot->voice->SetVolume(volume);

    slot->voice->Start(0);
    return true;
}

void SoundCodex::StopAllSE()
{
    for (auto& s : sePool)
    {
        if (s.voice)
        {
            s.voice->Stop(0);
            s.voice->FlushSourceBuffers();
        }
        s.inUse = false;
        s.clip.reset();
    }
}


// ------------------------------------------
// cache / load
std::shared_ptr<SoundCodex::AudioClip> SoundCodex::GetOrLoadClip_(std::string path)
{
    if (path.empty()) return nullptr;

    // leave the key as is (better to add NormalizePath later)
    std::string key(path);

    auto it = clipCache.find(key);
    if (it != clipCache.end())
        return it->second;

    auto clip = LoadWavClip_(path);
    if (!clip) return nullptr;

    clipCache.emplace(key, clip);
    return clip;
}

std::shared_ptr<SoundCodex::AudioClip> SoundCodex::LoadWavClip_(std::string path)
{
    DWORD dwChunkSize = 0;
    DWORD dwChunkPosition = 0;
    DWORD dwFiletype = 0;
    WAVEFORMATEXTENSIBLE wfx{};
    DWORD audioSize = 0;

    std::wstring wpath = ToWideUtf8(path);
    HANDLE hFile = CreateFileW(wpath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        return nullptr;

    if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
    {
        CloseHandle(hFile);
        return nullptr;
    }

    HRESULT hr = S_OK;

    // RIFF
    hr = CheckChunk_(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
    if (FAILED(hr))
    {
        CloseHandle(hFile);
        return nullptr;
    }
    hr = ReadChunkData_(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
    if (FAILED(hr) || dwFiletype != 'EVAW')
    {
        CloseHandle(hFile);
        return nullptr;
    }

    // fmt
    hr = CheckChunk_(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
    if (FAILED(hr))
    {
        CloseHandle(hFile);
        return nullptr;
    }
    hr = ReadChunkData_(hFile, &wfx, dwChunkSize, dwChunkPosition);
    if (FAILED(hr))
    {
        CloseHandle(hFile);
        return nullptr;
    }

    // data
    hr = CheckChunk_(hFile, 'atad', &audioSize, &dwChunkPosition);
    if (FAILED(hr) || audioSize == 0)
    {
        CloseHandle(hFile);
        return nullptr;
    }

    auto clip = std::make_shared<AudioClip>();
    clip->fmt = wfx.Format;
    clip->data.resize(audioSize);

    hr = ReadChunkData_(hFile, clip->data.data(), audioSize, dwChunkPosition);
    CloseHandle(hFile);

    if (FAILED(hr))
        return nullptr;

    return clip;
}

// specify the output bus
IXAudio2SourceVoice* SoundCodex::CreateSourceVoice_(const WAVEFORMATEX& fmt, IXAudio2Voice* dstBus)
{
    if (!XAudio2 || !dstBus) return nullptr;

    IXAudio2SourceVoice* v = nullptr;

    XAUDIO2_SEND_DESCRIPTOR sendDesc{};
    sendDesc.Flags = 0;
    sendDesc.pOutputVoice = dstBus;

    XAUDIO2_VOICE_SENDS sends{};
    sends.SendCount = 1;
    sends.pSends = &sendDesc;

    HRESULT hr = XAudio2->CreateSourceVoice(
        &v,
        &fmt,
        0,
        XAUDIO2_DEFAULT_FREQ_RATIO,
        nullptr,
        &sends,
        nullptr
    );

    if (FAILED(hr) || !v) return nullptr;
    return v;
}

void SoundCodex::DestroySourceVoice_(IXAudio2SourceVoice*& v)
{
    if (!v) return;
    v->Stop(0);
    v->FlushSourceBuffers();
    v->DestroyVoice();
    v = nullptr;
}

// ------------------------------------------
// slot acquire (free, addition, or replacement)
SoundCodex::SeVoiceSlot* SoundCodex::AcquireSeSlot_(const WAVEFORMATEX& fmt)
{
    // 1) prioritize free slots that match fmt
    for (auto& s : sePool)
    {
        if (!s.inUse && s.voice && FormatEquals_(s.fmt, fmt))
            return &s;
    }

    // 2) has empty slot but the fmt is different Å® recreate a slot and use it
    if (sePool.size() < kMaxSeVoices)
    {
        SeVoiceSlot slot{};
        slot.voice = CreateSourceVoice_(fmt, seBus);
        if (!slot.voice) return nullptr;
        slot.fmt = fmt;
        slot.inUse = false;
        slot.lastUseTick = 0;
        sePool.push_back(std::move(slot));
        return &sePool.back();
    }

    // 3) pool full -> replace the oldest slot with SAME fmt first
    SeVoiceSlot* bestSame = nullptr;
    for (auto& s : sePool)
    {
        if (!s.voice) continue;
        if (!FormatEquals_(s.fmt, fmt)) continue;

        if (!bestSame || s.lastUseTick < bestSame->lastUseTick)
            bestSame = &s;
    }

    if (bestSame)
    {
        bestSame->voice->Stop(0);
        bestSame->voice->FlushSourceBuffers();
        bestSame->inUse = false;
        bestSame->clip.reset();
        return bestSame;
    }

    // 4) no same fmt -> replace oldest ANY slot (must recreate voice)
    SeVoiceSlot* bestAny = &sePool[0];
    for (auto& s : sePool)
    {
        if (s.lastUseTick < bestAny->lastUseTick)
            bestAny = &s;
    }

    if (bestAny->voice)
        DestroySourceVoice_(bestAny->voice);

    bestAny->voice = CreateSourceVoice_(fmt, seBus);
    if (!bestAny->voice) return nullptr;

    bestAny->fmt = fmt;
    bestAny->inUse = false;
    bestAny->clip.reset();
    return bestAny;
}

// ------------------------------------------
// format compare
bool SoundCodex::FormatEquals_(const WAVEFORMATEX& a, const WAVEFORMATEX& b)
{
    return a.wFormatTag      == b.wFormatTag &&
           a.nChannels       == b.nChannels &&
           a.nSamplesPerSec  == b.nSamplesPerSec &&
           a.wBitsPerSample  == b.wBitsPerSample &&
           a.nBlockAlign     == b.nBlockAlign &&
           a.nAvgBytesPerSec == b.nAvgBytesPerSec &&
           a.cbSize          == b.cbSize;
}

// ------------------------------------------
// WAV Chunk loading
HRESULT SoundCodex::CheckChunk_(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition)
{
    HRESULT hr = S_OK;
    DWORD dwRead = 0;
    DWORD dwChunkType = 0;
    DWORD dwChunkDataSize = 0;
    DWORD dwRIFFDataSize = 0;
    DWORD dwFileType = 0;
    DWORD dwBytesRead = 0;
    DWORD dwOffset = 0;

    if (SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        return HRESULT_FROM_WIN32(GetLastError());

    while (hr == S_OK)
    {
        if (ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
            hr = HRESULT_FROM_WIN32(GetLastError());

        if (ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0)
            hr = HRESULT_FROM_WIN32(GetLastError());

        switch (dwChunkType)
        {
        case 'FFIR':
            dwRIFFDataSize = dwChunkDataSize;
            dwChunkDataSize = 4;
            if (ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
                hr = HRESULT_FROM_WIN32(GetLastError());
            break;

        default:
            if (SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
                return HRESULT_FROM_WIN32(GetLastError());
            break;
        }

        dwOffset += sizeof(DWORD) * 2;
        if (dwChunkType == format)
        {
            *pChunkSize = dwChunkDataSize;
            *pChunkDataPosition = dwOffset;
            return S_OK;
        }

        dwOffset += dwChunkDataSize;
        dwBytesRead = dwOffset;
        if (dwBytesRead >= dwRIFFDataSize)
            return S_FALSE;
    }

    return hr;
}

HRESULT SoundCodex::ReadChunkData_(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
    DWORD dwRead = 0;

    if (SetFilePointer(hFile, dwBufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        return HRESULT_FROM_WIN32(GetLastError());

    if (ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, NULL) == 0)
        return HRESULT_FROM_WIN32(GetLastError());

    return S_OK;
}