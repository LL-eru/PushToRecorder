#pragma once

//  =-=-= �C���N���[�h�� =-=-=
#include <portaudio.h>
#include <vector>
#include <chrono>
#include "Singleton.hxx"

/// @brief �^���E�Đ����Ǘ�����N���X
class Recorder
    :public Singleton<Recorder> {
public:
    /// @brief �N���X�̏�����
    Recorder();
	/// @brief �N���X�̏I������
    ~Recorder();

    /// @brief �^���J�n
    void StartRecording();
	/// @brief �^����~
    void StopRecording();
	/// @brief �Đ��J�n
    void StartPlayback();
	/// @brief �Đ���~
    void StopPlayback();

    /// @brief �I�u�W�F�N�g�����ݘ^�������ǂ���
    /// @return �^�����ł���� true�A�����łȂ���� false
    bool IsRecording() const { return m_isRecording; }
    /// @brief ���ݍĐ������ǂ����𔻒肵�ĕԂ�
    /// @return �Đ����̏ꍇ�� true�A����ȊO�i�Đ����Ă��Ȃ��ꍇ�j�� false ��Ԃ��܂��B
    bool IsPlaying();
	/// @brief �^���J�n����̌o�ߎ��Ԃ�b�P�ʂŎ擾����
    double GetRecordDurationSec() const;

private:
	/// @brief PortAudio �̘^���R�[���o�b�N�֐�
    static int RecordCallback(const void* inputBuffer, void*, unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* userData);

	/// @brief PortAudio �̍Đ��R�[���o�b�N�֐�
    static int PlayCallback(const void*, void* outputBuffer, unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* userData);

private:
	std::vector<short> m_recordedSamples; // �^���f�[�^���i�[����o�b�t�@
	PaStream* m_stream = nullptr;         // PortAudio �̃X�g���[���I�u�W�F�N�g

	bool m_isRecording = false;           // �^�������ǂ����̃t���O
	bool m_isPlaying = false;             // �Đ������ǂ����̃t���O
	std::chrono::steady_clock::time_point m_recordStart;// �^���J�n����
};
