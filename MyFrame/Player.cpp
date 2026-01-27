#include "Player.h"
//#include "Enemy.h"
//#include "EnemyTest.h"
//#include "EnemyTestTwo.h"
//#include "EnemyTestThree.h"
#include "Channels.h"

void Player::Update(float dt)
{
	visualPre.Update(dt);





	//// 変更前の座標を格納
	//auto Position = transInfo.position;
	//PositionOld = Position;

	//// 攻撃範囲の更新
	//if (!IsFlip) AttackPosition = { Position.x + CollisionSize.x, Position.y, Position.z };
	//else AttackPosition = { Position.x - CollisionSize.x, Position.y, Position.z };

	//// 状態遷移
	//if (GetWeaponSlots()->GetCanChange().first && !IsDeath)
	//	if (((GetMouse()->LeftIsPressed() && GetMouse()->RightIsPressed()) || (GetKbd()->KeyIsPressed(KK_J) && GetKbd()->KeyIsPressed(KK_K))) ||
	//		(IsFever && !GetWeaponSlots()->GetIsFever()))
	//		SetIsChange(true);	// Feverに入る
	//FSM->Update();
	//if (GetKbd()->KeyIsTriggered(KK_Q)) pWeapon->SwitchSlot(true);
	//if (GetKbd()->KeyIsTriggered(KK_E)) pWeapon->SwitchSlot(false);
	//pResource->Update();
	//pWeapon->Update();

	//// 重力
	//if (!OnFloor) MoveVelocity.y -= GRAVITY;

	//// 抵抗力
	//MoveVelocity.x -= MoveVelocity.x * FORCE_RATE;
	//MoveVelocity.z -= MoveVelocity.z * FORCE_RATE;

	//// 移動
	//Transform(MoveVelocity.x, MoveVelocity.y, MoveVelocity.z);

	//// マップ要素との当たり判定
	//MapItemCollide();
}

void Player::Draw(void)
{
	visualPre.Submit(Chan::main);
	visualPre.Submit(Chan::shadow);




	// 描画
	//FSM->Draw();

#ifdef _DEBUG
	if (!IsDeath)
	{
		//Collision::DrawBoxWireFrame(Position, CollisionSize, pGfx, pCamera, Colors::Red);
		//if (OnAttackCollision)	Collision::DrawBoxWireFrame(AttackPosition, AttackCollision, pGfx, pCamera, Colors::Blue);
		//else Collision::DrawBoxWireFrame(AttackPosition, AttackCollision, pGfx, pCamera, Colors::Cyan);
	}
#endif // _DEBUG
}

void Player::DoMove(float ratio)
{
	if (GetKbd()->KeyIsPressed(KK_A) || GetKbd()->KeyIsPressed(KK_D) || GetKbd()->KeyIsPressed(KK_W) || GetKbd()->KeyIsPressed(KK_S))
	{
		float dx = 0.0f, dz = 0.0f;
		if (GetKbd()->KeyIsPressed(KK_A)) dx--;
		if (GetKbd()->KeyIsPressed(KK_D)) dx++;
		if (GetKbd()->KeyIsPressed(KK_W)) dz++;
		if (GetKbd()->KeyIsPressed(KK_S)) dz--;
		if (abs(dx) > 0 || abs(dz) > 0)
		{
			float angle = atan2f(dx, dz);
			CalculateMoveVelocity(sinf(angle) * GetMoveAccel() * ratio, 0.0f, cosf(angle) * GetMoveAccel() * ratio);
		}
	}
}

bool Player::AttackCollide(float damage, XMFLOAT3 repel)
{
	//// プレイヤーを攻撃できる全ての敵を探す
	//std::vector<EnemyTest*>		 enemyTest = GetObjectPool()->GetObjectsByTag<EnemyTest>(character_Enemy_Test);
	//std::vector<EnemyTestTwo*>	 enemyTestTwo = GetObjectPool()->GetObjectsByTag<EnemyTestTwo>(character_Enemy_Test_Two);
	//std::vector<EnemyTestThree*> enemyTestThree = GetObjectPool()->GetObjectsByTag<EnemyTestThree>(character_Enemy_Test_Three);

	//// 全ての敵を整理する
	//std::vector<Enemy*> enemies;
	//enemies.reserve(enemyTest.size() + enemyTestTwo.size() + enemyTestThree.size());
	//enemies.insert(enemies.end(), enemyTest.begin(), enemyTest.end());
	//enemies.insert(enemies.end(), enemyTestTwo.begin(), enemyTestTwo.end());
	//enemies.insert(enemies.end(), enemyTestThree.begin(), enemyTestThree.end());

	//for (auto e : enemies)
	//{
	//	if (e->GetBeAttackedType() != GetDoAttackType() && !e->GetIsDeath())
	//	{
	//		bool isHit = Collision::CheckBoxCollider(GetAttackPosition(), GetAttackCollisionSize(), e->GetPosition(), e->GetCollisionSize());

	//		if (isHit)
	//		{
	//			e->SetIsHurt(true);	// 攻撃された状態に遷移
	//			e->CalculateHpCurrent(damage); // 体力計算
	//			e->SetBeAttackedType(GetDoAttackType());

	//			float dx = e->GetPosition().x - Position.x;
	//			float dz = e->GetPosition().z - Position.z;
	//			float angle = atan2f(dx, dz);
	//			e->CalculateMoveVelocity(sinf(angle) * repel.x, repel.y, cosf(angle) * repel.z); // 撃退する
	//			//e->CalculateMoveVelocity(IsFlip ? -repel : repel, 0.0f, 0.0f); 

	//			// ゲームリソース判断(Feverじゃない時だけエネルギーがもらえる)
	//			pResource->AttackSettlement(e->GetEnemyType(), IsFever);

	//			// カメラ効果
	//			switch (DoAttackType)
	//			{
	//			case Player_Attack_1: pCamera->SetScreenShake(2, 0.1f, 0.5f); break;
	//			case Player_Attack_2: pCamera->SetScreenShake(2, 0.5f, 1.0f); break;
	//			case Player_Attack_3: pCamera->SetScreenShake(4, 1.0f, 2.0f); break;
	//			case Player_Skill_1:  pCamera->SetScreenShake(4, 2.0f, 3.0f); pCamera->SetScreenFroze(16); break;
	//			case Player_Skill_3:  pCamera->SetScreenShake(2, 0.5f, 1.0f); pCamera->SetScreenFroze(8); break;
	//			case Player_Fever_1:  pCamera->SetScreenShake(4, 3.0f, 4.0f); pCamera->SetScreenFroze(16); break;
	//			case Player_Fever_2:  pCamera->SetScreenShake(2, 1.0f, 2.0f); pCamera->SetScreenFroze(8); break;
	//			case Player_Fever_3:  pCamera->SetScreenShake(1, 1.0f, 2.0f); pCamera->SetScreenFroze(4); break;
	//			}
	//		}
	//	}
	//}
	return false;
}

