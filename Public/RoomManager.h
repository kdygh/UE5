// 部屋の生成と戦闘進行を管理する。

#pragma once

#include "CoreMinimal.h"


class PJ26_API RoomManager
{
public:
	// 必要な構成要素を生成し、更新・衝突・表示に使用する初期値を設定する。
	RoomManager();
	// 保持している参照やネイティブ資源を整理し、終了時の後処理を行う。
	~RoomManager();
};
