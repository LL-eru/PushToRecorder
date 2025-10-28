#pragma once

// =-=-= インクルード部 =-=-=
#include "Supervision.hxx"

/// @brief シングルトンのインスタンスを生成・保持するクラス
template<typename Type>
class Singleton
{
public:
	/// @brief インスタンスを生成する
	/// @tparam ...Args&& 可変長テンプレート引数(値渡し) 
	/// @param ...args コンストラクタの引数
	/// @return インスタンス
	template<typename... Args>
	static inline Type& CreateInstance(Args&&... args)
	{
		// 初めて呼び出されたならインスタンスの生成
		std::call_once(initFlag, [](Args&&... args) {
			Create(std::forward<Args>(args)...);
		}, std::forward<Args>(args)...);
		return *instance;
	}

	/// @brief 既に生成されているインスタンスを取得する
	/// @return インスタンス
	static inline Type& GetInstance()
	{
#ifdef _DEBUG
		if (!instance)
			DebugBreak();
#endif // _DEBUG
		return *instance;
	}

private:
	/// @brief インスタンスの生成関数
	template<typename... Args>
	static inline void Create(Args&&... args)
	{
		instance = new Type(args...);
		Supervision::_addFinalizer(&Singleton<Type>::destroy);
	}

	/// @brief インスタンスを破棄する
	static inline void destroy()
	{
		delete instance;
		instance = nullptr;
	}

	static std::once_flag initFlag;	//作ったかのフラグ(排他制御)
	static Type* instance;			//自らのインスタンス

protected:
	/// @brief コンストラクタ
	Singleton() {}
	/// @brief デストラクタ
	virtual ~Singleton() = default;

	// コピー・代入禁止
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;
};

// 静的メンバを定義
template <typename Type> std::once_flag Singleton<Type>::initFlag;
template <typename Type> Type* Singleton<Type>::instance = nullptr;
