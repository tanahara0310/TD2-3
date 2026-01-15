#pragma once

class Menu {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// メニューが開いているかどうか
	/// </summary>
	/// <returns>開いているならtrue</returns>
	bool IsOpened() const { return isOpened_; }

private:
	bool isOpened_ = false;
};
