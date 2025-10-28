#pragma once

//  =-=-= インクルード部 =-=-=
#include <portaudio.h>
#include <vector>
#include <chrono>
#include "Singleton.hxx"

/// @brief 録音・再生を管理するクラス
class Recorder
    :public Singleton<Recorder> {
public:
    /// @brief クラスの初期化
    Recorder();
	/// @brief クラスの終了処理
    ~Recorder();

    /// @brief 録音開始
    void StartRecording();
	/// @brief 録音停止
    void StopRecording();
	/// @brief 再生開始
    void StartPlayback();
	/// @brief 再生停止
    void StopPlayback();

    /// @brief オブジェクトが現在録音中かどうか
    /// @return 録音中であれば true、そうでなければ false
    bool IsRecording() const { return m_isRecording; }
    /// @brief 現在再生中かどうかを判定して返す
    /// @return 再生中の場合は true、それ以外（再生していない場合）は false を返します。
    bool IsPlaying();
	/// @brief 録音開始からの経過時間を秒単位で取得する
    double GetRecordDurationSec() const;

private:
	/// @brief PortAudio の録音コールバック関数
    static int RecordCallback(const void* inputBuffer, void*, unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* userData);

	/// @brief PortAudio の再生コールバック関数
    static int PlayCallback(const void*, void* outputBuffer, unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* userData);

private:
	std::vector<short> m_recordedSamples; // 録音データを格納するバッファ
	PaStream* m_stream = nullptr;         // PortAudio のストリームオブジェクト

	bool m_isRecording = false;           // 録音中かどうかのフラグ
	bool m_isPlaying = false;             // 再生中かどうかのフラグ
	std::chrono::steady_clock::time_point m_recordStart;// 録音開始時間
};
