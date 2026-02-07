#include "Player.h"
#include "Channels.h"

#include "Enemy.h"
#include "Enemy_Red_0.h"
#include "Enemy_Red_1.h"
#include "Enemy_Green_0.h"
#include "Enemy_Blue_0.h"
#include "Enemy_Blue_1.h"

void Player::Update(float dt)
{
	// 変更前の座標を格納
	auto Position = transInfo.position;
	PositionOld = Position;
	// 攻撃範囲の更新
	if (!IsFlip) attackCollider.center = { Position.x + boxCollider.half.x + attackCollider.half.x, Position.y, Position.z };
	else attackCollider.center = { Position.x - boxCollider.half.x - attackCollider.half.x, Position.y, Position.z };

	// 状態遷移
	// Fever related
	if (GetWeaponSlots()->GetCanChange().first && !IsDeath)
	{
		if (((GetMouse()->LeftIsPressed() && GetMouse()->RightIsPressed()) || (GetKbd()->KeyIsPressed(KK_J) && GetKbd()->KeyIsPressed(KK_K))) || // Get into Fever
			(IsFever && !GetWeaponSlots()->GetIsFever())) // Exit Fever
		{
			SetIsChange(true);	// Feverに入る
		}
	}

	// FSM update
	FSM->Update(dt);
	visualPre_Effect_Bg->Update(dt);

	// Weapon ralated update
	{
		if (!FSM->IsInState("PLAYER_ATTACK") && !FSM->IsInState("PLAYER_SKILL"))
		{
			if (GetKbd()->KeyIsTriggered(KK_Q)) pWeapon->SwitchSlot(true);
			if (GetKbd()->KeyIsTriggered(KK_E)) pWeapon->SwitchSlot(false);
		}
		if ((currentWeaponType != pWeapon->GetCurrentSlot().GetType() && !FSM->IsInState("PLAYER_SKILL")) || // Not in Skill
			GetWasSkill()) // after Skill
		{
			currentWeaponType = pWeapon->GetCurrentSlot().GetType();
			switch (currentWeaponType)
			{
			case WEAPON_TYPE_0: visualPre_Effect_Bg->SetFrameAuto(12, 10, 1, 120, 0, 20.0f, false, true); break;
			case WEAPON_TYPE_1: visualPre_Effect_Bg->SetFrameAuto(10, 10, 1, 100, 1, 20.0f, false, true); break;
			case WEAPON_TYPE_2: visualPre_Effect_Bg->SetFrameAuto(12, 12, 1, 144, 2, 20.0f, false, true); break;
			case WEAPON_TYPE_3: visualPre_Effect_Bg->SetFrameAuto(12, 12, 1, 144, 3, 20.0f, false, true); break;
			}
		}
		pResource->Update();
		pWeapon->Update();
	}

	// 重力
	if (!OnFloor) MoveVelocity.y -= GRAVITY * dt;

	// 抵抗力
	MoveVelocity.x -= MoveVelocity.x * FORCE_RATE;
	MoveVelocity.z -= MoveVelocity.z * FORCE_RATE;

	// 移動
	{
		Transform(MoveVelocity.x, MoveVelocity.y, MoveVelocity.z);
		visualPre->SetPosition(transInfo.position);
		if(!FSM->IsInState("PLAYER_SKILL"))
		{
			visualPre_Effect_Bg->SetPosition(transInfo.position.x, transInfo.position.y, transInfo.position.z + 0.1f);
			visualPre_Effect->SetPosition(attackCollider.center.x - (IsFlip ? -0.5f : 0.5f), attackCollider.center.y + 0.5f, attackCollider.center.z - 0.1f);
		}
	}

	// マップ要素との当たり判定
	boxCollider.center = transInfo.position;
	MapItemCollide();
}

void Player::Submit(void)
{
	// 描画
	// visualPre
	visualPre->Submit(Chan::main);
	visualPre->Submit(Chan::shadow);
	// visualPre_Effect_Bg
	visualPre_Effect_Bg->Submit(Chan::main);
	// visualPre_Effect
	if ((FSM->IsInState("PLAYER_ATTACK") && pWeapon->GetCurrentSlot().GetType() != WEAPON_TYPE_2) // attack state
		|| FSM->IsInState("PLAYER_SKILL")) // skill state
		visualPre_Effect->Submit(Chan::main);

#ifdef _DEBUG
	if (!IsDeath)
	{
		boxColliderWire->DoSubmit(transInfo.position, boxCollider.GetSize());
		attackColliderWire->DoSubmit(attackCollider.center, attackCollider.GetSize());
	}
#endif
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
	std::vector<Enemy*> enemies;
	for (auto tag : {
		character_Enemy_Red_0,
		character_Enemy_Red_1,
		character_Enemy_Green_0,
		character_Enemy_Green_1,
		character_Enemy_Blue_0,
		character_Enemy_Blue_1
		}) {
		// プレイヤーを攻撃できる全ての敵を探す
		auto found = ObjectCodex::FindActiveObjectsByTag<Enemy>(tag);
		// 全ての敵を整理する
		enemies.reserve(enemies.size() + found.size());
		enemies.insert(enemies.end(), found.begin(), found.end());
	}

	for (auto e : enemies)
	{
		if (e->GetBeAttackedType() != GetDoAttackType() && !e->GetIsDeath())
		{
			bool isHit = CollisionSystem::IsOverlap(attackCollider, e->GetBoxCollider());

			if (isHit)
			{
				e->SetIsHurt(true);	// 攻撃された状態に遷移
				e->SetWasHurt(true);
				e->CalculateHpCurrent(damage); // 体力計算
				e->SetBeAttackedType(GetDoAttackType());

				float dx = e->GetPosition().x - transInfo.position.x;
				float dz = e->GetPosition().z - transInfo.position.z;
				float angle = atan2f(dx, dz);
				e->CalculateMoveVelocity(sinf(angle) * repel.x, repel.y, cosf(angle) * repel.z); // 撃退する
				//e->CalculateMoveVelocity(IsFlip ? -repel : repel, 0.0f, 0.0f); 

				// ゲームリソース判断(Feverじゃない時だけエネルギーがもらえる)
				pResource->AttackSettlement(e->GetEnemyType(), IsFever);

				// カメラ効果
				switch (DoAttackType)
				{
				case Player_Attack_1: AttackCameraShake(4, 0.3f, 0.5f); break;
				case Player_Attack_2: AttackCameraShake(4, 0.5f, 1.0f); break;
				case Player_Attack_3: AttackCameraShake(6, 1.0f, 1.5f); break;
				case Player_Skill_1:  AttackCameraShake(8, 2.0f, 3.0f); pCamera->SetScreenFroze(16); break;
				case Player_Skill_3:  AttackCameraShake(2, 0.5f, 1.0f); pCamera->SetScreenFroze(8); break;
				case Player_Fever_1:  AttackCameraShake(8, 3.0f, 4.0f); pCamera->SetScreenFroze(16); break;
				case Player_Fever_2:  AttackCameraShake(4, 1.0f, 2.0f); pCamera->SetScreenFroze(8); break;
				case Player_Fever_3:  AttackCameraShake(2, 1.0f, 2.0f); pCamera->SetScreenFroze(4); break;
				}
			}
		}
	}
	return false;
}

