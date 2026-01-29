#include "EnemyTestThree.h"
#include "Channels.h"

void EnemyTestThree::Update(float dt)
{
	// 変更前の座標を格納
	auto Position = transInfo.position;
	PositionOld = Position;
	searchCollider.center = Position;
	CheckIsInArea();

	// 攻撃範囲の更新
	if(IsInArea)
	{
		if (!IsFlip) attackCollider.center = { Position.x + boxCollider.half.x + attackCollider.half.x, Position.y, Position.z };
		else attackCollider.center = { Position.x - boxCollider.half.x - attackCollider.half.x, Position.y, Position.z };
		CheckIsInAttackArea();
	}

	// 状態遷移
	FSM->Update();

	// 重力
	if (!OnFloor) MoveVelocity.y -= GRAVITY * dt;

	// 抵抗力
	MoveVelocity.x -= MoveVelocity.x * FORCE_RATE;
	MoveVelocity.z -= MoveVelocity.z * FORCE_RATE;

	// 移動
	Transform(MoveVelocity.x, MoveVelocity.y, MoveVelocity.z);
	visualPre->SetPosition(transInfo.position);

	// マップ要素との当たり判定
	boxCollider.center = transInfo.position;
	MapItemCollide();
}

void EnemyTestThree::Submit(void)
{
	// 描画
	FSM->Draw();

	visualPre->Submit(Chan::main);
	visualPre->Submit(Chan::shadow);

#ifdef _DEBUG
	if (!IsDeath)
	{
		boxColliderWire->DoSubmit(transInfo.position, boxCollider.GetSize());
		searchColliderWire->DoSubmit(transInfo.position, searchCollider.GetSize());
		if (IsInArea) attackColliderWire->DoSubmit(attackCollider.center, attackCollider.GetSize());
	}
#endif
}

void EnemyTestThree::SetupTransitions(void)
{
	// 状態の遷移条件を増加する
	// ENEMY_IDLE → ENEMY_CHASE
	FSM->AddTransition(ENEMY_STATE[ENEMY_IDLE], ENEMY_STATE[ENEMY_CHASE], [](Character* owner) {
		auto enemyTestThree = static_cast<EnemyTestThree*>(owner);
		return enemyTestThree->IsInArea && !enemyTestThree->IsInAttackArea; // 判断の条件
		});

	// ENEMY_IDLE → ENEMY_ATTACK
	FSM->AddTransition(ENEMY_STATE[ENEMY_IDLE], ENEMY_STATE[ENEMY_ATTACK], [](Character* owner) {
		auto enemyTestThree = static_cast<EnemyTestThree*>(owner);
		return enemyTestThree->IsAttack;
		});

	// ENEMY_IDLE → ENEMY_HURT
	FSM->AddTransition(ENEMY_STATE[ENEMY_IDLE], ENEMY_STATE[ENEMY_HURT], [](Character* owner) {
		auto enemyTestThree = static_cast<EnemyTestThree*>(owner);
		return enemyTestThree->IsHurt;
		});

	// ENEMY_CHASE → ENEMY_IDLE
	FSM->AddTransition(ENEMY_STATE[ENEMY_CHASE], ENEMY_STATE[ENEMY_IDLE], [](Character* owner) {
		auto enemyTestThree = static_cast<EnemyTestThree*>(owner);
		return (!enemyTestThree->IsInArea) || (enemyTestThree->IsInAttackArea && enemyTestThree->AttackCountDown > 0);
		});

	// ENEMY_CHASE → ENEMY_ATTACK
	FSM->AddTransition(ENEMY_STATE[ENEMY_CHASE], ENEMY_STATE[ENEMY_ATTACK], [](Character* owner) {
		auto enemyTestThree = static_cast<EnemyTestThree*>(owner);
		return enemyTestThree->IsAttack;
		});

	// ENEMY_CHASE → ENEMY_HURT
	FSM->AddTransition(ENEMY_STATE[ENEMY_CHASE], ENEMY_STATE[ENEMY_HURT], [](Character* owner) {
		auto enemyTestThree = static_cast<EnemyTestThree*>(owner);
		return enemyTestThree->IsHurt;
		});

	// ENEMY_ATTACK → ENEMY_IDLE
	FSM->AddTransition(ENEMY_STATE[ENEMY_ATTACK], ENEMY_STATE[ENEMY_IDLE], [](Character* owner) {
		auto enemyTestThree = static_cast<EnemyTestThree*>(owner);
		return !enemyTestThree->IsAttack;
		});

	// ENEMY_ATTACK → ENEMY_HURT
	FSM->AddTransition(ENEMY_STATE[ENEMY_ATTACK], ENEMY_STATE[ENEMY_HURT], [](Character* owner) {
		auto enemyTestThree = static_cast<EnemyTestThree*>(owner);
		return enemyTestThree->IsHurt;
		});

	// ENEMY_HURT → ENEMY_IDLE
	FSM->AddTransition(ENEMY_STATE[ENEMY_HURT], ENEMY_STATE[ENEMY_IDLE], [](Character* owner) {
		auto enemyTestThree = static_cast<EnemyTestThree*>(owner);
		return !enemyTestThree->IsHurt && !enemyTestThree->IsDeath;
		});

	// ENEMY_HURT → ENEMY_DEATH
	FSM->AddTransition(ENEMY_STATE[ENEMY_HURT], ENEMY_STATE[ENEMY_DEATH], [](Character* owner) {
		auto enemyTestThree = static_cast<EnemyTestThree*>(owner);
		return enemyTestThree->IsDeath;
		});
}


