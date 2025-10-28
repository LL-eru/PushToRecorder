#include "Recorder.hxx"
#include <cstring>
#include <algorithm>

// �萔��`
#define SAMPLE_RATE (44100)          // �T���v�����O���[�g�i44.1kHz�j
#define FRAMES_PER_BUFFER (512)     // �o�b�t�@������̃t���[����
#define NUM_CHANNELS (1)            // �`�����l�����i���m�����j
#define SAMPLE_FORMAT paInt16       // �T���v���t�H�[�}�b�g�i16�r�b�g�����j

/// @brief �R���X�g���N�^
/// PortAudio�̏��������s��
Recorder::Recorder() {
    Pa_Initialize();
}

/// @brief �f�X�g���N�^
/// �^���E�Đ����~���APortAudio���I������
Recorder::~Recorder() {
    StopRecording();
    StopPlayback();
    Pa_Terminate();
}

/// @brief PortAudio�̘^���R�[���o�b�N�֐�
/// @param inputBuffer ���̓o�b�t�@
/// @param framesPerBuffer �o�b�t�@���̃t���[����
/// @param userData ���[�U�[�f�[�^�iRecorder�C���X�^���X�j
/// @return �R�[���o�b�N�̌p�����
int Recorder::RecordCallback(const void* inputBuffer, void*, unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* userData) {
    Recorder* self = reinterpret_cast<Recorder*>(userData);
    if (inputBuffer && self->m_isRecording) {
        // ���̓o�b�t�@�̃f�[�^��^���f�[�^�ɒǉ�
        const short* in = static_cast<const short*>(inputBuffer);
        self->m_recordedSamples.insert(self->m_recordedSamples.end(), in, in + framesPerBuffer);
    }
    return paContinue; // �^�����p��
}

/// @brief �Đ������ǂ������m�F����
/// @return �Đ����Ȃ�true�A�����łȂ����false
bool Recorder::IsPlaying() {
    if (m_isPlaying && m_stream && Pa_IsStreamActive(m_stream) == 0)
        m_isPlaying = false; // �Đ����I�����Ă���ꍇ�̓t���O���X�V
    return m_isPlaying;
}

/// @brief PortAudio�̍Đ��R�[���o�b�N�֐�
/// @param outputBuffer �o�̓o�b�t�@
/// @param framesPerBuffer �o�b�t�@���̃t���[����
/// @param userData ���[�U�[�f�[�^�iRecorder�C���X�^���X�ƍĐ��ʒu�j
/// @return �R�[���o�b�N�̌p�����
int Recorder::PlayCallback(const void*, void* outputBuffer, unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* userData) {
    auto* data = reinterpret_cast<std::pair<Recorder*, size_t*>*>(userData);
    Recorder* self = data->first;
    size_t* pos = data->second;

    short* out = static_cast<short*>(outputBuffer);
    size_t remaining = self->m_recordedSamples.size() - *pos; // �c��̃T���v����
    size_t framesToCopy = std::min<size_t>(framesPerBuffer, remaining);

    if (framesToCopy > 0) {
        // �Đ��f�[�^���o�̓o�b�t�@�ɃR�s�[
        memcpy(out, &self->m_recordedSamples[*pos], framesToCopy * sizeof(short));
        *pos += framesToCopy;
    }

    if (framesToCopy < framesPerBuffer) {
        // �Đ��f�[�^������Ȃ��ꍇ�̓[������
        memset(out + framesToCopy, 0, (framesPerBuffer - framesToCopy) * sizeof(short));
        delete pos; // �Đ��ʒu�̃����������
        delete data; // ���[�U�[�f�[�^�����
        return paComplete; // �Đ����I��
    }
    return paContinue; // �Đ����p��
}

/// @brief �^�����J�n����
void Recorder::StartRecording() {
    m_recordedSamples.clear(); // �^���f�[�^���N���A
    m_isRecording = true; // �^���t���O��ݒ�
    m_recordStart = std::chrono::steady_clock::now(); // �^���J�n�������L�^

    // ���̓X�g���[���̐ݒ�
    PaStreamParameters inputParams;
    inputParams.device = Pa_GetDefaultInputDevice();
    inputParams.channelCount = NUM_CHANNELS;
    inputParams.sampleFormat = SAMPLE_FORMAT;
    inputParams.suggestedLatency =
        Pa_GetDeviceInfo(inputParams.device)->defaultLowInputLatency;
    inputParams.hostApiSpecificStreamInfo = nullptr;

    // �X�g���[�����J���Ę^�����J�n
    Pa_OpenStream(&m_stream, &inputParams, nullptr, SAMPLE_RATE, FRAMES_PER_BUFFER,
        paClipOff, RecordCallback, this);
    Pa_StartStream(m_stream);
}

/// @brief �^�����~����
void Recorder::StopRecording() {
    if (!m_isRecording) return; // �^�����łȂ���Ή������Ȃ�
    m_isRecording = false; // �^���t���O������
    if (m_stream) {
        Pa_StopStream(m_stream); // �X�g���[�����~
        Pa_CloseStream(m_stream); // �X�g���[�������
        m_stream = nullptr;
    }
}

/// @brief �Đ����J�n����
void Recorder::StartPlayback() {
    if (m_recordedSamples.empty()) return; // �^���f�[�^����Ȃ牽�����Ȃ�
    m_isPlaying = true; // �Đ��t���O��ݒ�

    size_t* playPos = new size_t(0); // �Đ��ʒu��������
    auto* userData = new std::pair<Recorder*, size_t*>(this, playPos);

    // �o�̓X�g���[���̐ݒ�
    PaStreamParameters outputParams;
    outputParams.device = Pa_GetDefaultOutputDevice();
    outputParams.channelCount = NUM_CHANNELS;
    outputParams.sampleFormat = SAMPLE_FORMAT;
    outputParams.suggestedLatency =
        Pa_GetDeviceInfo(outputParams.device)->defaultLowOutputLatency;
    outputParams.hostApiSpecificStreamInfo = nullptr;

    // �X�g���[�����J���čĐ����J�n
    Pa_OpenStream(&m_stream, nullptr, &outputParams, SAMPLE_RATE, FRAMES_PER_BUFFER,
        paClipOff, PlayCallback, userData);
    Pa_StartStream(m_stream);
}

/// @brief �Đ����~����
void Recorder::StopPlayback() {
    if (!m_isPlaying) return; // �Đ����łȂ���Ή������Ȃ�
    m_isPlaying = false; // �Đ��t���O������
    if (m_stream) {
        Pa_StopStream(m_stream); // �X�g���[�����~
        Pa_CloseStream(m_stream); // �X�g���[�������
        m_stream = nullptr;
    }
}

/// @brief �^�����Ԃ�b�P�ʂŎ擾����
/// @return �^�����ԁi�b�j
double Recorder::GetRecordDurationSec() const {
    if (!m_isRecording) return 0.0; // �^�����łȂ����0��Ԃ�
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - m_recordStart).count() / 1000.0;
}