void Player::AttackCameraShake(int frames, float minRange, float maxRange)
{
	pCamera->SetScreenShake(frames, minRange, maxRange);
}

void Player::SetupTransitions(void)
{
	// 状態の遷移条件を増加する
	// PLAYER_IDLE → PLAYER_CHANGE
	FSM->AddTransition(PLAYER_STATE[PLAYER_IDLE], PLAYER_STATE[PLAYER_CHANGE], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return player->IsChange;
		});

	// PLAYER_IDLE → PLAYER_MOVE
	FSM->AddTransition(PLAYER_STATE[PLAYER_IDLE], PLAYER_STATE[PLAYER_MOVE], [](Character* owner) {
		auto player = static_cast<Player*>(owner);	// 入った対象をPlayerになる
		return player->pKbd->KeyIsPressed(KK_A) || player->pKbd->KeyIsPressed(KK_D) || player->pKbd->KeyIsPressed(KK_W) || player->pKbd->KeyIsPressed(KK_S); // 判断の条件
		});

	// PLAYER_IDLE → PLAYER_DASH
	FSM->AddTransition(PLAYER_STATE[PLAYER_IDLE], PLAYER_STATE[PLAYER_DASH], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return !player->IsFever && player->IsDash && player->OnFloor;
		});

	// PLAYER_IDLE → PLAYER_ATTACK
	FSM->AddTransition(PLAYER_STATE[PLAYER_IDLE], PLAYER_STATE[PLAYER_ATTACK], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return player->IsAttack;
		});

	// PLAYER_IDLE → PLAYER_SKILL
	FSM->AddTransition(PLAYER_STATE[PLAYER_IDLE], PLAYER_STATE[PLAYER_SKILL], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return !player->IsFever && player->IsSkill;
		});

	// PLAYER_IDLE → PLAYER_HURT
	FSM->AddTransition(PLAYER_STATE[PLAYER_IDLE], PLAYER_STATE[PLAYER_HURT], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return !player->IsFever && player->IsHurt;
		});

	// PLAYER_MOVE → PLAYER_CHANGE
	FSM->AddTransition(PLAYER_STATE[PLAYER_MOVE], PLAYER_STATE[PLAYER_CHANGE], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return player->IsChange;
		});

	// PLAYER_MOVE → PLAYER_IDLE
	FSM->AddTransition(PLAYER_STATE[PLAYER_MOVE], PLAYER_STATE[PLAYER_IDLE], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return !player->pKbd->KeyIsPressed(KK_A) && !player->pKbd->KeyIsPressed(KK_D) && !player->pKbd->KeyIsPressed(KK_W) && !player->pKbd->KeyIsPressed(KK_S);
		});

	// PLAYER_MOVE → PLAYER_DASH
	FSM->AddTransition(PLAYER_STATE[PLAYER_MOVE], PLAYER_STATE[PLAYER_DASH], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return !player->IsFever && player->IsDash && player->OnFloor;
		});

	// PLAYER_MOVE → PLAYER_ATTACK
	FSM->AddTransition(PLAYER_STATE[PLAYER_MOVE], PLAYER_STATE[PLAYER_ATTACK], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return player->IsAttack;
		});

	// PLAYER_MOVE → PLAYER_SKILL
	FSM->AddTransition(PLAYER_STATE[PLAYER_MOVE], PLAYER_STATE[PLAYER_SKILL], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return !player->IsFever && player->IsSkill;
		});

	// PLAYER_MOVE → PLAYER_HURT
	FSM->AddTransition(PLAYER_STATE[PLAYER_MOVE], PLAYER_STATE[PLAYER_HURT], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return !player->IsFever && player->IsHurt;
		});

	// PLAYER_DASH → PLAYER_CHANGE
	FSM->AddTransition(PLAYER_STATE[PLAYER_DASH], PLAYER_STATE[PLAYER_CHANGE], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return player->IsChange;
		});

	// PLAYER_DASH → PLAYER_IDLE
	FSM->AddTransition(PLAYER_STATE[PLAYER_DASH], PLAYER_STATE[PLAYER_IDLE], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return !player->IsDash;
		});

	// PLAYER_DASH → PLAYER_HURT
	FSM->AddTransition(PLAYER_STATE[PLAYER_DASH], PLAYER_STATE[PLAYER_HURT], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return player->IsHurt;
		});

	// PLAYER_ATTACK → PLAYER_CHANGE
	FSM->AddTransition(PLAYER_STATE[PLAYER_ATTACK], PLAYER_STATE[PLAYER_CHANGE], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return player->IsChange;
		});

	// PLAYER_ATTACK → PLAYER_IDLE
	FSM->AddTransition(PLAYER_STATE[PLAYER_ATTACK], PLAYER_STATE[PLAYER_IDLE], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return !player->IsAttack;
		});

	// PLAYER_ATTACK → PLAYER_DASH
	FSM->AddTransition(PLAYER_STATE[PLAYER_ATTACK], PLAYER_STATE[PLAYER_DASH], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return !player->IsFever && player->IsDash;
		});

	// PLAYER_ATTACK → PLAYER_HURT
	FSM->AddTransition(PLAYER_STATE[PLAYER_ATTACK], PLAYER_STATE[PLAYER_HURT], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return !player->IsFever && player->IsHurt;
		});

	// PLAYER_SKILL → PLAYER_CHANGE
	FSM->AddTransition(PLAYER_STATE[PLAYER_SKILL], PLAYER_STATE[PLAYER_CHANGE], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return player->IsChange;
		});

	// PLAYER_SKILL → PLAYER_IDLE
	FSM->AddTransition(PLAYER_STATE[PLAYER_SKILL], PLAYER_STATE[PLAYER_IDLE], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return !player->IsSkill;
		});

	// PLAYER_SKILL → PLAYER_DASH
	FSM->AddTransition(PLAYER_STATE[PLAYER_SKILL], PLAYER_STATE[PLAYER_DASH], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return player->IsDash;
		});

	// PLAYER_SKILL → PLAYER_HURT
	FSM->AddTransition(PLAYER_STATE[PLAYER_SKILL], PLAYER_STATE[PLAYER_HURT], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return player->IsHurt;
		});

	// PLAYER_HURT → PLAYER_CHANGE
	FSM->AddTransition(PLAYER_STATE[PLAYER_HURT], PLAYER_STATE[PLAYER_CHANGE], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return player->IsChange && player->HpCurrent > 0;
		});

	// PLAYER_HURT → PLAYER_IDLE
	FSM->AddTransition(PLAYER_STATE[PLAYER_HURT], PLAYER_STATE[PLAYER_IDLE], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return !player->IsHurt && !player->IsDeath;
		});

	// PLAYER_HURT → PLAYER_DEATH
	FSM->AddTransition(PLAYER_STATE[PLAYER_HURT], PLAYER_STATE[PLAYER_DEATH], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return player->IsDeath;
		});

	// PLAYER_CHANGE → PLAYER_IDLE
	FSM->AddTransition(PLAYER_STATE[PLAYER_CHANGE], PLAYER_STATE[PLAYER_IDLE], [](Character* owner) {
		auto player = static_cast<Player*>(owner);
		return !player->IsChange;
		});
}