void Player::SetupTransitions(void)
{
	//// 状態の遷移条件を増加する
	//// PLAYER_IDLE → PLAYER_CHANGE
	//FSM->AddTransition(PLAYER_STATE[PLAYER_IDLE], PLAYER_STATE[PLAYER_CHANGE], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return player->IsChange;
	//	});

	//// PLAYER_IDLE → PLAYER_MOVE
	//FSM->AddTransition(PLAYER_STATE[PLAYER_IDLE], PLAYER_STATE[PLAYER_MOVE], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);	// 入った対象をPlayerになる
	//	return player->pKbd->KeyIsPressed(KK_A) || player->pKbd->KeyIsPressed(KK_D) || player->pKbd->KeyIsPressed(KK_W) || player->pKbd->KeyIsPressed(KK_S); // 判断の条件
	//	});

	//// PLAYER_IDLE → PLAYER_DASH
	//FSM->AddTransition(PLAYER_STATE[PLAYER_IDLE], PLAYER_STATE[PLAYER_DASH], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return !player->IsFever && player->IsDash && player->OnFloor;
	//	});

	//// PLAYER_IDLE → PLAYER_ATTACK
	//FSM->AddTransition(PLAYER_STATE[PLAYER_IDLE], PLAYER_STATE[PLAYER_ATTACK], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return player->IsAttack;
	//	});

	//// PLAYER_IDLE → PLAYER_SKILL
	//FSM->AddTransition(PLAYER_STATE[PLAYER_IDLE], PLAYER_STATE[PLAYER_SKILL], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return !player->IsFever && player->IsSkill;
	//	});

	//// PLAYER_IDLE → PLAYER_HURT
	//FSM->AddTransition(PLAYER_STATE[PLAYER_IDLE], PLAYER_STATE[PLAYER_HURT], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return !player->IsFever && player->IsHurt;
	//	});

	//// PLAYER_MOVE → PLAYER_CHANGE
	//FSM->AddTransition(PLAYER_STATE[PLAYER_MOVE], PLAYER_STATE[PLAYER_CHANGE], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return player->IsChange;
	//	});

	//// PLAYER_MOVE → PLAYER_IDLE
	//FSM->AddTransition(PLAYER_STATE[PLAYER_MOVE], PLAYER_STATE[PLAYER_IDLE], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return !player->pKbd->KeyIsPressed(KK_A) && !player->pKbd->KeyIsPressed(KK_D) && !player->pKbd->KeyIsPressed(KK_W) && !player->pKbd->KeyIsPressed(KK_S);
	//	});

	//// PLAYER_MOVE → PLAYER_DASH
	//FSM->AddTransition(PLAYER_STATE[PLAYER_MOVE], PLAYER_STATE[PLAYER_DASH], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return !player->IsFever && player->IsDash && player->OnFloor;
	//	});

	//// PLAYER_MOVE → PLAYER_ATTACK
	//FSM->AddTransition(PLAYER_STATE[PLAYER_MOVE], PLAYER_STATE[PLAYER_ATTACK], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return player->IsAttack;
	//	});

	//// PLAYER_MOVE → PLAYER_SKILL
	//FSM->AddTransition(PLAYER_STATE[PLAYER_MOVE], PLAYER_STATE[PLAYER_SKILL], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return !player->IsFever && player->IsSkill;
	//	});

	//// PLAYER_MOVE → PLAYER_HURT
	//FSM->AddTransition(PLAYER_STATE[PLAYER_MOVE], PLAYER_STATE[PLAYER_HURT], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return !player->IsFever && player->IsHurt;
	//	});

	//// PLAYER_DASH → PLAYER_CHANGE
	//FSM->AddTransition(PLAYER_STATE[PLAYER_DASH], PLAYER_STATE[PLAYER_CHANGE], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return player->IsChange;
	//	});

	//// PLAYER_DASH → PLAYER_IDLE
	//FSM->AddTransition(PLAYER_STATE[PLAYER_DASH], PLAYER_STATE[PLAYER_IDLE], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return !player->IsDash;
	//	});

	//// PLAYER_DASH → PLAYER_HURT
	//FSM->AddTransition(PLAYER_STATE[PLAYER_DASH], PLAYER_STATE[PLAYER_HURT], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return player->IsHurt;
	//	});

	//// PLAYER_ATTACK → PLAYER_CHANGE
	//FSM->AddTransition(PLAYER_STATE[PLAYER_ATTACK], PLAYER_STATE[PLAYER_CHANGE], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return player->IsChange;
	//	});

	//// PLAYER_ATTACK → PLAYER_IDLE
	//FSM->AddTransition(PLAYER_STATE[PLAYER_ATTACK], PLAYER_STATE[PLAYER_IDLE], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return !player->IsAttack;
	//	});

	//// PLAYER_ATTACK → PLAYER_DASH
	//FSM->AddTransition(PLAYER_STATE[PLAYER_ATTACK], PLAYER_STATE[PLAYER_DASH], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return !player->IsFever && player->IsDash;
	//	});

	//// PLAYER_ATTACK → PLAYER_HURT
	//FSM->AddTransition(PLAYER_STATE[PLAYER_ATTACK], PLAYER_STATE[PLAYER_HURT], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return !player->IsFever && player->IsHurt;
	//	});

	//// PLAYER_SKILL → PLAYER_CHANGE
	//FSM->AddTransition(PLAYER_STATE[PLAYER_SKILL], PLAYER_STATE[PLAYER_CHANGE], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return player->IsChange;
	//	});

	//// PLAYER_SKILL → PLAYER_IDLE
	//FSM->AddTransition(PLAYER_STATE[PLAYER_SKILL], PLAYER_STATE[PLAYER_IDLE], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return !player->IsSkill;
	//	});

	//// PLAYER_SKILL → PLAYER_DASH
	//FSM->AddTransition(PLAYER_STATE[PLAYER_SKILL], PLAYER_STATE[PLAYER_DASH], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return player->IsDash;
	//	});

	//// PLAYER_SKILL → PLAYER_HURT
	//FSM->AddTransition(PLAYER_STATE[PLAYER_SKILL], PLAYER_STATE[PLAYER_HURT], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return player->IsHurt;
	//	});

	//// PLAYER_HURT → PLAYER_CHANGE
	//FSM->AddTransition(PLAYER_STATE[PLAYER_HURT], PLAYER_STATE[PLAYER_CHANGE], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return player->IsChange && player->HpCurrent > 0;
	//	});

	//// PLAYER_HURT → PLAYER_IDLE
	//FSM->AddTransition(PLAYER_STATE[PLAYER_HURT], PLAYER_STATE[PLAYER_IDLE], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return !player->IsHurt && !player->IsDeath;
	//	});

	//// PLAYER_HURT → PLAYER_DEATH
	//FSM->AddTransition(PLAYER_STATE[PLAYER_HURT], PLAYER_STATE[PLAYER_DEATH], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return player->IsDeath;
	//	});

	//// PLAYER_CHANGE → PLAYER_IDLE
	//FSM->AddTransition(PLAYER_STATE[PLAYER_CHANGE], PLAYER_STATE[PLAYER_IDLE], [](Character* owner) {
	//	auto player = static_cast<Player*>(owner);
	//	return !player->IsChange;
	//	});
}


