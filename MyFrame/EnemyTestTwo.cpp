#include "EnemyTestTwo.h"
#include "Channels.h"

void EnemyTestTwo::Update(float dt)
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

void EnemyTestTwo::Submit(void)
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

void EnemyTestTwo::SetupTransitions(void)
{
	// 状態の遷移条件を増加する
	// ENEMY_IDLE → ENEMY_CHASE
	FSM->AddTransition(ENEMY_STATE[ENEMY_IDLE], ENEMY_STATE[ENEMY_CHASE], [](Character* owner) {
		auto enemyTestTwo = static_cast<EnemyTestTwo*>(owner);
		return enemyTestTwo->IsInArea && !enemyTestTwo->IsInAttackArea; // 判断の条件
		});

	// ENEMY_IDLE → ENEMY_ATTACK
	FSM->AddTransition(ENEMY_STATE[ENEMY_IDLE], ENEMY_STATE[ENEMY_ATTACK], [](Character* owner) {
		auto enemyTestTwo = static_cast<EnemyTestTwo*>(owner);
		return enemyTestTwo->IsAttack;
		});

	// ENEMY_IDLE → ENEMY_HURT
	FSM->AddTransition(ENEMY_STATE[ENEMY_IDLE], ENEMY_STATE[ENEMY_HURT], [](Character* owner) {
		auto enemyTestTwo = static_cast<EnemyTestTwo*>(owner);
		return enemyTestTwo->IsHurt;
		});

	// ENEMY_CHASE → ENEMY_IDLE
	FSM->AddTransition(ENEMY_STATE[ENEMY_CHASE], ENEMY_STATE[ENEMY_IDLE], [](Character* owner) {
		auto enemyTestTwo = static_cast<EnemyTestTwo*>(owner);
		return (!enemyTestTwo->IsInArea) || (enemyTestTwo->IsInAttackArea && enemyTestTwo->AttackCountDown > 0);
		});

	// ENEMY_CHASE → ENEMY_ATTACK
	FSM->AddTransition(ENEMY_STATE[ENEMY_CHASE], ENEMY_STATE[ENEMY_ATTACK], [](Character* owner) {
		auto enemyTestTwo = static_cast<EnemyTestTwo*>(owner);
		return enemyTestTwo->IsAttack;
		});

	// ENEMY_CHASE → ENEMY_HURT
	FSM->AddTransition(ENEMY_STATE[ENEMY_CHASE], ENEMY_STATE[ENEMY_HURT], [](Character* owner) {
		auto enemyTestTwo = static_cast<EnemyTestTwo*>(owner);
		return enemyTestTwo->IsHurt;
		});

	// ENEMY_ATTACK → ENEMY_IDLE
	FSM->AddTransition(ENEMY_STATE[ENEMY_ATTACK], ENEMY_STATE[ENEMY_IDLE], [](Character* owner) {
		auto enemyTestTwo = static_cast<EnemyTestTwo*>(owner);
		return !enemyTestTwo->IsAttack;
		});

	// ENEMY_ATTACK → ENEMY_HURT
	FSM->AddTransition(ENEMY_STATE[ENEMY_ATTACK], ENEMY_STATE[ENEMY_HURT], [](Character* owner) {
		auto enemyTestTwo = static_cast<EnemyTestTwo*>(owner);
		return enemyTestTwo->IsHurt;
		});

	// ENEMY_HURT → ENEMY_IDLE
	FSM->AddTransition(ENEMY_STATE[ENEMY_HURT], ENEMY_STATE[ENEMY_IDLE], [](Character* owner) {
		auto enemyTestTwo = static_cast<EnemyTestTwo*>(owner);
		return !enemyTestTwo->IsHurt && !enemyTestTwo->IsDeath;
		});

	// ENEMY_HURT → ENEMY_DEATH
	FSM->AddTransition(ENEMY_STATE[ENEMY_HURT], ENEMY_STATE[ENEMY_DEATH], [](Character* owner) {
		auto enemyTestTwo = static_cast<EnemyTestTwo*>(owner);
		return enemyTestTwo->IsDeath;
		});
}


/*------------------------------------------------------------------------------
   EnemyTestTwo_IdleState
------------------------------------------------------------------------------*/
void EnemyTestTwo_IdleState::OnEnter(EnemyTestTwo* owner)
{
	// ポリゴンから開始時間を取って、アニメーションエラーを避ける
	startTime = std::chrono::steady_clock::now();
	FrameNo = 0;			// フレームを1から計算
	FrameTotalCount = 8;	// アクションのフレームの総数
	frameDuration = std::chrono::milliseconds(100);
}