/*------------------------------------------------------------------------------
   Player_IdleState
------------------------------------------------------------------------------*/
Player_IdleState::Player_IdleState()
{
	packs.emplace_back(12, 16, 145, 24);
	packs.emplace_back(12, 12,  49, 24);
	packs.emplace_back(12, 10,  97, 24);

	assist.Loop = true;
	assist.FPS = packs[0].FPS;
	assist.FrameTotal = packs[0].FrameTotalCount;
}

void Player_IdleState::OnEnter(Player* owner)
{
	assist.Reset();
}

void Player_IdleState::Update(Player* owner,  float dt)
{
	assist.Update(dt);

	// 遷移判断
	if (owner->GetKbd()->KeyIsTriggered(KK_SPACE))										owner->SetIsDash(true);
	if (owner->GetMouse()->LeftIsTriggered() || owner->GetKbd()->KeyIsTriggered(KK_J))	owner->SetIsAttack(true);
	if (owner->GetMouse()->RightIsTriggered() || owner->GetKbd()->KeyIsTriggered(KK_K))
		if (owner->GetWeaponSlots()->GetCurrentSlot().GetType() != WEAPON_TYPE_0)		owner->SetIsSkill(true);

	// 攻撃をカウントダウン
	owner->DoAttackCountDown();

	// sprite set
	if (owner->GetFeverType() == WEAPON_TYPE_0)
	{
		switch (owner->GetWeaponSlots()->GetCurrentSlot().GetType())
		{
		case WEAPON_TYPE_0:	owner->GetVisualPre()->SetFrame(packs[0].numU, packs[0].numV, packs[0].FrameStart, packs[0].FrameTotalCount, assist.FrameNo, 0, !owner->GetIsFlip()); break;
		case WEAPON_TYPE_1: owner->GetVisualPre()->SetFrame(packs[1].numU, packs[1].numV, packs[1].FrameStart, packs[1].FrameTotalCount, assist.FrameNo, 1, !owner->GetIsFlip()); break;
		case WEAPON_TYPE_2: owner->GetVisualPre()->SetFrame(packs[1].numU, packs[1].numV, packs[1].FrameStart, packs[1].FrameTotalCount, assist.FrameNo, 2, !owner->GetIsFlip()); break;
		case WEAPON_TYPE_3: owner->GetVisualPre()->SetFrame(packs[1].numU, packs[1].numV, packs[1].FrameStart, packs[1].FrameTotalCount, assist.FrameNo, 3, !owner->GetIsFlip()); break;
		default: break;
		}
	}
	else
	{
		owner->GetVisualPre()->SetFrame(packs[2].numU, packs[2].numV, packs[2].FrameStart, packs[2].FrameTotalCount, assist.FrameNo, 4, !owner->GetIsFlip());
	}
}


/*------------------------------------------------------------------------------
   Player_MoveState関数
------------------------------------------------------------------------------*/
Player_MoveState::Player_MoveState()
{
	packs.emplace_back(12, 16, 169, 24);
	packs.emplace_back(12, 12,  73, 24);
	packs.emplace_back(12, 10,  97, 24);

	assist.Loop = true;
	assist.FPS = packs[0].FPS;
	assist.FrameTotal = packs[0].FrameTotalCount;
}

void Player_MoveState::OnEnter(Player* owner)
{
	assist.Reset();
}

