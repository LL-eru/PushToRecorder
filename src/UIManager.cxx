#include "UIManager.hxx"
#include <sstream>

/// @brief コンストラクタ
/// @param hInstance アプリケーションインスタンスハンドル
/// UIManagerの初期化を行い、ウィンドウとラベルを作成する
UIManager::UIManager(HINSTANCE hInstance) 
    : m_recorder(Recorder::GetInstance()) {

    // ウィンドウクラスの設定
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc; // ウィンドウプロシージャ
    wc.hInstance = hInstance;
    wc.lpszClassName = TEXT("VoiceRecorderApp");
    RegisterClass(&wc); // ウィンドウクラスを登録

    // メインウィンドウの作成
    m_hWnd = CreateWindow(wc.lpszClassName, TEXT("Push to recorder"),
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME, // サイズ変更不可
        CW_USEDEFAULT, CW_USEDEFAULT, 320, 150, // ウィンドウサイズ
        nullptr, nullptr, hInstance, nullptr);

    // ラベルウィンドウの作成
    m_hLabel = CreateWindow(TEXT("STATIC"), TEXT("待機中\n[スペース]を押し続けて録音"),
        WS_CHILD | WS_VISIBLE | SS_CENTER, // 中央揃え
        10, 20, 280, 80, m_hWnd, nullptr, hInstance, nullptr);
}

/// @brief メインループを実行する
/// @param nCmdShow ウィンドウの表示方法
/// @return 終了コード
int UIManager::Run(int nCmdShow) {
    ShowWindow(m_hWnd, nCmdShow); // ウィンドウを表示
    SetTimer(m_hWnd, 1, 100, nullptr); // タイマーを設定（100ms間隔）

    MSG msg;
    // メッセージループ
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam; // 終了コードを返す
}

/// @brief ラベルの内容を更新する
/// 録音・再生の状態に応じてラベルのテキストを変更する
void UIManager::UpdateLabel() {
    std::ostringstream oss;
    State currentState;

    if (m_recorder.IsRecording()) {
        currentState = State::Recording;
        oss << "録音中: " << m_recorder.GetRecordDurationSec() << " 秒\n[スペース]を押し続けて録音";
    }
    else if (m_recorder.IsPlaying()) {
        currentState = State::Playing;
        oss << "再生中...\n[スペース]で中断して録音開始";
    }
    else {
        currentState = State::Idle;
        oss << "待機中\n[スペース]を押し続けて録音\n[左Ctrl]で再生開始";
    }

    // 状態が変化した場合、または録音中の場合にラベルを更新
    if (currentState == State::Recording || m_oldState != currentState) {
        SetWindowTextA(m_hLabel, oss.str().c_str());
        m_oldState = currentState;
    }
}

/// @brief ウィンドウプロシージャ
/// @param hWnd ウィンドウハンドル
/// @param msg メッセージ
/// @param wParam メッセージの追加情報
/// @param lParam メッセージの追加情報
/// @return メッセージ処理結果
LRESULT CALLBACK UIManager::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    auto& instance = UIManager::GetInstance(); // UIManagerのインスタンスを取得
    auto& rec = Recorder::GetInstance(); // Recorderのインスタンスを取得

    if (!&instance) return DefWindowProc(hWnd, msg, wParam, lParam);

    switch (msg) {
    case WM_KEYDOWN: // キーが押されたとき
        if (wParam == VK_SPACE) { // スペースキー
            if (rec.IsPlaying()) {
                rec.StopPlayback(); // 再生を停止
                rec.StartRecording(); // 録音を開始
            }
            else if (!rec.IsRecording()) {
                rec.StartRecording(); // 録音を開始
            }
        }
        else if (wParam == VK_CONTROL) { // Ctrlキー
            if (!rec.IsPlaying() && !rec.IsRecording()) {
                rec.StartPlayback(); // 再生を開始
            }
        }
        break;

    case WM_KEYUP: // キーが離されたとき
        if (wParam == VK_SPACE) { // スペースキー
            if (rec.IsRecording()) {
                rec.StopRecording(); // 録音を停止
            }
            rec.StartPlayback(); // 再生を開始
        }
        break;

    case WM_TIMER: // タイマーイベント
        instance.UpdateLabel(); // ラベルを更新
        break;

    case WM_DESTROY: // ウィンドウが破棄されるとき
        rec.StopRecording(); // 録音を停止
        rec.StopPlayback(); // 再生を停止
        PostQuitMessage(0); // アプリケーションを終了
        break;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam); // デフォルトの処理
}
