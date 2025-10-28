#include "Supervision.hxx"
#include "Recorder.hxx"
#include "UIManager.hxx"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // ������
    Recorder& recorder = Recorder::CreateInstance();
    UIManager& ui = UIManager::CreateInstance(hInstance);

    // ���C�����[�v
    auto result = ui.Run(nCmdShow);

	// �I������
	Supervision::Finalize();
	return result;
}