void Player_MoveState::Update(Player* owner, float dt)
{
	assist.Update(dt);

	// 遷移判断
	if (owner->GetKbd()->KeyIsTriggered(KK_SPACE))										owner->SetIsDash(true);
	if (owner->GetMouse()->LeftIsTriggered() || owner->GetKbd()->KeyIsTriggered(KK_J))	owner->SetIsAttack(true);
	if (owner->GetMouse()->RightIsTriggered() || owner->GetKbd()->KeyIsTriggered(KK_K))
		if (owner->GetWeaponSlots()->GetCurrentSlot().GetType() != WEAPON_TYPE_0)		owner->SetIsSkill(true);

	// 方向判断
	if (owner->GetKbd()->KeyIsPressed(KK_A)) owner->SetIsFlip(true);
	if (owner->GetKbd()->KeyIsPressed(KK_D)) owner->SetIsFlip(false);

	// 移動
	owner->DoMove(1.0f);

	// 攻撃をカウントダウン
	owner->DoAttackCountDown();

	// sprite set
	if (owner->GetFeverType() == WEAPON_TYPE_0)
	{
		switch (owner->GetWeaponSlots()->GetCurrentSlot().GetType())
		{
		case WEAPON_TYPE_0:	owner->GetVisualPre()->SetFrame(packs[0].numU, packs[0].numV, packs[0].FrameStart, packs[0].FrameTotalCount, assist.FrameNo, 0, !owner->GetIsFlip()); break;
		case WEAPON_TYPE_1: owner->GetVisualPre()->SetFrame(packs[1].numU, packs[1].numV, packs[1].FrameStart, packs[1].FrameTotalCount, assist.FrameNo, 1, !owner->GetIsFlip()); break;
		case WEAPON_TYPE_2: owner->GetVisualPre()->SetFrame(packs[1].numU, packs[1].numV, packs[1].FrameStart, packs[1].FrameTotalCount, assist.FrameNo, 2, !owner->GetIsFlip()); break;
		case WEAPON_TYPE_3: owner->GetVisualPre()->SetFrame(packs[1].numU, packs[1].numV, packs[1].FrameStart, packs[1].FrameTotalCount, assist.FrameNo, 3, !owner->GetIsFlip()); break;
		default: break;
		}
	}
	else
	{
		owner->GetVisualPre()->SetFrame(packs[2].numU, packs[2].numV, packs[2].FrameStart, packs[2].FrameTotalCount, assist.FrameNo, 4, !owner->GetIsFlip());
	}
}


/*------------------------------------------------------------------------------
   Player_DashState関数
------------------------------------------------------------------------------*/
void Player_DashState::OnEnter(Player* owner)
{
	// anime set
	owner->GetVisualPre()->SetFrameAuto(pack.numU, pack.numV, pack.FrameStart, pack.FrameTotalCount, 0, pack.FPS, owner->GetMoveVelocity().x >= 0 ? true : false, false);

	// dash direction preparation
	if (owner->GetKbd()->KeyIsPressed(KK_A) || owner->GetKbd()->KeyIsPressed(KK_D) || owner->GetKbd()->KeyIsPressed(KK_W) || owner->GetKbd()->KeyIsPressed(KK_S))
		isInput = true;
	else
		isInput = false;
}

void Player_DashState::Update(Player* owner, float dt)
{
	owner->GetVisualPre()->Update(dt);

	// デッシュ中に攻撃とスキルを事前入力する
	int currentFrame = owner->GetVisualPre()->GetCurrentFrame();

	if (pack.FrameTotalCount - currentFrame < 2)
	{
		if (!owner->GetIsAttack() && !owner->GetIsSkill())
		{
			if (owner->GetMouse()->LeftIsTriggered()  || owner->GetKbd()->KeyIsTriggered(KK_J))	owner->SetIsAttack(true);
			if (owner->GetMouse()->RightIsTriggered() || owner->GetKbd()->KeyIsTriggered(KK_K))
				if (owner->GetWeaponSlots()->GetCurrentSlot().GetType() != WEAPON_TYPE_0)		owner->SetIsSkill(true);
		}
	}

	// Dash発生の判断
	if (currentFrame == 1)
	{
		if (isInput)
		{
			XMFLOAT3 move = owner->GetMoveVelocity();
			float angle = atan2f(move.x, move.z);
			owner->CalculateMoveVelocity(sinf(angle) * owner->GetMoveAccel() * 40.0f, 0.0f, cosf(angle) * owner->GetMoveAccel() * 40.0f);
		}
		else
			owner->CalculateMoveVelocity((owner->GetIsFlip() ? -1.0f : 1.0f) * owner->GetMoveAccel() * 40.0f, 0.0f, 0.0f);
	}

	// 攻撃をカウントダウン
	owner->DoAttackCountDown();

	// 終わったらデッシュフラグを閉じる
	if (owner->GetVisualPre()->ClipFinished())	owner->SetIsDash(false);
}


/*------------------------------------------------------------------------------
   Player_AttackState関数
------------------------------------------------------------------------------*/
Player_AttackState::Player_AttackState()
{
	packs.emplace_back(12, 16, 49, 15);
	packs.emplace_back(12, 12,  1, 15);
	packs.emplace_back(12, 10,  1, 12);
	ePacks.emplace_back( 7,  7, 1, 15);
	ePacks.emplace_back(12, 12, 1, 15);
	ePacks.emplace_back(12, 14, 1, 15);
	ePacks.emplace_back(12, 13, 1, 15);

	assist.Loop = false;
	assist.FPS = packs[0].FPS;
	assist.FrameTotal = packs[0].FrameTotalCount;
	oldFrame = 0;
}

void Player_AttackState::OnEnter(Player* owner)
{
	// anime set
	assist.Reset();
	switch (owner->GetFeverType())
	{
	case WEAPON_TYPE_0:	
	{
		auto type = owner->GetWeaponSlots()->GetCurrentSlot().GetType();
		if (type == WEAPON_TYPE_0)
		{
			mode = 0;
		}
		else
		{
			mode = 1;
		}
		packs[mode].FrameTotalCount = 15;
		owner->SetDoAttackType(Player_Attack_1);
		owner->GetWeaponSlots()->UseWeapon(-10.0f);
	}
		break;
	case WEAPON_TYPE_1:	mode = 2; packs[mode].FrameTotalCount = 12; owner->SetDoAttackType(Player_Fever_1);	break;
	case WEAPON_TYPE_2:	mode = 2; packs[mode].FrameTotalCount = 12; owner->SetDoAttackType(Player_Fever_2);	break;
	case WEAPON_TYPE_3:	mode = 2; packs[mode].FrameTotalCount = 12; owner->SetDoAttackType(Player_Fever_3);	break;
	}
	assist.FrameTotal = packs[mode].FrameTotalCount;

	isCombo = false;
}

