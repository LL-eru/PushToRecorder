#include "UIManager.hxx"
#include <sstream>

/// @brief �R���X�g���N�^
/// @param hInstance �A�v���P�[�V�����C���X�^���X�n���h��
/// UIManager�̏��������s���A�E�B���h�E�ƃ��x�����쐬����
UIManager::UIManager(HINSTANCE hInstance) 
    : m_recorder(Recorder::GetInstance()) {

    // �E�B���h�E�N���X�̐ݒ�
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc; // �E�B���h�E�v���V�[�W��
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("VoiceRecorderApp");
    RegisterClass(&wc); // �E�B���h�E�N���X��o�^

    // ���C���E�B���h�E�̍쐬
    m_hWnd = CreateWindow(wc.lpszClassName, TEXT("Push to recorder"),
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME, // �T�C�Y�ύX�s��
        CW_USEDEFAULT, CW_USEDEFAULT, 320, 150, // �E�B���h�E�T�C�Y
        nullptr, nullptr, hInstance, nullptr);

    // ���x���E�B���h�E�̍쐬
    m_hLabel = CreateWindow(TEXT("STATIC"), TEXT("�ҋ@��\n[�X�y�[�X]�����������Ę^��"),
        WS_CHILD | WS_VISIBLE | SS_CENTER, // ��������
        10, 20, 280, 80, m_hWnd, nullptr, hInstance, nullptr);
}

/// @brief ���C�����[�v�����s����
/// @param nCmdShow �E�B���h�E�̕\�����@
/// @return �I���R�[�h
int UIManager::Run(int nCmdShow) {
    ShowWindow(m_hWnd, nCmdShow); // �E�B���h�E��\��
    SetTimer(m_hWnd, 1, 100, nullptr); // �^�C�}�[��ݒ�i100ms�Ԋu�j

    MSG msg;
    // ���b�Z�[�W���[�v
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam; // �I���R�[�h��Ԃ�
}

/// @brief ���x���̓��e���X�V����
/// �^���E�Đ��̏�Ԃɉ����ă��x���̃e�L�X�g��ύX����
void UIManager::UpdateLabel() {
    std::ostringstream oss;
    State currentState;

    if (m_recorder.IsRecording()) {
        currentState = State::Recording;
        oss << "�^����: " << m_recorder.GetRecordDurationSec() << " �b\n[�X�y�[�X]�����������Ę^��";
    }
    else if (m_recorder.IsPlaying()) {
        currentState = State::Playing;
        oss << "�Đ���...\n[�X�y�[�X]�Œ��f���Ę^���J�n";
    }
    else {
        currentState = State::Idle;
        oss << "�ҋ@��\n[�X�y�[�X]�����������Ę^��\n[��Ctrl]�ōĐ��J�n";
    }

    // ��Ԃ��ω������ꍇ�A�܂��͘^�����̏ꍇ�Ƀ��x�����X�V
    if (currentState == State::Recording || m_oldState != currentState) {
        SetWindowTextA(m_hLabel, oss.str().c_str());
        m_oldState = currentState;
    }
}

/// @brief �E�B���h�E�v���V�[�W��
/// @param hWnd �E�B���h�E�n���h��
/// @param msg ���b�Z�[�W
/// @param wParam ���b�Z�[�W�̒ǉ����
/// @param lParam ���b�Z�[�W�̒ǉ����
/// @return ���b�Z�[�W��������
LRESULT CALLBACK UIManager::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto& instance = UIManager::GetInstance(); // UIManager�̃C���X�^���X���擾
    auto& rec = Recorder::GetInstance(); // Recorder�̃C���X�^���X���擾

    if (!&instance) return DefWindowProc(hWnd, msg, wParam, lParam);

    switch (msg) {
    case WM_KEYDOWN: // �L�[�������ꂽ�Ƃ�
        if (wParam == VK_SPACE) { // �X�y�[�X�L�[
            if (rec.IsPlaying()) {
                rec.StopPlayback(); // �Đ����~
                rec.StartRecording(); // �^�����J�n
            }
            else if (!rec.IsRecording()) {
                rec.StartRecording(); // �^�����J�n
            }
        }
        else if (wParam == VK_CONTROL) { // Ctrl�L�[
            if (!rec.IsPlaying() && !rec.IsRecording()) {
                rec.StartPlayback(); // �Đ����J�n
            }
        }
        break;

    case WM_KEYUP: // �L�[�������ꂽ�Ƃ�
        if (wParam == VK_SPACE) { // �X�y�[�X�L�[
            if (rec.IsRecording()) {
                rec.StopRecording(); // �^�����~
            }
            rec.StartPlayback(); // �Đ����J�n
        }
        break;

    case WM_TIMER: // �^�C�}�[�C�x���g
        instance.UpdateLabel(); // ���x�����X�V
        break;

    case WM_DESTROY: // �E�B���h�E���j�������Ƃ�
        rec.StopRecording(); // �^�����~
        rec.StopPlayback(); // �Đ����~
        PostQuitMessage(0); // �A�v���P�[�V�������I��
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam); // �f�t�H���g�̏���
}
