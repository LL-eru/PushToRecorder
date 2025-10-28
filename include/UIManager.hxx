#pragma once

//  =-=-= インクルード部 =-=-=
#include <windows.h>
#include <string>
#include "Recorder.hxx"
#include "Singleton.hxx"

/// @brief WIN32APIを使用したUI管理クラス
class UIManager
	:public Singleton<UIManager>{
public:
	enum class State {
		Idle,			// 待機中
		Recording,		// 録音中
		Playing			// 再生中
	};

	/// @brief デフォルトコンストラクタ禁止　下記のコンストラクタを使用すること
	UIManager() = delete;

	/// @brief 初期化
	/// @param hInstance アプリケーションインスタンスハンドル
	/// @param recorder 録音・再生管理オブジェクトへのポインタ
    UIManager(HINSTANCE hInstance);

	/// @brief メインループ実行
	/// @param nCmdShow ウィンドウの表示方法
	/// @return 終了コード
    int Run(int nCmdShow);

private:
	/// @brief ウィンドウプロシージャ
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	/// @brief ラベルの内容を更新する
    void UpdateLabel();

private:
	HWND m_hWnd;					// メインウィンドウハンドル
	HWND m_hLabel;				// ラベルウィンドウハンドル
	State m_oldState = State::Idle;
	Recorder& m_recorder;			// 録音・再生管理オブジェクトへの参照
};