/*------------------------------------------------------------------------------
   EnemyTestThree_IdleState
------------------------------------------------------------------------------*/
void EnemyTestThree_IdleState::OnEnter(EnemyTestThree* owner)
{
	// ポリゴンから開始時間を取って、アニメーションエラーを避ける
	startTime = std::chrono::steady_clock::now();
	FrameNo = 0;			// フレームを1から計算
	FrameTotalCount = 4;	// アクションのフレームの総数
	frameDuration = std::chrono::milliseconds(100);
}

void EnemyTestThree_IdleState::Update(EnemyTestThree* owner)
{
	// フレーム更新
	auto totalFramesPassed = (std::chrono::steady_clock::now() - startTime) / frameDuration;
	// 繰り返すのアクションはフレームを1からリセット
	FrameNo = (totalFramesPassed % FrameTotalCount + 1);

	owner->CheckIsFlip();		// 向いている方向の判断
	owner->CheckIsAttack();		// 攻撃するかどうかを判断
	owner->DoAttackCountDown();	// 攻撃をカウントダウン
}

void EnemyTestThree_IdleState::Draw(EnemyTestThree* owner)
{
	owner->GetVisualPre()->SetFrame(8, 5, 1, FrameTotalCount, FrameNo, 0, !owner->GetIsFlip());
}


/*------------------------------------------------------------------------------
   EnemyTestThree_ChaseState
------------------------------------------------------------------------------*/
void EnemyTestThree_ChaseState::OnEnter(EnemyTestThree* owner)
{
	startTime = std::chrono::steady_clock::now();
	FrameNo = 0;
	FrameTotalCount = 4;
	frameDuration = std::chrono::milliseconds(100);
}

void EnemyTestThree_ChaseState::Update(EnemyTestThree* owner)
{
	// フレーム更新
	auto totalFramesPassed = (std::chrono::steady_clock::now() - startTime) / frameDuration;
	// 繰り返すのアクションはフレームを1からリセット
	FrameNo = (totalFramesPassed % FrameTotalCount + 1);

	owner->CheckIsFlip();		// 向いている方向の判断
	owner->CheckIsAttack();		// 攻撃するかどうかを判断
	owner->DoChase();			// ターゲットを追いかける
	owner->DoAttackCountDown();	// 攻撃をカウントダウン
}

void EnemyTestThree_ChaseState::Draw(EnemyTestThree* owner)
{
	owner->GetVisualPre()->SetFrame(8, 5, 9, FrameTotalCount, FrameNo, 0, !owner->GetIsFlip());
}


/*------------------------------------------------------------------------------
   EnemyTestThree_AttackState
------------------------------------------------------------------------------*/
void EnemyTestThree_AttackState::OnEnter(EnemyTestThree* owner)
{
	startTime = std::chrono::steady_clock::now();
	FrameNo = 0;
	FrameTotalCount = 10;
	frameDuration = std::chrono::milliseconds(100);

	FrameNoOld = 0;
	HaveHit = false;
}

