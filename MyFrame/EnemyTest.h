#pragma once
#include "Enemy.h"
#include <random>

class EnemyTest_IdleState;
class EnemyTest_ChaseState;
class EnemyTest_AttackState;
class EnemyTest_HurtState;
class EnemyTest_DeathState;

class EnemyTest : public Enemy
{
public:
	EnemyTest(Graphics& gfx, Rgph::RenderGraph& rg, Object_Type_Tag tag, XMFLOAT3 position)
		:
		Enemy(tag)
	{
		// FSM state init
		FSM = std::make_unique<StateMachine<EnemyTest>>(this);
		FSM->AddState(ENEMY_STATE[ENEMY_IDLE],	 std::make_unique<EnemyTest_IdleState>());
		FSM->AddState(ENEMY_STATE[ENEMY_CHASE],  std::make_unique<EnemyTest_ChaseState>());
		FSM->AddState(ENEMY_STATE[ENEMY_ATTACK], std::make_unique<EnemyTest_AttackState>());
		FSM->AddState(ENEMY_STATE[ENEMY_HURT],	 std::make_unique<EnemyTest_HurtState>());
		FSM->AddState(ENEMY_STATE[ENEMY_DEATH],  std::make_unique<EnemyTest_DeathState>());

		// FSM state transitions init
		SetupTransitions();
		// init the state
		FSM->ChangeState(ENEMY_STATE[ENEMY_IDLE]);

		// parameters init
		SetPosition(position);
		SetSize({ 9.0f, 6.0f, 0.0f });
		SetCollisionSize({ 3.0f, 6.0f, 1.0f });
		SetCollisionOnOff(true);
		SetHpMax(8.0f);
		ResetHpCurrent();
		SetMoveAccel(0.01f);
		SetAttackCollisionSize({ 3.0f,2.0f,1.0f });
		SetAttackInterval(2.0f);
		SetEnemyType(ENEMY_TYPE_1);
		SetSearchArea({ 20.0f,4.0f,10.0f });

		// graphics init
		visualPre = std::make_unique<Sprite3D>(gfx, std::vector<std::string>{"asset\\Images\\EnemyTest.png"}, std::to_string(tag));
		visualPre->SetPosition(transInfo.position);
		visualPre->SetScale(transInfo.scale);
		visualPre->LinkTechniques(rg);

#ifdef _DEBUG
		// collider wireframe init
		boxColliderWire = std::make_unique<CubeWireframe>(gfx, XMFLOAT3{ 1.0f, 0.0f, 0.0f }, "wireBox");
		boxColliderWire->LinkTechniques(rg);
		searchColliderWire = std::make_unique<CubeWireframe>(gfx, XMFLOAT3{ 0.0f, 1.0f, 0.0f }, "wireSearch");
		searchColliderWire->LinkTechniques(rg);
		attackColliderWire = std::make_unique<CubeWireframe>(gfx, XMFLOAT3{ 0.0f, 0.0f, 1.0f }, "wireAttack");
		attackColliderWire->LinkTechniques(rg);
#endif
	}
	void OnEnable(void) override
	{
		std::mt19937 rng(std::random_device{}());
		std::uniform_int_distribution<int> d(-20, 20);
		XMFLOAT3 pos = { AttackTarget->GetPosition().x + d(rng),10.0f,AttackTarget->GetPosition().z + d(rng) };

		SetPosition(pos);
		FSM->ChangeState(ENEMY_STATE[ENEMY_IDLE]);
		ResetHpCurrent();
		SetIsDeath(false);
	};
	void Update(float dt) override;
	void Submit(void) override;
private:
	void SetupTransitions(void) override;
private:
	std::unique_ptr<StateMachine<EnemyTest>> FSM;
};

/*------------------------------------------------------------------------------
   EnemyTest_IdleState
------------------------------------------------------------------------------*/
class EnemyTest_IdleState : public State<EnemyTest>
{
public:
	void OnEnter(EnemyTest* owner) override;
	void OnExit(EnemyTest* owner) override {};
	void Update(EnemyTest* owner) override;
	void Draw(EnemyTest* owner) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_IDLE]; }
};

/*------------------------------------------------------------------------------
   EnemyTest_ChaseState
------------------------------------------------------------------------------*/
class EnemyTest_ChaseState : public State<EnemyTest>
{
public:
	void OnEnter(EnemyTest* owner) override;
	void OnExit(EnemyTest* owner) override {};
	void Update(EnemyTest* owner) override;
	void Draw(EnemyTest* owner) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_CHASE]; }
};

/*------------------------------------------------------------------------------
   EnemyTest_AttackState
------------------------------------------------------------------------------*/
class EnemyTest_AttackState : public State<EnemyTest>
{
public:
	void OnEnter(EnemyTest* owner) override;
	void OnExit(EnemyTest* owner) override;
	void Update(EnemyTest* owner) override;
	void Draw(EnemyTest* owner) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_ATTACK]; }
private:
	unsigned int FrameNoOld{ 0 };
	bool HaveHit{ false };
};

/*------------------------------------------------------------------------------
   EnemyTest_HurtState
------------------------------------------------------------------------------*/
class EnemyTest_HurtState : public State<EnemyTest>
{
public:
	void OnEnter(EnemyTest* owner) override;
	void OnExit(EnemyTest* owner) override;
	void Update(EnemyTest* owner) override;
	void Draw(EnemyTest* owner) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_HURT]; }
private:
	Attack_Type_Tag BeAttackedTypeOld{ Attack_Type_None };
};

/*------------------------------------------------------------------------------
   EnemyTest_DeathState
------------------------------------------------------------------------------*/
class EnemyTest_DeathState : public State<EnemyTest>
{
public:
	void OnEnter(EnemyTest* owner) override;
	void OnExit(EnemyTest* owner) override {};
	void Update(EnemyTest* owner) override;
	void Draw(EnemyTest* owner) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_DEATH]; }
};