///*------------------------------------------------------------------------------
//   Player_IdleState関数
//------------------------------------------------------------------------------*/
//void Player_IdleState::OnEnter(Player* owner)
//{
//	// ポリゴンから開始時間を取って、アニメーションエラーを避ける
//	startTime = std::chrono::steady_clock::now();
//	FrameNo = 0;	// フレームを1から計算
//	FrameTotalCount = 24;
//}
//
//void Player_IdleState::Update(Player* owner)
//{
//	// フレーム更新
//	auto totalFramesPassed = 5 * (std::chrono::steady_clock::now() - startTime) / frameDuration;
//	// 繰り返すのアクションはフレームを1からリセット
//	FrameNo = (totalFramesPassed % FrameTotalCount + 1);
//
//	// 遷移判断
//	if (owner->GetKbd()->KeyIsTriggered(KK_SPACE))										owner->SetIsDash(true);
//	if (owner->GetMouse()->LeftIsTriggered() || owner->GetKbd()->KeyIsTriggered(KK_J))	owner->SetIsAttack(true);
//	if (owner->GetMouse()->RightIsTriggered() || owner->GetKbd()->KeyIsTriggered(KK_K))
//		if (owner->GetWeaponSlots()->GetCurrentSlot().GetType() != WEAPON_TYPE_0)		owner->SetIsSkill(true);
//
//	owner->DoAttackCountDown();	// 攻撃をカウントダウン
//}
//
//void Player_IdleState::Draw(Player* owner)
//{
//	switch (owner->GetFeverType())
//	{
//	case WEAPON_TYPE_0:
//		switch (owner->GetWeaponSlots()->GetCurrentSlot().GetType())
//		{
//		case WEAPON_TYPE_0:	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 16, 145, FrameTotalCount, FrameNo, owner->GetTexture(0), !owner->GetIsFlip());	break;
//		case WEAPON_TYPE_1:	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 12, 49, FrameTotalCount, FrameNo, owner->GetTexture(1), !owner->GetIsFlip());	break;
//		case WEAPON_TYPE_2:	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 12, 49, FrameTotalCount, FrameNo, owner->GetTexture(2), !owner->GetIsFlip());	break;
//		case WEAPON_TYPE_3:	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 12, 49, FrameTotalCount, FrameNo, owner->GetTexture(3), !owner->GetIsFlip());	break;
//		}
//		break;
//	case WEAPON_TYPE_1:	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 10, 97, FrameTotalCount, FrameNo, owner->GetTexture(4), !owner->GetIsFlip());	break;
//	case WEAPON_TYPE_2:	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 10, 97, FrameTotalCount, FrameNo, owner->GetTexture(4), !owner->GetIsFlip());	break;
//	case WEAPON_TYPE_3:	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 10, 97, FrameTotalCount, FrameNo, owner->GetTexture(4), !owner->GetIsFlip());	break;
//	}
//}
//
//
///*------------------------------------------------------------------------------
//   Player_MoveState関数
//------------------------------------------------------------------------------*/
//void Player_MoveState::OnEnter(Player* owner)
//{
//	startTime = std::chrono::steady_clock::now();
//	FrameNo = 0;
//	FrameTotalCount = 24;
//}
//
//void Player_MoveState::Update(Player* owner)
//{
//	auto totalFramesPassed = 5 * (std::chrono::steady_clock::now() - startTime) / frameDuration;
//	// 繰り返すのアクションはフレームを1からリセット
//	FrameNo = (totalFramesPassed % FrameTotalCount + 1);
//
//	// 遷移判断
//	if (owner->GetKbd()->KeyIsTriggered(KK_SPACE))										owner->SetIsDash(true);
//	if (owner->GetMouse()->LeftIsTriggered() || owner->GetKbd()->KeyIsTriggered(KK_J))	owner->SetIsAttack(true);
//	if (owner->GetMouse()->RightIsTriggered() || owner->GetKbd()->KeyIsTriggered(KK_K))
//		if (owner->GetWeaponSlots()->GetCurrentSlot().GetType() != WEAPON_TYPE_0)		owner->SetIsSkill(true);
//
//	// 方向判断
//	if (owner->GetKbd()->KeyIsPressed(KK_A)) owner->SetIsFlip(true);
//	if (owner->GetKbd()->KeyIsPressed(KK_D)) owner->SetIsFlip(false);
//
//	// 移動
//	owner->DoMove(1.0f);
//
//	// 攻撃をカウントダウン
//	owner->DoAttackCountDown();
//}
//
//void Player_MoveState::Draw(Player* owner)
//{
//	switch (owner->GetFeverType())
//	{
//	case WEAPON_TYPE_0:
//		switch (owner->GetWeaponSlots()->GetCurrentSlot().GetType())
//		{
//		case WEAPON_TYPE_0:	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 16, 169, FrameTotalCount, FrameNo, owner->GetTexture(0), !owner->GetIsFlip());	break;
//		case WEAPON_TYPE_1:	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 12, 73, FrameTotalCount, FrameNo, owner->GetTexture(1), !owner->GetIsFlip());	break;
//		case WEAPON_TYPE_2:	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 12, 73, FrameTotalCount, FrameNo, owner->GetTexture(2), !owner->GetIsFlip());	break;
//		case WEAPON_TYPE_3:	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 12, 73, FrameTotalCount, FrameNo, owner->GetTexture(3), !owner->GetIsFlip());	break;
//		}
//		break;
//	case WEAPON_TYPE_1:	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 10, 97, FrameTotalCount, FrameNo, owner->GetTexture(4), !owner->GetIsFlip());	break;
//	case WEAPON_TYPE_2:	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 10, 97, FrameTotalCount, FrameNo, owner->GetTexture(4), !owner->GetIsFlip());	break;
//	case WEAPON_TYPE_3:	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 10, 97, FrameTotalCount, FrameNo, owner->GetTexture(4), !owner->GetIsFlip());	break;
//	}
//}
//
//
///*------------------------------------------------------------------------------
//   Player_DashState関数
//------------------------------------------------------------------------------*/
//void Player_DashState::OnEnter(Player* owner)
//{
//	startTime = std::chrono::steady_clock::now();
//	owner->SetIsFlip(owner->GetMoveVelocity().x > 0 ? false : true);
//	FrameNo = 0;
//	FrameTotalCount = 12;
//
//	FrameNoOld = 0;
//	if (owner->GetKbd()->KeyIsPressed(KK_A) || owner->GetKbd()->KeyIsPressed(KK_D) || owner->GetKbd()->KeyIsPressed(KK_W) || owner->GetKbd()->KeyIsPressed(KK_S))
//		isInput = true;
//	else
//		isInput = false;
//}
//
//void Player_DashState::OnExit(Player* owner)
//{
//	owner->SetIsDash(false);
//}
//
//void Player_DashState::Update(Player* owner)
//{
//	FrameNo = 5 * (std::chrono::steady_clock::now() - startTime) / frameDuration + 1;
//	// 終わったらデッシュフラグを閉じる
//	if (FrameNo > FrameTotalCount)	owner->SetIsDash(false);
//
//	// デッシュ中に攻撃とスキルを事前入力する
//	if (FrameTotalCount - FrameNo < 2)
//	{
//		if (!owner->GetIsAttack() && !owner->GetIsSkill())
//		{
//			if (owner->GetMouse()->LeftIsTriggered() || owner->GetKbd()->KeyIsTriggered(KK_J))	owner->SetIsAttack(true);
//			if (owner->GetMouse()->RightIsTriggered() || owner->GetKbd()->KeyIsTriggered(KK_K))
//				if (owner->GetWeaponSlots()->GetCurrentSlot().GetType() != WEAPON_TYPE_0)		owner->SetIsSkill(true);
//		}
//	}
//
//	// Dash発生の判断
//	if (FrameNoOld != FrameNo && FrameNo == 1)
//	{
//		if (isInput)
//		{
//			XMFLOAT3 move = owner->GetMoveVelocity();
//			float angle = atan2f(move.x, move.z);
//			owner->CalculateMoveVelocity(sinf(angle) * owner->GetMoveAccel() * 40.0f, 0.0f, cosf(angle) * owner->GetMoveAccel() * 40.0f);
//		}
//		else
//			owner->CalculateMoveVelocity((owner->GetIsFlip() ? -1.0f : 1.0f) * owner->GetMoveAccel() * 40.0f, 0.0f, 0.0f);
//	}
//
//	// 攻撃をカウントダウン
//	owner->DoAttackCountDown();
//
//	// 変更前の座標を更新
//	FrameNoOld = FrameNo;
//}
//
//void Player_DashState::Draw(Player* owner)
//{
//	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 16, 121, FrameTotalCount, FrameNo, owner->GetTexture(0), !owner->GetIsFlip());
//}
//
//
///*------------------------------------------------------------------------------
//   Player_AttackState関数
//------------------------------------------------------------------------------*/
//void Player_AttackState::OnEnter(Player* owner)
//{
//	startTime = std::chrono::steady_clock::now();
//	FrameNo = 0;
//	switch (owner->GetFeverType())
//	{
//	case WEAPON_TYPE_0:	FrameTotalCount = 15;	owner->SetDoAttackType(Player_Attack_1);	owner->GetWeaponSlots()->UseWeapon(-20.0f);	break;
//	case WEAPON_TYPE_1:	FrameTotalCount = 12;	owner->SetDoAttackType(Player_Fever_1);	break;
//	case WEAPON_TYPE_2:	FrameTotalCount = 12;	owner->SetDoAttackType(Player_Fever_2);	break;
//	case WEAPON_TYPE_3:	FrameTotalCount = 12;	owner->SetDoAttackType(Player_Fever_3);	break;
//	}
//
//	FrameNoOld = 0;
//	isCombo = false;
//}
//
//void Player_AttackState::OnExit(Player* owner)
//{
//	owner->ResetAttackCountDown();			// 攻撃終わったら攻撃のカウンターをリセットする
//	owner->SetDoAttackType(Attack_Type_None);
//	owner->SetAttackCollisionOnOff(false);	// もし攻撃の途中で攻撃されたら、こっちに攻撃フラグとコリジョンを閉じる
//	owner->SetIsAttack(false);
//}
//
//void Player_AttackState::Update(Player* owner)
//{
//	FrameNo = 5 * (std::chrono::steady_clock::now() - startTime) / frameDuration + 1;
//	// 終わったら攻撃フラグを閉じる
//	if (FrameNo > FrameTotalCount)	owner->SetIsAttack(false);
//
//	// ダッシュで攻撃をキャンセル
//	if (owner->GetKbd()->KeyIsTriggered(KK_SPACE))	owner->SetIsDash(true);
//	// 攻撃中にスキルを事前入力する
//	if (FrameTotalCount - FrameNo < 10)
//	{
//		if (owner->GetMouse()->RightIsTriggered() || owner->GetKbd()->KeyIsTriggered(KK_K))
//			if (owner->GetWeaponSlots()->GetCurrentSlot().GetType() != WEAPON_TYPE_0)
//				owner->SetIsSkill(true);
//		if (owner->GetMouse()->LeftIsTriggered() || owner->GetKbd()->KeyIsTriggered(KK_J))
//		{
//			if (!owner->GetIsFever()) owner->GetWeaponSlots()->UseWeapon(-25.0f);
//			isCombo = true;
//		}
//	}
//
//	switch (owner->GetFeverType())
//	{
//	case WEAPON_TYPE_0:
//	{
//		switch (owner->GetDoAttackType())
//		{
//		case Player_Attack_1:
//		{
//			// 攻撃コリジョンを開けるフレーム
//			if (FrameNoOld != FrameNo && FrameNo == 9)
//				owner->SetAttackCollisionOnOff(true);
//			// 攻撃コリジョンを閉じるフレーム
//			if (FrameNoOld != FrameNo && FrameNo == 14)
//				owner->SetAttackCollisionOnOff(false);
//			// 攻撃当たる判定
//			if (owner->GetAttackCollisionOnOff()) owner->AttackCollide(-1.0f, { 0.5f,0.5f,0.5f });
//			break;
//		}
//		case Player_Attack_2:
//		{
//			// 攻撃コリジョンを開けるフレーム
//			if (FrameNoOld != FrameNo && FrameNo == 25)	owner->SetAttackCollisionOnOff(true);
//			// 攻撃コリジョンを閉じるフレーム
//			if (FrameNoOld != FrameNo && FrameNo == 30)	owner->SetAttackCollisionOnOff(false);
//			// 攻撃当たる判定
//			if (owner->GetAttackCollisionOnOff()) owner->AttackCollide(-1.0f, { 1.0f,1.0f,1.0f });
//			break;
//		}
//		case Player_Attack_3:
//		{
//			// 攻撃コリジョンを開けるフレーム
//			if (FrameNoOld != FrameNo && FrameNo == 36)
//				owner->SetAttackCollisionOnOff(true);
//			// 攻撃コリジョンを閉じるフレーム
//			if (FrameNoOld != FrameNo && FrameNo == 41)
//				owner->SetAttackCollisionOnOff(false);
//			// 攻撃当たる判定
//			if (owner->GetAttackCollisionOnOff()) owner->AttackCollide(-1.0f, { 1.5f ,0.5f,1.5f });
//			break;
//		}
//		}
//
//		// 連撃の判断
//		if (FrameNo == FrameTotalCount && isCombo)
//		{
//			// もし連撃したら、スキルの事前入力を消ます
//			owner->SetIsSkill(false);
//			owner->SetDoAttackType(static_cast<Attack_Type_Tag>(owner->GetDoAttackType() + 1));
//
//			FrameTotalCount = (owner->GetDoAttackType() == Player_Attack_2) ? 32 : 48;
//
//			isCombo = false;
//		}
//		break;
//	}
//	case WEAPON_TYPE_1:
//	case WEAPON_TYPE_2:
//	case WEAPON_TYPE_3:
//	{
//		switch (owner->GetDoAttackType())
//		{
//		case Player_Attack_1:
//		{
//			// 攻撃コリジョンを開けるフレーム
//			if (FrameNoOld != FrameNo && FrameNo == 5)
//				owner->SetAttackCollisionOnOff(true);
//			// 攻撃コリジョンを閉じるフレーム
//			if (FrameNoOld != FrameNo && FrameNo == 9)
//				owner->SetAttackCollisionOnOff(false);
//			// 攻撃当たる判定
//			if (owner->GetAttackCollisionOnOff()) owner->AttackCollide(-1.0f, { 0.5f,0.5f,0.5f });
//			break;
//		}
//		case Player_Attack_2:
//		{
//			// 攻撃コリジョンを開けるフレーム
//			if (FrameNoOld != FrameNo && FrameNo == 16)	owner->SetAttackCollisionOnOff(true);
//			// 攻撃コリジョンを閉じるフレーム
//			if (FrameNoOld != FrameNo && FrameNo == 21)	owner->SetAttackCollisionOnOff(false);
//			// 攻撃当たる判定
//			if (owner->GetAttackCollisionOnOff()) owner->AttackCollide(-1.0f, { 1.0f,1.0f,1.0f });
//			break;
//		}
//		case Player_Attack_3:
//		{
//			// 攻撃コリジョンを開けるフレーム
//			if (FrameNoOld != FrameNo && FrameNo == 45)
//				owner->SetAttackCollisionOnOff(true);
//			// 攻撃コリジョンを閉じるフレーム
//			if (FrameNoOld != FrameNo && FrameNo == 46)
//				owner->SetAttackCollisionOnOff(false);
//			// 攻撃当たる判定
//			if (owner->GetAttackCollisionOnOff()) owner->AttackCollide(-1.0f, { 1.5f ,0.5f,1.5f });
//			break;
//		}
//		}
//
//		// 連撃の判断
//		if (FrameNo == FrameTotalCount && isCombo)
//		{
//			// もし連撃したら、スキルの事前入力を消ます
//			owner->SetIsSkill(false);
//			owner->SetDoAttackType(static_cast<Attack_Type_Tag>(owner->GetDoAttackType() + 1));
//
//			FrameTotalCount = (owner->GetDoAttackType() == Player_Attack_2) ? 30 : 48;
//
//			isCombo = false;
//		}
//		break;
//	}
//	}
//
//	// 移動
//	owner->DoMove(0.1f);
//
//	// 変更前の座標を更新
//	FrameNoOld = FrameNo;
//}
//
//void Player_AttackState::Draw(Player* owner)
//{
//	switch (owner->GetFeverType())
//	{
//	case WEAPON_TYPE_0:
//		switch (owner->GetWeaponSlots()->GetCurrentSlot().GetType())
//		{
//		case WEAPON_TYPE_0:	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 16, 49, FrameTotalCount, FrameNo, owner->GetTexture(0), !owner->GetIsFlip());	break;
//		case WEAPON_TYPE_1:	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 12, 1, FrameTotalCount, FrameNo, owner->GetTexture(1), !owner->GetIsFlip());	break;
//		case WEAPON_TYPE_2:	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 12, 1, FrameTotalCount, FrameNo, owner->GetTexture(2), !owner->GetIsFlip());	break;
//		case WEAPON_TYPE_3:	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 12, 1, FrameTotalCount, FrameNo, owner->GetTexture(3), !owner->GetIsFlip());	break;
//		}
//		break;
//	case WEAPON_TYPE_1:	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 10, 1, FrameTotalCount, FrameNo, owner->GetTexture(4), !owner->GetIsFlip());	break;
//	case WEAPON_TYPE_2:	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 10, 1, FrameTotalCount, FrameNo, owner->GetTexture(4), !owner->GetIsFlip());	break;
//	case WEAPON_TYPE_3:	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 10, 1, FrameTotalCount, FrameNo, owner->GetTexture(4), !owner->GetIsFlip());	break;
//	}
//}
//
//
///*------------------------------------------------------------------------------
//   Player_SkillState
//------------------------------------------------------------------------------*/
//void Player_SkillState::OnEnter(Player* owner)
//{
//	startTime = std::chrono::steady_clock::now();
//	WeaponType = owner->GetWeaponSlots()->GetCurrentSlot().GetType();
//	FrameNo = 0;
//	switch (WeaponType)
//	{
//	case WEAPON_TYPE_1:
//	{
//		owner->SetAttackCollisionSize({ 50.0f,10.0f,50.0f });
//		owner->SetDoAttackType(Player_Skill_1);
//		break;
//	}
//	case WEAPON_TYPE_2:
//	{
//		owner->SetDoAttackType(Player_Skill_2);
//		break;
//	}
//	case WEAPON_TYPE_3:
//	{
//		owner->SetDoAttackType(Player_Skill_3);
//		break;
//	}
//	}
//	FrameTotalCount = 48;
//
//	FrameNoOld = 0;
//
//	owner->GetWeaponSlots()->UseWeapon(-100.0f);
//}
//
//void Player_SkillState::OnExit(Player* owner)
//{
//	switch (WeaponType)
//	{
//	case WEAPON_TYPE_1:
//	{
//		owner->SetAttackCollisionSize({ 20.0f,10.0f,10.0f });
//		break;
//	}
//	case WEAPON_TYPE_2:
//	{
//		break;
//	}
//	case WEAPON_TYPE_3:
//	{
//		owner->SetCollisionOnOff(true); // コリジュンを開ける
//		break;
//	}
//	}
//
//	owner->SetDoAttackType(Attack_Type_None);
//	owner->SetIsSkill(false);
//}
//
//void Player_SkillState::Update(Player* owner)
//{
//	FrameNo = 5 * (std::chrono::steady_clock::now() - startTime) / frameDuration + 1;
//	// 終わったらスキルフラグを閉じる
//	if (FrameNo > FrameTotalCount) owner->SetIsSkill(false);
//
//	// ダッシュでスキルをキャンセル
//	if (owner->GetKbd()->KeyIsTriggered(KK_SPACE))	owner->SetIsDash(true);
//	// スキル中に攻撃とスキルを事前入力する
//	if (FrameTotalCount - FrameNo < 2)
//	{
//		if (!owner->GetIsAttack() && !owner->GetIsSkill())
//		{
//			if (owner->GetMouse()->LeftIsTriggered() || owner->GetKbd()->KeyIsTriggered(KK_J))	owner->SetIsAttack(true);
//			if (owner->GetMouse()->RightIsTriggered() || owner->GetKbd()->KeyIsTriggered(KK_K))
//				if (owner->GetWeaponSlots()->GetCurrentSlot().GetType() != WEAPON_TYPE_0)		owner->SetIsSkill(true);
//		}
//	}
//
//	switch (WeaponType)
//	{
//	case WEAPON_TYPE_1:
//	{
//		owner->SetAttackPosition(owner->GetPosition());
//		// 攻撃コリジョンを開けるフレーム
//		if (FrameNoOld != FrameNo && FrameNo == 31) owner->SetAttackCollisionOnOff(true);
//		// 攻撃コリジョンを閉じるフレーム
//		if (FrameNoOld != FrameNo && FrameNo == 46) owner->SetAttackCollisionOnOff(false);
//		// 攻撃当たる判定
//		if (owner->GetAttackCollisionOnOff()) owner->AttackCollide(-3.0f, { 3.0f,1.0f,3.0f });
//		break;
//	}
//	case WEAPON_TYPE_2:
//		if (FrameNoOld != FrameNo && FrameNo == 45) owner->CalculateHpCurrent(3.0f);
//		break;
//	case WEAPON_TYPE_3:
//		// 攻撃コリジョンを開けるフレーム
//		if (FrameNoOld != FrameNo && FrameNo == 22)
//		{
//			owner->SetAttackCollisionOnOff(true);
//			owner->SetCollisionOnOff(false);	// コリジュンを閉じる
//			owner->CalculateMoveVelocity((owner->GetIsFlip() ? -1.0f : 1.0f) * owner->GetMoveAccel() * 100.0f, 0.0f, 0.0f);
//		}
//		// 攻撃コリジョンを閉じるフレーム
//		if (FrameNoOld != FrameNo && FrameNo == 43) owner->SetAttackCollisionOnOff(false);
//		// 攻撃当たる判定
//		if (owner->GetAttackCollisionOnOff()) owner->AttackCollide(-1.0f, { 2.0f,0.0f,3.0f });
//		break;
//	}
//
//	// 移動
//	owner->DoMove(0.1f);
//
//	// 変更前の座標を更新
//	FrameNoOld = FrameNo;
//}
//
//void Player_SkillState::Draw(Player* owner)
//{
//	switch (WeaponType)
//	{
//	case WEAPON_TYPE_1:
//	{
//		owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 12, 97, FrameTotalCount, FrameNo, owner->GetTexture(1), !owner->GetIsFlip());
//		break;
//	}
//	case WEAPON_TYPE_2:
//	{
//		owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 12, 97, FrameTotalCount, FrameNo, owner->GetTexture(2), !owner->GetIsFlip());
//		break;
//	}
//	case WEAPON_TYPE_3:
//	{
//		owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 12, 97, FrameTotalCount, FrameNo, owner->GetTexture(3), !owner->GetIsFlip());
//		break;
//	}
//	}
//}
//
//
///*------------------------------------------------------------------------------
//   Player_HurtState
//------------------------------------------------------------------------------*/
//void Player_HurtState::OnEnter(Player* owner)
//{
//	startTime = std::chrono::steady_clock::now();
//	FrameNo = 0;
//	FrameTotalCount = 6;
//	owner->SetCollisionOnOff(false); // コリジュンを閉じる
//
//	owner->GetResourceBars()->ResetCombo();
//}
//
//void Player_HurtState::OnExit(Player* owner)
//{
//	owner->SetIsHurt(false); // DeathStateに遷移するかも、も一回IsHurtをリセットする
//	owner->SetCollisionOnOff(true); // コリジュンを開ける
//}
//
//void Player_HurtState::Update(Player* owner)
//{
//	FrameNo = 5 * (std::chrono::steady_clock::now() - startTime) / frameDuration + 1;
//	// 攻撃された後は何するの判断
//	if (FrameNo > FrameTotalCount)
//	{
//		// もし体力がゼロたら、死ぬ状態で遷移
//		if (owner->GetHpCurrent() <= 0)	owner->SetIsDeath(true);
//		// 終わったら攻撃されたフラグを閉じる
//		else owner->SetIsHurt(false);
//	}
//
//	// 攻撃されたの中にデッシュと攻撃とスキルを事前入力する
//	if (FrameTotalCount - FrameNo < 2)
//	{
//		if (!owner->GetIsDash() && !owner->GetIsAttack() && !owner->GetIsSkill())
//		{
//			if (owner->GetKbd()->KeyIsTriggered(KK_SPACE))										owner->SetIsDash(true);
//			if (owner->GetMouse()->LeftIsTriggered() || owner->GetKbd()->KeyIsTriggered(KK_J))	owner->SetIsAttack(true);
//			if (owner->GetMouse()->RightIsTriggered() || owner->GetKbd()->KeyIsTriggered(KK_K))
//				if (owner->GetWeaponSlots()->GetCurrentSlot().GetType() != WEAPON_TYPE_0)		owner->SetIsSkill(true);
//		}
//	}
//
//	// 攻撃をカウントダウン
//	owner->DoAttackCountDown();
//}
//
//void Player_HurtState::Draw(Player* owner)
//{
//	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 16, 97, FrameTotalCount, FrameNo, owner->GetTexture(0), !owner->GetIsFlip());
//}
//
//
///*------------------------------------------------------------------------------
//   Player_DeathState
//------------------------------------------------------------------------------*/
//void Player_DeathState::OnEnter(Player* owner)
//{
//	startTime = std::chrono::steady_clock::now();
//	FrameNo = 0;
//	FrameTotalCount = 48;
//}
//
//void Player_DeathState::Update(Player* owner)
//{
//	FrameNo = 5 * (std::chrono::steady_clock::now() - startTime) / frameDuration + 1;
//	// 終わったら何がする…
//	if (FrameNo > FrameTotalCount)	owner->SetIsGameOver(true);
//}
//
//void Player_DeathState::Draw(Player* owner)
//{
//	owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 16, 1, FrameTotalCount, FrameNo, owner->GetTexture(0), !owner->GetIsFlip());
//}
//
//
///*------------------------------------------------------------------------------
//   Player_ChangeState
//------------------------------------------------------------------------------*/
//void Player_ChangeState::OnEnter(Player* owner)
//{
//	startTime = std::chrono::steady_clock::now();
//	FrameNo = 0;
//	if (owner->GetIsFever())
//		FrameTotalCount = 48;
//	else
//	{
//		owner->GetWeaponSlots()->DoChange();
//		FrameTotalCount = 48;
//	}
//}
//
//void Player_ChangeState::OnExit(Player* owner)
//{
//	if (owner->GetIsFever()) owner->SetFeverType(WEAPON_TYPE_0);
//	else owner->SetFeverType(owner->GetWeaponSlots()->GetCanChange().second);
//
//	owner->SetIsDash(false);
//	owner->SetIsAttack(false);
//	owner->SetIsSkill(false);
//	owner->SetIsHurt(false);
//
//	owner->GetWeaponSlots()->FinishChange();
//
//	owner->SetIsFever(!owner->GetIsFever());
//}
//
//void Player_ChangeState::Update(Player* owner)
//{
//	FrameNo = 5 * (std::chrono::steady_clock::now() - startTime) / frameDuration + 1;
//	//変身完了の判断
//	if (FrameNo > FrameTotalCount)	owner->SetIsChange(false);
//}
//
//void Player_ChangeState::Draw(Player* owner)
//{
//	if (owner->GetIsFever()) owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 10, 49, FrameTotalCount, FrameNo, owner->GetTexture(4), !owner->GetIsFlip());
//	else owner->GetPolygon()->PolygonRectDraw(owner->GetPosition(), owner->GetSize(), 12, 10, 49, FrameTotalCount, FrameNo, owner->GetTexture(4), !owner->GetIsFlip());
//}