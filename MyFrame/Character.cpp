#include <math.h>
#include "Character.h"
#include "ObjectCodex.h"

//#include "Block.h"
//#include "BlockTiled.h"
//#include "Player.h"
//#include "EnemyTest.h"
//#include "EnemyTestTwo.h"
//#include "EnemyTestThree.h"

void Character::MapItemCollide(void)
{
//	auto Position = GetPosition();
//
//	// 上下左右の当たり方向を検出するため、oldを用いたチェックを行う
//	float OwnerTop =		Position.y + CollisionSize.y / 2;	// プレイヤーの上端
//	float OwnerBottom =		Position.y - CollisionSize.y / 2;	// プレイヤーの下端
//	float OwnerRight =		Position.x + CollisionSize.x / 2;	// プレイヤーの右端
//	float OwnerLeft =		Position.x - CollisionSize.x / 2;	// プレイヤーの左端
//	float OwnerBack =		Position.z + CollisionSize.z / 2;	// プレイヤーの後端
//	float OwnerFront =		Position.z - CollisionSize.z / 2;	// プレイヤーの前端
//	float OwnerOldTop =		PositionOld.y + CollisionSize.y / 2;	// プレイヤーの古い位置の上端
//	float OwnerOldBottom =	PositionOld.y - CollisionSize.y / 2;	// プレイヤーの古い位置の下端
//	float OwnerOldRight =	PositionOld.x + CollisionSize.x / 2;	// プレイヤーの古い位置の右端
//	float OwnerOldLeft =	PositionOld.x - CollisionSize.x / 2;	// プレイヤーの古い位置の左端
//	float OwnerOldBack =	PositionOld.z + CollisionSize.z / 2;	// プレイヤーの古い位置の後端
//	float OwnerOldFront =	PositionOld.z - CollisionSize.z / 2;	// プレイヤーの古い位置の前端
//	float ItemTop, ItemBottom, ItemRight, ItemLeft, ItemFront, ItemBack;
//
//
///*------------------------------------------------------------------------------
//   MapItem
//------------------------------------------------------------------------------*/
//	OnFloor = false; //先ずは床にいないを想定する
//
//	// 全てのマップオブジェクトを探す
//	std::vector<Block*> block =				ObjectCodex::FindActiveObjectsByTag<Block>(environment_Block);
//	std::vector<BlockTiled*> blockTiled =	ObjectCodex::FindActiveObjectsByTag<BlockTiled>(environment_BlockTiled);
//
//	// 全てのマップオブジェクトをマップオブジェクトをして整理する
//	std::vector<ObjectBase*> mapItems;
//	mapItems.reserve(block.size() + blockTiled.size());
//	mapItems.insert(mapItems.end(),	block.begin(),		block.end());
//	mapItems.insert(mapItems.end(),	blockTiled.begin(),	blockTiled.end());
//
//	// ループでコリジョン判断
//	for (auto b : mapItems)
//	{
//		if (this == b) continue;
//
//		bool isCollide = false;
//
//		ItemTop =		b->GetPosition().y + b->GetCollisionSize().y / 2;	// ブロックの上端
//		ItemBottom =	b->GetPosition().y - b->GetCollisionSize().y / 2;	// ブロックの下端
//		ItemRight =		b->GetPosition().x + b->GetCollisionSize().x / 2;	// ブロックの右端
//		ItemLeft =		b->GetPosition().x - b->GetCollisionSize().x / 2;	// ブロックの左端
//		ItemBack =		b->GetPosition().z + b->GetCollisionSize().z / 2;	// ブロックの後端
//		ItemFront =		b->GetPosition().z - b->GetCollisionSize().z / 2;	// ブロックの前端
//
//		// 当たっている
//		isCollide = Collision::CheckBoxCollider(*this, *b);
//		if (isCollide)
//		{
//			// 上から下に当たった(乗った)
//			if (OwnerOldBottom >= ItemTop && OwnerBottom <= ItemTop)
//			{
//				// プレイヤーの下とブロックの上を比べ、プレイヤーの下がoldの時は上側にあり、現在は下側にある場合
//				// 着地
//				MoveVelocity.y = 0.0f;
//				// プレイヤーの場所を固定(地面に引っかからないように)
//				Position.y = ItemTop + CollisionSize.y / 2 + 0.1f;
//				// 地面に乗っている
//				OnFloor = true;
//				continue;
//			}
//			// 下から上に当たった
//			if (OwnerOldTop <= ItemBottom && OwnerTop >= ItemBottom)
//			{
//				// プレイヤーの上とブロックの下を比べ、プレイヤーの上がoldの時は下側にあり、現在は上側にある場合
//				// 止める
//				MoveVelocity.y = 0.0f;
//				// プレイヤーの場所を固定
//				Position.y = ItemBottom - CollisionSize.y / 2 - 0.1f;
//				continue;
//			}
//			// 左から右に当たった
//			if (OwnerOldRight <= ItemLeft && OwnerRight >= ItemLeft)
//			{
//				Position.x = ItemLeft - CollisionSize.x / 2 - 0.1f; // プレイヤーの場所を固定
//				continue;
//			}
//			// 右から左に当たった
//			if (OwnerOldLeft >= ItemRight && OwnerLeft <= ItemRight)
//			{
//				Position.x = ItemRight + CollisionSize.x / 2 + 0.1f; // プレイヤーの場所を固定
//				continue;
//			}
//			// 後から前に当たった
//			if (OwnerOldBack <= ItemFront && OwnerBack >= ItemFront)
//			{
//				Position.z = ItemFront - CollisionSize.z / 2 - 0.1f; // プレイヤーの場所を固定
//				continue;
//			}
//			// 前から後に当たった
//			if (OwnerOldFront >= ItemBack && OwnerFront <= ItemBack)
//			{
//				Position.z = ItemBack + CollisionSize.z / 2 + 0.1f; // プレイヤーの場所を固定
//				continue;
//			}
//		}
//	}
//
//
///*------------------------------------------------------------------------------
//   MapCharacter
//------------------------------------------------------------------------------*/
//	// 全てのキャラクターを探す
//	std::vector<Player*> player = ObjectCodex::FindActiveObjectsByTag<Player>(character_Player);
//	std::vector<EnemyTest*> enemyTest = ObjectCodex::FindActiveObjectsByTag<EnemyTest>(character_Enemy_Test);
//	std::vector<EnemyTestTwo*> enemyTestTwo = ObjectCodex::FindActiveObjectsByTag<EnemyTestTwo>(character_Enemy_Test_Two);
//	std::vector<EnemyTestThree*> enemyTestThree = ObjectCodex::FindActiveObjectsByTag<EnemyTestThree>(character_Enemy_Test_Three);
//
//	// 全てのキャラクターを整理する
//	std::vector<Character*> mapCharacters;
//	mapCharacters.reserve(player.size() + enemyTest.size() + enemyTestTwo.size() + enemyTestThree.size());
//	mapCharacters.insert(mapCharacters.end(), player.begin(), player.end());
//	mapCharacters.insert(mapCharacters.end(), enemyTest.begin(), enemyTest.end());
//	mapCharacters.insert(mapCharacters.end(), enemyTestTwo.begin(), enemyTestTwo.end());
//	mapCharacters.insert(mapCharacters.end(), enemyTestThree.begin(), enemyTestThree.end());
//
//	// ループでコリジョン判断
//	for (auto b : mapCharacters)
//	{
//		if (this == b || this->IsDeath || b->IsDeath || !this->OnCollision || !b->OnCollision) continue;
//
//		bool isCollide = false;
//
//		ItemTop =		b->GetPosition().y + b->GetCollisionSize().y / 2;	// ブロックの上端
//		ItemBottom =	b->GetPosition().y - b->GetCollisionSize().y / 2;	// ブロックの下端
//		ItemRight =		b->GetPosition().x + b->GetCollisionSize().x / 2;	// ブロックの右端
//		ItemLeft =		b->GetPosition().x - b->GetCollisionSize().x / 2;	// ブロックの左端
//		ItemBack =		b->GetPosition().z + b->GetCollisionSize().z / 2;	// ブロックの後端
//		ItemFront =		b->GetPosition().z - b->GetCollisionSize().z / 2;	// ブロックの前端
//
//		// 当たっている
//		isCollide = Collision::CheckBoxCollider(*this, *b);
//		if (isCollide)
//		{
//			// 上から下に当たった(乗った)
//			if (OwnerOldBottom >= ItemTop && OwnerBottom <= ItemTop)
//			{
//				// プレイヤーの下とブロックの上を比べ、プレイヤーの下がoldの時は上側にあり、現在は下側にある場合
//				// 着地
//				MoveVelocity.y = 0.0f;
//				// プレイヤーの場所を固定(地面に引っかからないように)
//				Position.y = ItemTop + CollisionSize.y / 2 + 0.1f;
//
//				float UpOrDown = (b->GetPosition().z - PositionOld.z > 0.0f) ? 1.0f : -1.0f;
//				float RightOrLeft = (b->GetPosition().x - PositionOld.x > 0.0f) ? 1.0f : -1.0f;
//				b->CalculateMoveVelocity(MoveAccel * 0.1f * RightOrLeft, 0.0f, MoveAccel * 0.1f * UpOrDown);
//
//
//				// 地面に乗っている
//				OnFloor = true;
//				continue;
//			}
//			// 下から上に当たった
//			if (OwnerOldTop <= ItemBottom && OwnerTop >= ItemBottom)
//			{
//				// プレイヤーの上とブロックの下を比べ、プレイヤーの上がoldの時は下側にあり、現在は上側にある場合
//				// 止める
//				MoveVelocity.y = 0.0f;
//				// プレイヤーの場所を固定
//				Position.y = ItemBottom - CollisionSize.y / 2 - 0.1f;
//				continue;
//			}
//			// 左から右に当たった
//			if (OwnerOldRight <= ItemLeft && OwnerRight >= ItemLeft)
//			{
//				Position.x = ItemLeft - CollisionSize.x / 2 - 0.1f; // プレイヤーの場所を固定
//
//				float UpOrDown = (b->GetPosition().z - PositionOld.z > 0.0f) ? 1.0f : -1.0f;
//				b->CalculateMoveVelocity(MoveAccel * 0.2f, 0.0f, MoveAccel * 0.1f * UpOrDown);
//
//				continue;
//			}
//			// 右から左に当たった
//			if (OwnerOldLeft >= ItemRight && OwnerLeft <= ItemRight)
//			{
//				Position.x = ItemRight + CollisionSize.x / 2 + 0.1f; // プレイヤーの場所を固定
//
//				float UpOrDown = (b->GetPosition().z - PositionOld.z > 0.0f) ? 1.0f : -1.0f;
//				b->CalculateMoveVelocity(-MoveAccel * 0.2f, 0.0f, MoveAccel * 0.1f * UpOrDown);
//
//				continue;
//			}
//			// 後から前に当たった
//			if (OwnerOldBack <= ItemFront && OwnerBack >= ItemFront)
//			{
//				Position.z = ItemFront - CollisionSize.z / 2 - 0.1f; // プレイヤーの場所を固定
//
//				float RightOrLeft = (b->GetPosition().x - PositionOld.x > 0.0f) ? 1.0f : -1.0f;
//				b->CalculateMoveVelocity(MoveAccel * 0.1f * RightOrLeft, 0.0f, MoveAccel * 0.2f);
//
//				continue;
//			}
//			// 前から後に当たった
//			if (OwnerOldFront >= ItemBack && OwnerFront <= ItemBack)
//			{
//				Position.z = ItemBack + CollisionSize.z / 2 + 0.1f; // プレイヤーの場所を固定
//
//				float RightOrLeft = (b->GetPosition().x - PositionOld.x > 0.0f) ? 1.0f : -1.0f;
//				b->CalculateMoveVelocity(MoveAccel * 0.1f * RightOrLeft, 0.0f, -MoveAccel * 0.2f);
//
//				continue;
//			}
//		}
//	}
}