void EnemyTestThree_AttackState::OnExit(EnemyTestThree* owner)
{
	owner->ResetAttackCountDown();			// 攻撃終わったら攻撃のカウンターをリセットする
	owner->SetAttackCollisionOnOff(false);	// もし攻撃の途中で攻撃されたら、こっちに攻撃フラグとコリジョンを閉じる
	owner->SetIsAttack(false);
}

void EnemyTestThree_AttackState::Update(EnemyTestThree* owner)
{
	FrameNo = (std::chrono::steady_clock::now() - startTime) / frameDuration + 1;
	// 終わったら攻撃フラグを閉じる
	if (FrameNo > FrameTotalCount) owner->SetIsAttack(false);

	// 攻撃コリジョンを開けるフレーム
	if (FrameNoOld != FrameNo && FrameNo == 6) owner->SetAttackCollisionOnOff(true);
	// 攻撃コリジョンを閉じるフレーム
	if (FrameNoOld != FrameNo && FrameNo == 8) owner->SetAttackCollisionOnOff(false);
	// 攻撃当たる判定
	if (owner->GetAttackCollisionOnOff() && !HaveHit) HaveHit = owner->AttackCollide(-1.0f, { 0.5f ,0.1f,0.5f });

	// 変更前の座標を更新
	FrameNoOld = FrameNo;
}

void EnemyTestThree_AttackState::Draw(EnemyTestThree* owner)
{
	owner->GetVisualPre()->SetFrame(8, 5, 17, FrameTotalCount, FrameNo, 0, !owner->GetIsFlip());
}


/*------------------------------------------------------------------------------
   EnemyTestThree_HurtState
------------------------------------------------------------------------------*/
void EnemyTestThree_HurtState::OnEnter(EnemyTestThree* owner)
{
	startTime = std::chrono::steady_clock::now();
	FrameNo = 0;
	FrameTotalCount = 5;
	frameDuration = std::chrono::milliseconds(100);

	BeAttackedTypeOld = owner->GetBeAttackedType();
}

void EnemyTestThree_HurtState::OnExit(EnemyTestThree* owner)
{
	owner->SetBeAttackedType(Attack_Type_None);
}

void EnemyTestThree_HurtState::Update(EnemyTestThree* owner)
{
	FrameNo = (std::chrono::steady_clock::now() - startTime) / frameDuration + 1;
	// 攻撃された後は何するの判断
	if (FrameNo > FrameTotalCount)
	{
		// もし体力がゼロたら、死ぬ状態で遷移
		if (owner->GetHpCurrent() == 0)	owner->SetIsDeath(true);
		// 終わったら攻撃されたフラグを閉じる
		else owner->SetIsHurt(false);
	}

	if (owner->GetBeAttackedType() != BeAttackedTypeOld)
	{
		FrameNo = 1;
		BeAttackedTypeOld = owner->GetBeAttackedType();
	}

	// 攻撃をカウントダウン
	owner->DoAttackCountDown();
}

void EnemyTestThree_HurtState::Draw(EnemyTestThree* owner)
{
	owner->GetVisualPre()->SetFrame(8, 5, 25, FrameTotalCount, FrameNo, 0, !owner->GetIsFlip());
}


/*------------------------------------------------------------------------------
   EnemyTestThree_DeathState
------------------------------------------------------------------------------*/
void EnemyTestThree_DeathState::OnEnter(EnemyTestThree* owner)
{
	startTime = std::chrono::steady_clock::now();
	FrameNo = 0;
	FrameTotalCount = 6;
	frameDuration = std::chrono::milliseconds(100);
}

void EnemyTestThree_DeathState::Update(EnemyTestThree* owner)
{
	FrameNo = (std::chrono::steady_clock::now() - startTime) / frameDuration + 1;
	// 終わったら使用終わるをセットする
	if (FrameNo > FrameTotalCount) owner->Deactivate();
}

void EnemyTestThree_DeathState::Draw(EnemyTestThree* owner)
{
	owner->GetVisualPre()->SetFrame(8, 5, 33, FrameTotalCount, FrameNo, 0, !owner->GetIsFlip());
}