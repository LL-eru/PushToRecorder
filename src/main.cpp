#include "Supervision.hxx"
#include "Recorder.hxx"
#include "UIManager.hxx"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // 初期化
    Recorder& recorder = Recorder::CreateInstance();
    UIManager& ui = UIManager::CreateInstance(hInstance);

    // メインループ
    auto result = ui.Run(nCmdShow);

	// 終了処理
	Supervision::Finalize();
	return result;
}
