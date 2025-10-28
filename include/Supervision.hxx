#pragma once

// =-=-= インクルード部 =-=-=
#include <stack>
#include <mutex>

/// @brief シングルトンの最終処理を行うクラス
class Supervision final
{
	template<typename Type> friend class Singleton;
	using func = void(*)();
public:
	/// @brief 終了処理を行う
	static void Finalize();

	/// @brief 終了処理を追加する
	/// @param func 終了処理
	static void _addFinalizer(func);
private:
	static std::stack<func> m_finalizers;//終了処理
	static std::mutex gMutex;// 排他制御用
};
