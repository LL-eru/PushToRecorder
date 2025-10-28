#pragma once

//  =-=-= �C���N���[�h�� =-=-=
#include <windows.h>
#include <string>
#include "Recorder.hxx"
#include "Singleton.hxx"

/// @brief WIN32API���g�p����UI�Ǘ��N���X
class UIManager
	:public Singleton<UIManager>{
public:
	enum class State {
		Idle,			// �ҋ@��
		Recording,		// �^����
		Playing			// �Đ���
	};

	/// @brief �f�t�H���g�R���X�g���N�^�֎~�@���L�̃R���X�g���N�^���g�p���邱��
	UIManager() = delete;

	/// @brief ������
	/// @param hInstance �A�v���P�[�V�����C���X�^���X�n���h��
	/// @param recorder �^���E�Đ��Ǘ��I�u�W�F�N�g�ւ̃|�C���^
    UIManager(HINSTANCE hInstance);

	/// @brief ���C�����[�v���s
	/// @param nCmdShow �E�B���h�E�̕\�����@
	/// @return �I���R�[�h
    int Run(int nCmdShow);

private:
	/// @brief �E�B���h�E�v���V�[�W��
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	/// @brief ���x���̓��e���X�V����
    void UpdateLabel();

private:
	HWND m_hWnd;					// ���C���E�B���h�E�n���h��
	HWND m_hLabel;				// ���x���E�B���h�E�n���h��
	State m_oldState = State::Idle;
	Recorder& m_recorder;			// �^���E�Đ��Ǘ��I�u�W�F�N�g�ւ̎Q��
};
