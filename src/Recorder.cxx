#include "Recorder.hxx"
#include <cstring>
#include <algorithm>

// 定数定義
#define SAMPLE_RATE (44100)          // サンプリングレート（44.1kHz）
#define FRAMES_PER_BUFFER (512)     // バッファあたりのフレーム数
#define NUM_CHANNELS (1)            // チャンネル数（モノラル）
#define SAMPLE_FORMAT paInt16       // サンプルフォーマット（16ビット整数）

/// @brief コンストラクタ
/// PortAudioの初期化を行う
Recorder::Recorder() {
    Pa_Initialize();
}

/// @brief デストラクタ
/// 録音・再生を停止し、PortAudioを終了する
Recorder::~Recorder() {
    StopRecording();
    StopPlayback();
    Pa_Terminate();
}

/// @brief PortAudioの録音コールバック関数
/// @param inputBuffer 入力バッファ
/// @param framesPerBuffer バッファ内のフレーム数
/// @param userData ユーザーデータ（Recorderインスタンス）
/// @return コールバックの継続状態
int Recorder::RecordCallback(const void* inputBuffer, void*, unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* userData) {
    Recorder* self = reinterpret_cast<Recorder*>(userData);
    if (inputBuffer && self->m_isRecording) {
        // 入力バッファのデータを録音データに追加
        const short* in = static_cast<const short*>(inputBuffer);
        self->m_recordedSamples.insert(self->m_recordedSamples.end(), in, in + framesPerBuffer);
    }
    return paContinue; // 録音を継続
}

/// @brief 再生中かどうかを確認する
/// @return 再生中ならtrue、そうでなければfalse
bool Recorder::IsPlaying() {
    if (m_isPlaying && m_stream && Pa_IsStreamActive(m_stream) == 0)
        m_isPlaying = false; // 再生が終了している場合はフラグを更新
    return m_isPlaying;
}

/// @brief PortAudioの再生コールバック関数
/// @param outputBuffer 出力バッファ
/// @param framesPerBuffer バッファ内のフレーム数
/// @param userData ユーザーデータ（Recorderインスタンスと再生位置）
/// @return コールバックの継続状態
int Recorder::PlayCallback(const void*, void* outputBuffer, unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* userData) {
    auto* data = reinterpret_cast<std::pair<Recorder*, size_t*>*>(userData);
    Recorder* self = data->first;
    size_t* pos = data->second;

    short* out = static_cast<short*>(outputBuffer);
    size_t remaining = self->m_recordedSamples.size() - *pos; // 残りのサンプル数
    size_t framesToCopy = std::min<size_t>(framesPerBuffer, remaining);

    if (framesToCopy > 0) {
        // 再生データを出力バッファにコピー
        memcpy(out, &self->m_recordedSamples[*pos], framesToCopy * sizeof(short));
        *pos += framesToCopy;
    }

    if (framesToCopy < framesPerBuffer) {
        // 再生データが足りない場合はゼロ埋め
        memset(out + framesToCopy, 0, (framesPerBuffer - framesToCopy) * sizeof(short));
        delete pos; // 再生位置のメモリを解放
        delete data; // ユーザーデータを解放
        return paComplete; // 再生を終了
    }
    return paContinue; // 再生を継続
}

/// @brief 録音を開始する
void Recorder::StartRecording() {
    m_recordedSamples.clear(); // 録音データをクリア
    m_isRecording = true; // 録音フラグを設定
    m_recordStart = std::chrono::steady_clock::now(); // 録音開始時刻を記録

    // 入力ストリームの設定
    PaStreamParameters inputParams;
    inputParams.device = Pa_GetDefaultInputDevice();
    inputParams.channelCount = NUM_CHANNELS;
    inputParams.sampleFormat = SAMPLE_FORMAT;
    inputParams.suggestedLatency =
        Pa_GetDeviceInfo(inputParams.device)->defaultLowInputLatency;
    inputParams.hostApiSpecificStreamInfo = nullptr;

    // ストリームを開いて録音を開始
    Pa_OpenStream(&m_stream, &inputParams, nullptr, SAMPLE_RATE, FRAMES_PER_BUFFER,
        paClipOff, RecordCallback, this);
    Pa_StartStream(m_stream);
}

/// @brief 録音を停止する
void Recorder::StopRecording() {
    if (!m_isRecording) return; // 録音中でなければ何もしない
    m_isRecording = false; // 録音フラグを解除
    if (m_stream) {
        Pa_StopStream(m_stream); // ストリームを停止
        Pa_CloseStream(m_stream); // ストリームを閉じる
        m_stream = nullptr;
    }
}

/// @brief 再生を開始する
void Recorder::StartPlayback() {
    if (m_recordedSamples.empty()) return; // 録音データが空なら何もしない
    m_isPlaying = true; // 再生フラグを設定

    size_t* playPos = new size_t(0); // 再生位置を初期化
    auto* userData = new std::pair<Recorder*, size_t*>(this, playPos);

    // 出力ストリームの設定
    PaStreamParameters outputParams;
    outputParams.device = Pa_GetDefaultOutputDevice();
    outputParams.channelCount = NUM_CHANNELS;
    outputParams.sampleFormat = SAMPLE_FORMAT;
    outputParams.suggestedLatency =
        Pa_GetDeviceInfo(outputParams.device)->defaultLowOutputLatency;
    outputParams.hostApiSpecificStreamInfo = nullptr;

    // ストリームを開いて再生を開始
    Pa_OpenStream(&m_stream, nullptr, &outputParams, SAMPLE_RATE, FRAMES_PER_BUFFER,
        paClipOff, PlayCallback, userData);
    Pa_StartStream(m_stream);
}

/// @brief 再生を停止する
void Recorder::StopPlayback() {
    if (!m_isPlaying) return; // 再生中でなければ何もしない
    m_isPlaying = false; // 再生フラグを解除
    if (m_stream) {
        Pa_StopStream(m_stream); // ストリームを停止
        Pa_CloseStream(m_stream); // ストリームを閉じる
        m_stream = nullptr;
    }
}

/// @brief 録音時間を秒単位で取得する
/// @return 録音時間（秒）
double Recorder::GetRecordDurationSec() const {
    if (!m_isRecording) return 0.0; // 録音中でなければ0を返す
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - m_recordStart).count() / 1000.0;
}