void Player_AttackState::OnExit(Player* owner)
{
	owner->ResetAttackCountDown();			// 攻撃終わったら攻撃のカウンターをリセットする
	owner->SetDoAttackType(Attack_Type_None);
	owner->SetAttackCollisionOnOff(false);	// もし攻撃の途中で攻撃されたら、こっちに攻撃フラグとコリジョンを閉じる
	owner->SetIsAttack(false);
}

void Player_AttackState::Update(Player* owner, float dt)
{
	assist.Update(dt);

	auto type = owner->GetWeaponSlots()->GetCurrentSlot().GetType();

	// ダッシュで攻撃をキャンセル
	if (owner->GetKbd()->KeyIsTriggered(KK_SPACE))	owner->SetIsDash(true);
	// 攻撃中にスキルを事前入力する
	int currentFrame = assist.GetCurrentFrame();
	if (packs[mode].FrameTotalCount - currentFrame < 10)
	{
		if (owner->GetMouse()->RightIsTriggered() || owner->GetKbd()->KeyIsTriggered(KK_K))
			if (owner->GetWeaponSlots()->GetCurrentSlot().GetType() != WEAPON_TYPE_0)
				owner->SetIsSkill(true);
		if (!isCombo)
		{
			if (owner->GetMouse()->LeftIsTriggered() || owner->GetKbd()->KeyIsTriggered(KK_J))
			{
				isCombo = true;

				if (!owner->GetIsFever()) 
				{
					if (owner->GetDoAttackType() == Player_Attack_1)
					{
						switch (type)
						{
						case WEAPON_TYPE_1:	owner->GetWeaponSlots()->UseWeapon(-30.0f); break;
						case WEAPON_TYPE_2:	owner->GetWeaponSlots()->UseWeapon(-20.0f); break;
						case WEAPON_TYPE_3: owner->GetWeaponSlots()->UseWeapon(-10.0f); break;
						}
					}
					if (owner->GetDoAttackType() == Player_Attack_2)
					{
						switch (type)
						{
						case WEAPON_TYPE_1:	owner->GetWeaponSlots()->UseWeapon(-40.0f); break;
						case WEAPON_TYPE_2:	owner->GetWeaponSlots()->UseWeapon(-50.0f); break;
						case WEAPON_TYPE_3: owner->GetWeaponSlots()->UseWeapon(-10.0f); break;
						}
					}
				}
			}
		}
	}

	if (mode != 2)
	{
		switch (owner->GetDoAttackType())
		{
		case Player_Attack_1:
		{
			// 攻撃コリジョンを開けるフレーム
			if (currentFrame == 9 && oldFrame != currentFrame)
				owner->SetAttackCollisionOnOff(true);
			// 攻撃コリジョンを閉じるフレーム
			if (currentFrame == 14 && oldFrame != currentFrame)
				owner->SetAttackCollisionOnOff(false);
			// 攻撃当たる判定
			if (owner->GetAttackCollisionOnOff())
			{
				switch (type)
				{
				case WEAPON_TYPE_0:	owner->AttackCollide(-1.0f, { 0.2f,0.1f,0.2f }); break;
				case WEAPON_TYPE_1:	owner->AttackCollide(-2.0f, { 0.3f,0.2f,0.3f }); break;
				case WEAPON_TYPE_3: owner->AttackCollide(-1.0f, { 0.1f,0.1f,0.1f }); break;
				}
			}
			break;
		}
		case Player_Attack_2:
		{
			// 攻撃コリジョンを開けるフレーム
			if (currentFrame == 25 && oldFrame != currentFrame)	owner->SetAttackCollisionOnOff(true);
			// 攻撃コリジョンを閉じるフレーム
			if (currentFrame == 30 && oldFrame != currentFrame)	owner->SetAttackCollisionOnOff(false);
			// 攻撃当たる判定
			if (owner->GetAttackCollisionOnOff()) 
			{
				switch (type)
				{
				case WEAPON_TYPE_0:	owner->AttackCollide(-1.0f, { 0.1f,0.2f,0.1f }); break;
				case WEAPON_TYPE_1:	owner->AttackCollide(-2.0f, { 0.2f,0.3f,0.2f }); break;
				case WEAPON_TYPE_3: owner->AttackCollide(-1.0f, { 0.1f,0.1f,0.1f }); break;
				}
			}
			break;
		}
		case Player_Attack_3:
		{
			// 攻撃コリジョンを開けるフレーム
			if (currentFrame == 36 && oldFrame != currentFrame)
				owner->SetAttackCollisionOnOff(true);
			// 攻撃コリジョンを閉じるフレーム
			if (currentFrame == 41 && oldFrame != currentFrame)
				owner->SetAttackCollisionOnOff(false);
			// 攻撃当たる判定
			if (owner->GetAttackCollisionOnOff())
			{
				switch (type)
				{
				case WEAPON_TYPE_0:	owner->AttackCollide(-2.0f, { 0.3f,0.1f,0.3f }); break;
				case WEAPON_TYPE_1:	owner->AttackCollide(-2.0f, { 0.4f,0.2f,0.4f }); break;
				case WEAPON_TYPE_3: owner->AttackCollide(-1.0f, { 0.1f,0.1f,0.1f }); break;
				}
			}
			break;
		}
		}
	}
	else
	{
		switch (owner->GetDoAttackType())
		{
		case Player_Attack_1:
		{
			// 攻撃コリジョンを開けるフレーム
			if (currentFrame == 5 && oldFrame != currentFrame)
				owner->SetAttackCollisionOnOff(true);
			// 攻撃コリジョンを閉じるフレーム
			if (currentFrame == 9 && oldFrame != currentFrame)
				owner->SetAttackCollisionOnOff(false);
			// 攻撃当たる判定
			if (owner->GetAttackCollisionOnOff()) owner->AttackCollide(-1.0f, { 0.5f,0.5f,0.5f });
			break;
		}
		case Player_Attack_2:
		{
			// 攻撃コリジョンを開けるフレーム
			if (currentFrame == 16 && oldFrame != currentFrame)	owner->SetAttackCollisionOnOff(true);
			// 攻撃コリジョンを閉じるフレーム
			if (currentFrame == 21 && oldFrame != currentFrame)	owner->SetAttackCollisionOnOff(false);
			// 攻撃当たる判定
			if (owner->GetAttackCollisionOnOff()) owner->AttackCollide(-1.0f, { 1.0f,1.0f,1.0f });
			break;
		}
		case Player_Attack_3:
		{
			// 攻撃コリジョンを開けるフレーム
			if (currentFrame == 45 && oldFrame != currentFrame)
				owner->SetAttackCollisionOnOff(true);
			// 攻撃コリジョンを閉じるフレーム
			if (currentFrame == 46 && oldFrame != currentFrame)
				owner->SetAttackCollisionOnOff(false);
			// 攻撃当たる判定
			if (owner->GetAttackCollisionOnOff()) owner->AttackCollide(-1.0f, { 1.5f ,0.5f,1.5f });
			break;
		}
		}
	}

	// 連撃の判断
	if (currentFrame == packs[mode].FrameTotalCount && isCombo)
	{
		// もし連撃したら、スキルの事前入力を消ます
		owner->SetIsSkill(false);
		owner->SetDoAttackType(static_cast<Attack_Type_Tag>(owner->GetDoAttackType() + 1));

		if (mode != 2)
			packs[mode].FrameTotalCount = (owner->GetDoAttackType() == Player_Attack_2) ? 32 : 48;
		else
			packs[mode].FrameTotalCount = (owner->GetDoAttackType() == Player_Attack_2) ? 30 : 48;
		assist.FrameTotal = packs[mode].FrameTotalCount;

		isCombo = false;
	}

	// 移動
	owner->DoMove(0.1f);

	// sprite set
	if (mode != 2)
	{
		switch (owner->GetWeaponSlots()->GetCurrentSlot().GetType())
		{
		case WEAPON_TYPE_0:	
				owner->GetVisualPre()->SetFrame(packs[0].numU, packs[0].numV, packs[0].FrameStart, packs[0].FrameTotalCount, assist.FrameNo, 0, !owner->GetIsFlip()); 
				owner->GetVisualPreEffect()->SetFrame(ePacks[0].numU, ePacks[0].numV, ePacks[0].FrameStart, packs[0].FrameTotalCount, assist.FrameNo, 0, !owner->GetIsFlip());
				break;
		case WEAPON_TYPE_1: 
				owner->GetVisualPre()->SetFrame(packs[1].numU, packs[1].numV, packs[1].FrameStart, packs[1].FrameTotalCount, assist.FrameNo, 1, !owner->GetIsFlip()); 
				owner->GetVisualPreEffect()->SetFrame(ePacks[1].numU, ePacks[1].numV, ePacks[1].FrameStart, packs[1].FrameTotalCount, assist.FrameNo, 1, !owner->GetIsFlip());
				break;
		case WEAPON_TYPE_2: 
				owner->GetVisualPre()->SetFrame(packs[1].numU, packs[1].numV, packs[1].FrameStart, packs[1].FrameTotalCount, assist.FrameNo, 2, !owner->GetIsFlip()); 
				//owner->GetVisualPreEffectAttack()->SetFrame(ePacks[2].numU, ePacks[2].numV, ePacks[2].FrameStart, packs[1].FrameTotalCount, assist.FrameNo, 2, !owner->GetIsFlip());
				break;
		case WEAPON_TYPE_3: 
				owner->GetVisualPre()->SetFrame(packs[1].numU, packs[1].numV, packs[1].FrameStart, packs[1].FrameTotalCount, assist.FrameNo, 3, !owner->GetIsFlip()); 
				owner->GetVisualPreEffect()->SetFrame(ePacks[3].numU, ePacks[3].numV, ePacks[3].FrameStart, packs[1].FrameTotalCount, assist.FrameNo, 3, !owner->GetIsFlip());
				break;
		}
	}
	else
	{
		owner->GetVisualPre()->SetFrame(packs[2].numU, packs[2].numV, packs[2].FrameStart, packs[2].FrameTotalCount, assist.FrameNo, 4, !owner->GetIsFlip());
	}

	// save old frame
	oldFrame = currentFrame;

	// close the attack flag when the attack is done
	if (assist.Finished())	owner->SetIsAttack(false);
}