void EnemyTestTwo_IdleState::Update(EnemyTestTwo* owner)
{
	// フレーム更新
	auto totalFramesPassed = (std::chrono::steady_clock::now() - startTime) / frameDuration;
	// 繰り返すのアクションはフレームを1からリセット
	FrameNo = (totalFramesPassed % FrameTotalCount + 1);

	owner->CheckIsFlip();		// 向いている方向の判断
	owner->CheckIsAttack();		// 攻撃するかどうかを判断
	owner->DoAttackCountDown();	// 攻撃をカウントダウン
}

void EnemyTestTwo_IdleState::Draw(EnemyTestTwo* owner)
{
	owner->GetVisualPre()->SetFrame(13, 5, 1, FrameTotalCount, FrameNo, 0, owner->GetIsFlip());
}


/*------------------------------------------------------------------------------
   EnemyTestTwo_ChaseState
------------------------------------------------------------------------------*/
void EnemyTestTwo_ChaseState::OnEnter(EnemyTestTwo* owner)
{
	startTime = std::chrono::steady_clock::now();
	FrameNo = 0;
	FrameTotalCount = 10;
	frameDuration = std::chrono::milliseconds(100);
}

void EnemyTestTwo_ChaseState::Update(EnemyTestTwo* owner)
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

void EnemyTestTwo_ChaseState::Draw(EnemyTestTwo* owner)
{
	owner->GetVisualPre()->SetFrame(13, 5, 14, FrameTotalCount, FrameNo, 0, owner->GetIsFlip());
}


/*------------------------------------------------------------------------------
   EnemyTestTwo_AttackState
------------------------------------------------------------------------------*/
void EnemyTestTwo_AttackState::OnEnter(EnemyTestTwo* owner)
{
	startTime = std::chrono::steady_clock::now();
	FrameNo = 0;
	FrameTotalCount = 10;
	frameDuration = std::chrono::milliseconds(100);

	FrameNoOld = 0;
	HaveHit = false;
}

void EnemyTestTwo_AttackState::OnExit(EnemyTestTwo* owner)
{
	owner->ResetAttackCountDown();			// 攻撃終わったら攻撃のカウンターをリセットする
	owner->SetAttackCollisionOnOff(false);	// もし攻撃の途中で攻撃されたら、こっちに攻撃フラグとコリジョンを閉じる
	owner->SetIsAttack(false);
}

void EnemyTestTwo_AttackState::Update(EnemyTestTwo* owner)
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

void EnemyTestTwo_AttackState::Draw(EnemyTestTwo* owner)
{
	owner->GetVisualPre()->SetFrame(13, 5, 27, FrameTotalCount, FrameNo, 0, owner->GetIsFlip());
}


/*------------------------------------------------------------------------------
   EnemyTestTwo_HurtState
------------------------------------------------------------------------------*/
void EnemyTestTwo_HurtState::OnEnter(EnemyTestTwo* owner)
{
	startTime = std::chrono::steady_clock::now();
	FrameNo = 0;
	FrameTotalCount = 5;
	frameDuration = std::chrono::milliseconds(100);

	BeAttackedTypeOld = owner->GetBeAttackedType();
}

void EnemyTestTwo_HurtState::OnExit(EnemyTestTwo* owner)
{
	owner->SetBeAttackedType(Attack_Type_None);
}

void EnemyTestTwo_HurtState::Update(EnemyTestTwo* owner)
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

void EnemyTestTwo_HurtState::Draw(EnemyTestTwo* owner)
{
	owner->GetVisualPre()->SetFrame(13, 5, 40, FrameTotalCount, FrameNo, 0, owner->GetIsFlip());
}


/*------------------------------------------------------------------------------
   EnemyTestTwo_DeathState
------------------------------------------------------------------------------*/
void EnemyTestTwo_DeathState::OnEnter(EnemyTestTwo* owner)
{
	startTime = std::chrono::steady_clock::now();
	FrameNo = 0;
	FrameTotalCount = 13;
	frameDuration = std::chrono::milliseconds(100);
}

void EnemyTestTwo_DeathState::Update(EnemyTestTwo* owner)
{
	FrameNo = (std::chrono::steady_clock::now() - startTime) / frameDuration + 1;
	// 終わったら使用終わるをセットする
	if (FrameNo > FrameTotalCount) owner->Deactivate();
}

void EnemyTestTwo_DeathState::Draw(EnemyTestTwo* owner)
{
	owner->GetVisualPre()->SetFrame(13, 5, 53, FrameTotalCount, FrameNo, 0, owner->GetIsFlip());
}