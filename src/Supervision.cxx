#include "Supervision.hxx"

// =-=-= インスタンス変数の実体化 =-=-=
std::stack<Supervision::func> Supervision::m_finalizers; // 終了処理
std::mutex Supervision::gMutex; // 排他制御用

/// @brief 終了処理を保存
/// @param fin 終了処理
void Supervision::_addFinalizer(func fin)
{
    std::lock_guard<std::mutex> lock(gMutex); // 排他制御
    m_finalizers.push(fin); // 終了処理を追加
}

/// @brief 終了処理を行う
void Supervision::Finalize()
{
    std::lock_guard<std::mutex> lock(gMutex); // 排他制御

    while (!m_finalizers.empty()) { // 終了処理を逆順に実行
        m_finalizers.top()();
        m_finalizers.pop();
    }
}