/*------------------------------------------------------------------------------
   Player_SkillState
------------------------------------------------------------------------------*/
Player_SkillState::Player_SkillState()
{
	ePacks.emplace_back(12, 12, 49, 48);
	ePacks.emplace_back(12, 14, 49, 48);
	ePacks.emplace_back(12, 13, 49, 48);
	bgPacks.emplace_back(12, 12, 97, 48);
	bgPacks.emplace_back(12, 14, 97, 48);
	bgPacks.emplace_back(12, 13, 97, 48);
}

void Player_SkillState::OnEnter(Player* owner)
{
	baseAttackCollisionSize = owner->GetAttackCollisionSize();

	// anime set
	WeaponType = owner->GetWeaponSlots()->GetCurrentSlot().GetType();
	switch (WeaponType)
	{
	case WEAPON_TYPE_1: 
		owner->GetVisualPre()->SetFrameAuto(pack.numU, pack.numV, pack.FrameStart, pack.FrameTotalCount, 1, pack.FPS, !owner->GetIsFlip(), false);
		owner->GetVisualPreEffectBg()->SetFrameAuto(bgPacks[0].numU, bgPacks[0].numV, bgPacks[0].FrameStart, bgPacks[0].FrameTotalCount, 4, bgPacks[0].FPS, !owner->GetIsFlip(), false);
		owner->GetVisualPreEffect()->SetFrameAuto(ePacks[0].numU, ePacks[0].numV, ePacks[0].FrameStart, ePacks[0].FrameTotalCount, 1, ePacks[0].FPS, !owner->GetIsFlip(), false);
		break;
	case WEAPON_TYPE_2: 
		owner->GetVisualPre()->SetFrameAuto(pack.numU, pack.numV, pack.FrameStart, pack.FrameTotalCount, 2, pack.FPS, !owner->GetIsFlip(), false);
		owner->GetVisualPreEffectBg()->SetFrameAuto(bgPacks[1].numU, bgPacks[1].numV, bgPacks[1].FrameStart, bgPacks[1].FrameTotalCount, 5, bgPacks[1].FPS, !owner->GetIsFlip(), false);
		owner->GetVisualPreEffect()->SetFrameAuto(ePacks[1].numU, ePacks[1].numV, ePacks[1].FrameStart, ePacks[1].FrameTotalCount, 2, ePacks[1].FPS, !owner->GetIsFlip(), false);
		break;
	case WEAPON_TYPE_3: 
		owner->GetVisualPre()->SetFrameAuto(pack.numU, pack.numV, pack.FrameStart, pack.FrameTotalCount, 3, pack.FPS, !owner->GetIsFlip(), false);
		owner->GetVisualPreEffectBg()->SetFrameAuto(bgPacks[2].numU, bgPacks[2].numV, bgPacks[2].FrameStart, bgPacks[2].FrameTotalCount, 6, bgPacks[2].FPS, !owner->GetIsFlip(), false);
		owner->GetVisualPreEffect()->SetFrameAuto(ePacks[2].numU, ePacks[2].numV, ePacks[2].FrameStart, ePacks[2].FrameTotalCount, 3, ePacks[2].FPS, !owner->GetIsFlip(), false);
		break;
	}

	// skill set
	switch (WeaponType)
	{
	case WEAPON_TYPE_1:
		owner->SetAttackCollisionSize({ 20.0f,5.0f,20.0f });
		owner->GetVisualPreEffectBg()->SetScale(20.0f, 20.0f, 0.0f);
		owner->GetVisualPreEffect()->SetScale(20.0f, 20.0f, 0.0f);
		owner->SetDoAttackType(Player_Skill_1);
		break;
	case WEAPON_TYPE_2:
		owner->GetVisualPreEffectBg()->SetScale(7.5f, 7.5f, 0.0f);
		owner->GetVisualPreEffect()->SetScale(7.5, 7.5f, 0.0f);
		owner->SetDoAttackType(Player_Skill_2);
		break;
	case WEAPON_TYPE_3:
		owner->SetAttackCollisionSize({ 5.0f,5.0f,5.0f });
		owner->GetVisualPreEffectBg()->SetScale(10.0f, 10.0f, 0.0f);
		owner->GetVisualPreEffect()->SetScale(10.0f, 10.0f, 0.0f);
		owner->SetDoAttackType(Player_Skill_3);
		break;
	}
	owner->GetWeaponSlots()->UseWeapon(-100.0f);
}

