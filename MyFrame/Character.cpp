#include <math.h>
#include "Character.h"
#include "ObjectCodex.h"
#include "Collision.h"

#include "Block.h"
//#include "BlockTiled.h"

#include "Player.h"
#include "EnemyTest.h"
#include "EnemyTestTwo.h"
#include "EnemyTestThree.h"

void Character::MapItemCollide(void)
{
	// 上下左右の当たり方向を検出するため、oldを用いたチェックを行う
	auto Position = GetPosition();
	auto CollHalf = GetCollisionSize();
	float OwnerTop =	Position.y + CollHalf.y; // プレイヤーの上端
	float OwnerBottom =	Position.y - CollHalf.y; // プレイヤーの下端
	float OwnerRight =	Position.x + CollHalf.x; // プレイヤーの右端
	float OwnerLeft =	Position.x - CollHalf.x; // プレイヤーの左端
	float OwnerBack =	Position.z + CollHalf.z; // プレイヤーの後端
	float OwnerFront =	Position.z - CollHalf.z; // プレイヤーの前端
	float OwnerOldTop =		PositionOld.y + CollHalf.y;	// プレイヤーの古い位置の上端
	float OwnerOldBottom =	PositionOld.y - CollHalf.y;	// プレイヤーの古い位置の下端
	float OwnerOldRight =	PositionOld.x + CollHalf.x;	// プレイヤーの古い位置の右端
	float OwnerOldLeft =	PositionOld.x - CollHalf.x;	// プレイヤーの古い位置の左端
	float OwnerOldBack =	PositionOld.z + CollHalf.z;	// プレイヤーの古い位置の後端
	float OwnerOldFront =	PositionOld.z - CollHalf.z;	// プレイヤーの古い位置の前端
	float ItemTop, ItemBottom, ItemRight, ItemLeft, ItemBack, ItemFront;

/*------------------------------------------------------------------------------
   MapItem
------------------------------------------------------------------------------*/
	OnFloor = false; //先ずは床にいないを想定する

	// 全てのマップオブジェクトを探す
	std::vector<Block*> block =				ObjectCodex::FindActiveObjectsByTag<Block>(environment_Block);
	//std::vector<BlockTiled*> blockTiled =	ObjectCodex::FindActiveObjectsByTag<BlockTiled>(environment_BlockTiled);

	// 全てのマップオブジェクトをマップオブジェクトをして整理する
	std::vector<ObjectBase*> mapItems;
	mapItems.reserve(block.size());
	//mapItems.reserve(block.size() + blockTiled.size());
	mapItems.insert(mapItems.end(),	block.begin(),		block.end());
	//mapItems.insert(mapItems.end(),	blockTiled.begin(),	blockTiled.end());

	// ループでコリジョン判断
	for (auto b : mapItems)
	{
		if (this == b) continue;

		 bool isCollide = CollisionSystem::IsOverlap(this->boxCollider, b->GetBoxCollider());
		
		if (isCollide)
		{
			auto ItemPosition = b->GetPosition();
			auto ItemCollHalf = b->GetCollisionSize();
			ItemTop =	 ItemPosition.y + ItemCollHalf.y;	// ブロックの上端
			ItemBottom = ItemPosition.y - ItemCollHalf.y;	// ブロックの下端
			ItemRight =	 ItemPosition.x + ItemCollHalf.x;	// ブロックの右端
			ItemLeft =	 ItemPosition.x - ItemCollHalf.x;	// ブロックの左端
			ItemBack =	 ItemPosition.z + ItemCollHalf.z;	// ブロックの後端
			ItemFront =	 ItemPosition.z - ItemCollHalf.z;	// ブロックの前端

			// 上から下に当たった(乗った)
			if (OwnerOldBottom >= ItemTop && OwnerBottom <= ItemTop)
			{
				// 下とブロックの上を比べ、下がoldの時は上側にあり、現在は下側にある場合
				// 着地
				MoveVelocity.y = 0.0f;
				// プレイヤーの場所を固定(地面に引っかからないように)
				transInfo.position.y = ItemTop + CollHalf.y + 0.001f;
				// 地面に乗っている
				OnFloor = true;
				continue;
			}
			// 下から上に当たった
			if (OwnerOldTop <= ItemBottom && OwnerTop >= ItemBottom)
			{
				// 上とブロックの下を比べ、上がoldの時は下側にあり、現在は上側にある場合
				// 止める
				MoveVelocity.y = 0.0f;
				// プレイヤーの場所を固定
				transInfo.position.y = ItemBottom - CollHalf.y - 0.1f;
				continue;
			}
			// 左から右に当たった
			if (OwnerOldRight <= ItemLeft && OwnerRight >= ItemLeft)
			{
				transInfo.position.x = ItemLeft - CollHalf.x - 0.1f; // 場所を固定
				continue;
			}
			// 右から左に当たった
			if (OwnerOldLeft >= ItemRight && OwnerLeft <= ItemRight)
			{
				transInfo.position.x = ItemRight + CollHalf.x + 0.1f; // 場所を固定
				continue;
			}
			// 後から前に当たった
			if (OwnerOldBack <= ItemFront && OwnerBack >= ItemFront)
			{
				transInfo.position.z = ItemFront - CollHalf.z - 0.1f; // 場所を固定
				continue;
			}
			// 前から後に当たった
			if (OwnerOldFront >= ItemBack && OwnerFront <= ItemBack)
			{
				transInfo.position.z = ItemBack + CollHalf.z + 0.1f; // 場所を固定
				continue;
			}
		}
	}


/*------------------------------------------------------------------------------
   MapCharacter
------------------------------------------------------------------------------*/
	// 全てのキャラクターを探す
	std::vector<Player*> player = ObjectCodex::FindActiveObjectsByTag<Player>(character_Player);
	std::vector<EnemyTest*> enemyTest = ObjectCodex::FindActiveObjectsByTag<EnemyTest>(character_Enemy_Test);
	std::vector<EnemyTestTwo*> enemyTestTwo = ObjectCodex::FindActiveObjectsByTag<EnemyTestTwo>(character_Enemy_Test_Two);
	std::vector<EnemyTestThree*> enemyTestThree = ObjectCodex::FindActiveObjectsByTag<EnemyTestThree>(character_Enemy_Test_Three);

	// 全てのキャラクターを整理する
	std::vector<Character*> mapCharacters;
	mapCharacters.reserve(player.size() + enemyTest.size() + enemyTestTwo.size() + enemyTestThree.size());
	mapCharacters.insert(mapCharacters.end(), player.begin(), player.end());
	mapCharacters.insert(mapCharacters.end(), enemyTest.begin(), enemyTest.end());
	mapCharacters.insert(mapCharacters.end(), enemyTestTwo.begin(), enemyTestTwo.end());
	mapCharacters.insert(mapCharacters.end(), enemyTestThree.begin(), enemyTestThree.end());

	// ループでコリジョン判断
	for (auto c : mapCharacters)
	{
		if (this == c || this->IsDeath || c->IsDeath || !this->OnCollision || !c->OnCollision) continue;

		bool isCollide = CollisionSystem::IsOverlap(this->boxCollider, c->GetBoxCollider());

		if (isCollide)
		{
			auto ItemPosition = c->GetPosition();
			auto ItemCollHalf = c->GetCollisionSize();
			ItemTop =	 ItemPosition.y + ItemCollHalf.y;	// キャラクターの上端
			ItemBottom = ItemPosition.y - ItemCollHalf.y;	// キャラクターの下端
			ItemRight =	 ItemPosition.x + ItemCollHalf.x;	// キャラクターの右端
			ItemLeft =	 ItemPosition.x - ItemCollHalf.x;	// キャラクターの左端
			ItemBack =	 ItemPosition.z + ItemCollHalf.z;	// キャラクターの後端
			ItemFront =	 ItemPosition.z - ItemCollHalf.z;	// キャラクターの前端

			// 上から下に当たった(乗った)
			if (OwnerOldBottom >= ItemTop && OwnerBottom <= ItemTop)
			{
				// 下とブロックの上を比べ、下がoldの時は上側にあり、現在は下側にある場合
				// 着地
				MoveVelocity.y = 0.0f;
				// 場所を固定(地面に引っかからないように)

				transInfo.position.y = ItemTop + CollHalf.y + 0.001f;

				float UpOrDown = (c->GetPosition().z - PositionOld.z > 0.0f) ? 1.0f : -1.0f;
				float RightOrLeft = (c->GetPosition().x - PositionOld.x > 0.0f) ? 1.0f : -1.0f;
				c->CalculateMoveVelocity(MoveAccel * 0.1f * RightOrLeft, 0.0f, MoveAccel * 0.1f * UpOrDown);

				// 地面に乗っている
				OnFloor = true;
				continue;
			}
			// 下から上に当たった
			if (OwnerOldTop <= ItemBottom && OwnerTop >= ItemBottom)
			{
				// 上とブロックの下を比べ、上がoldの時は下側にあり、現在は上側にある場合
				// 止める
				MoveVelocity.y = 0.0f;
				// プレイヤーの場所を固定
				transInfo.position.y = ItemBottom - CollHalf.y - 0.1f;
				continue;
			}
			// 左から右に当たった
			if (OwnerOldRight <= ItemLeft && OwnerRight >= ItemLeft)
			{
				transInfo.position.x = ItemLeft - CollHalf.x - 0.1f; // 場所を固定

				float UpOrDown = (c->GetPosition().z - PositionOld.z > 0.0f) ? 1.0f : -1.0f;
				c->CalculateMoveVelocity(MoveAccel * 0.2f, 0.0f, MoveAccel * 0.1f * UpOrDown);

				continue;
			}
			// 右から左に当たった
			if (OwnerOldLeft >= ItemRight && OwnerLeft <= ItemRight)
			{
				transInfo.position.x = ItemRight + CollHalf.x + 0.1f; // 場所を固定

				float UpOrDown = (c->GetPosition().z - PositionOld.z > 0.0f) ? 1.0f : -1.0f;
				c->CalculateMoveVelocity(-MoveAccel * 0.2f, 0.0f, MoveAccel * 0.1f * UpOrDown);

				continue;
			}
			// 後から前に当たった
			if (OwnerOldBack <= ItemFront && OwnerBack >= ItemFront)
			{
				transInfo.position.z = ItemFront - CollHalf.z - 0.1f; // 場所を固定

				float RightOrLeft = (c->GetPosition().x - PositionOld.x > 0.0f) ? 1.0f : -1.0f;
				c->CalculateMoveVelocity(MoveAccel * 0.1f * RightOrLeft, 0.0f, MoveAccel * 0.2f);

				continue;
			}
			// 前から後に当たった
			if (OwnerOldFront >= ItemBack && OwnerFront <= ItemBack)
			{
				transInfo.position.z = ItemBack + CollHalf.z + 0.1f; // 場所を固定

				float RightOrLeft = (c->GetPosition().x - PositionOld.x > 0.0f) ? 1.0f : -1.0f;
				c->CalculateMoveVelocity(MoveAccel * 0.1f * RightOrLeft, 0.0f, -MoveAccel * 0.2f);

				continue;
			}
		}
	}
}