void Player_SkillState::OnExit(Player* owner)
{
	switch (WeaponType)
	{
	case WEAPON_TYPE_1:
	{
		owner->SetAttackCollisionSize(baseAttackCollisionSize);
		owner->GetVisualPreEffectBg()->SetScale(owner->GetEffectSize());
		owner->GetVisualPreEffect()->SetScale(owner->GetEffectSize());
		owner->SetWasSkill(true);
		break;
	}
	case WEAPON_TYPE_2:
	{
		owner->GetVisualPreEffectBg()->SetScale(owner->GetEffectSize());
		owner->GetVisualPreEffect()->SetScale(owner->GetEffectSize());
		owner->SetWasSkill(true);
		break;
	}
	case WEAPON_TYPE_3:
	{
		owner->SetAttackCollisionSize(baseAttackCollisionSize);
		owner->GetVisualPreEffectBg()->SetScale(owner->GetEffectSize());
		owner->GetVisualPreEffect()->SetScale(owner->GetEffectSize());
		owner->SetCollisionOnOff(true); // コリジュンを開ける
		owner->SetWasSkill(true);
		break;
	}
	}

	owner->SetDoAttackType(Attack_Type_None);
	owner->SetIsSkill(false);
}

void Player_SkillState::Update(Player* owner, float dt)
{
	// set anime
	owner->GetVisualPre()->Update(dt);
	owner->GetVisualPreEffect()->Update(dt);

	// ダッシュでスキルをキャンセル
	if (owner->GetKbd()->KeyIsTriggered(KK_SPACE))	owner->SetIsDash(true);
	// スキル中に攻撃とスキルを事前入力する
	int currentFrame = owner->GetVisualPre()->GetCurrentFrame();

	if (pack.FrameTotalCount - currentFrame < 2)
	{
		if (!owner->GetIsAttack() && !owner->GetIsSkill())
		{
			if (owner->GetMouse()->LeftIsTriggered() || owner->GetKbd()->KeyIsTriggered(KK_J))	owner->SetIsAttack(true);
			if (owner->GetMouse()->RightIsTriggered() || owner->GetKbd()->KeyIsTriggered(KK_K))
				if (owner->GetWeaponSlots()->GetCurrentSlot().GetType() != WEAPON_TYPE_0)		owner->SetIsSkill(true);
		}
	}

	auto pos = owner->GetPosition();
	switch (WeaponType)
	{
	case WEAPON_TYPE_1:
	{
		owner->SetAttackPosition(owner->GetPosition());
		// 攻撃コリジョンを開けるフレーム
		if (currentFrame == 31) owner->SetAttackCollisionOnOff(true);
		// 攻撃コリジョンを閉じるフレーム
		if (currentFrame == 46) owner->SetAttackCollisionOnOff(false);
		// 攻撃当たる判定
		if (owner->GetAttackCollisionOnOff()) 
			owner->AttackCollide(-3.0f, { 1.0f,0.5f,1.0f });
		// VisualPrePos
		owner->GetVisualPreEffectBg()->SetPosition(pos.x, pos.y + 5.0f, pos.z + 0.1f);
		owner->GetVisualPreEffect()->SetPosition(pos.x, pos.y + 5.0f, pos.z - 0.1f);
		break;
	}
	case WEAPON_TYPE_2:
		if (currentFrame == 45) owner->CalculateHpCurrent(3.0f);
		// VisualPrePos
		owner->GetVisualPreEffectBg()->SetPosition(pos.x, pos.y + 1.25f, pos.z + 0.1f);
		owner->GetVisualPreEffect()->SetPosition(pos.x, pos.y + 1.25f, pos.z - 0.1f);
		break;
	case WEAPON_TYPE_3:
		// 攻撃コリジョンを開けるフレーム
		if (currentFrame == 17)
		{
			owner->SetAttackCollisionOnOff(true);
			owner->SetCollisionOnOff(false);	// コリジュンを閉じる
			owner->CalculateMoveVelocity((owner->GetIsFlip() ? -1.0f : 1.0f) * owner->GetMoveAccel() * 60.0f, 0.1f, 0.0f);
		}
		// 攻撃コリジョンを閉じるフレーム
		if (currentFrame == 43) owner->SetAttackCollisionOnOff(false);
		// 攻撃当たる判定
		if (owner->GetAttackCollisionOnOff()) owner->AttackCollide(-1.0f, { 0.5f,0.0f,1.0f });
		// VisualPrePos
		owner->GetVisualPreEffectBg()->SetPosition(pos.x, pos.y, pos.z + 0.1f);
		owner->GetVisualPreEffect()->SetPosition(pos.x, pos.y, pos.z - 0.1f);
		break;
	}

	// 移動
	owner->DoMove(0.1f);

	// 終わったらスキルフラグを閉じる
	if (owner->GetVisualPre()->ClipFinished()) owner->SetIsSkill(false);
}


/*------------------------------------------------------------------------------
   Player_HurtState
------------------------------------------------------------------------------*/
void Player_HurtState::OnEnter(Player* owner)
{
	// anime set
	owner->GetVisualPre()->SetFrameAuto(pack.numU, pack.numV, pack.FrameStart, pack.FrameTotalCount, 0, pack.FPS, !owner->GetIsFlip(), false);

	owner->SetCollisionOnOff(false); // コリジュンを閉じる

	owner->GetResourceBars()->ResetCombo();
}

void Player_HurtState::OnExit(Player* owner)
{
	owner->SetIsHurt(false); // DeathStateに遷移するかも、も一回IsHurtをリセットする
	owner->SetCollisionOnOff(true); // コリジュンを開ける
}

void Player_HurtState::Update(Player* owner, float dt)
{
	owner->GetVisualPre()->Update(dt);

	// 攻撃されたの中にデッシュと攻撃とスキルを事前入力する
	if (pack.FrameTotalCount - owner->GetVisualPre()->GetCurrentFrame() < 2)
	{
		if (!owner->GetIsDash() && !owner->GetIsAttack() && !owner->GetIsSkill())
		{
			if (owner->GetKbd()->KeyIsTriggered(KK_SPACE))										owner->SetIsDash(true);
			if (owner->GetMouse()->LeftIsTriggered() || owner->GetKbd()->KeyIsTriggered(KK_J))	owner->SetIsAttack(true);
			if (owner->GetMouse()->RightIsTriggered() || owner->GetKbd()->KeyIsTriggered(KK_K))
				if (owner->GetWeaponSlots()->GetCurrentSlot().GetType() != WEAPON_TYPE_0)		owner->SetIsSkill(true);
		}
	}

	// 攻撃をカウントダウン
	owner->DoAttackCountDown();

	// 攻撃された後は何するの判断
	if (owner->GetVisualPre()->ClipFinished())
	{
		// もし体力がゼロたら、死ぬ状態で遷移
		if (owner->GetHpCurrent() <= 0)	owner->SetIsDeath(true);
		// 終わったら攻撃されたフラグを閉じる
		else owner->SetIsHurt(false);
	}
}


/*------------------------------------------------------------------------------
   Player_DeathState
------------------------------------------------------------------------------*/
void Player_DeathState::OnEnter(Player* owner)
{
	// anime set
	owner->GetVisualPre()->SetFrameAuto(pack.numU, pack.numV, pack.FrameStart, pack.FrameTotalCount, 0, pack.FPS, !owner->GetIsFlip(), false);
}

void Player_DeathState::Update(Player* owner, float dt)
{
	owner->GetVisualPre()->Update(dt);
		
	// 終わったら何がする…
	if (owner->GetVisualPre()->ClipFinished())	owner->SetIsGameOver(true);
}


/*------------------------------------------------------------------------------
   Player_ChangeState
------------------------------------------------------------------------------*/
void Player_ChangeState::OnEnter(Player* owner)
{
	// anime set
	if (owner->GetIsFever())
		pack.FrameTotalCount = 48;
	else
	{
		owner->GetWeaponSlots()->DoChange();
		pack.FrameTotalCount = 48;
	}
	owner->GetVisualPre()->SetFrameAuto(pack.numU, pack.numV, pack.FrameStart, pack.FrameTotalCount, 4, pack.FPS, !owner->GetIsFlip(), false);
}

void Player_ChangeState::OnExit(Player* owner)
{
	if (owner->GetIsFever()) owner->SetFeverType(WEAPON_TYPE_0);
	else owner->SetFeverType(owner->GetWeaponSlots()->GetCanChange().second);

	owner->SetIsDash(false);
	owner->SetIsAttack(false);
	owner->SetIsSkill(false);
	owner->SetIsHurt(false);

	owner->GetWeaponSlots()->FinishChange();

	owner->SetIsFever(!owner->GetIsFever());
}

void Player_ChangeState::Update(Player* owner, float dt)
{
	owner->GetVisualPre()->Update(dt);

	//変身完了の判断
	if (owner->GetVisualPre()->ClipFinished())	owner->SetIsChange(false);
}