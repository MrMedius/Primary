#pragma once
#include "Enemy.h"
#include <random>

class EnemyTestTwo_IdleState;
class EnemyTestTwo_ChaseState;
class EnemyTestTwo_AttackState;
class EnemyTestTwo_HurtState;
class EnemyTestTwo_DeathState;

class EnemyTestTwo : public Enemy
{
public:
	EnemyTestTwo(Graphics& gfx, Rgph::RenderGraph& rg, Object_Type_Tag tag, XMFLOAT3 position)
		:
		Enemy(tag)
	{
		// FSM state init
		FSM = std::make_unique<StateMachine<EnemyTestTwo>>(this);
		FSM->AddState(ENEMY_STATE[ENEMY_IDLE],	 std::make_unique<EnemyTestTwo_IdleState>());
		FSM->AddState(ENEMY_STATE[ENEMY_CHASE],  std::make_unique<EnemyTestTwo_ChaseState>());
		FSM->AddState(ENEMY_STATE[ENEMY_ATTACK], std::make_unique<EnemyTestTwo_AttackState>());
		FSM->AddState(ENEMY_STATE[ENEMY_HURT],	 std::make_unique<EnemyTestTwo_HurtState>());
		FSM->AddState(ENEMY_STATE[ENEMY_DEATH],  std::make_unique<EnemyTestTwo_DeathState>());

		// FSM state transitions init
		SetupTransitions();
		// init the state
		FSM->ChangeState(ENEMY_STATE[ENEMY_IDLE]);

		// parameters init
		SetPosition(position);
		SetSize({ 9.0f, 6.0f, 0.0f });
		SetCollisionSize({ 3.0f, 6.0f, 1.0f });
		SetCollisionOnOff(true);
		SetHpMax(5.0f);
		ResetHpCurrent();
		SetMoveAccel(0.025f);
		SetAttackCollisionSize({ 2.0f,3.0f,1.5f });
		SetAttackInterval(3.0f);
		SetEnemyType(ENEMY_TYPE_2);
		SetSearchArea({ 15.0f,4.0f,15.0f });

		// graphics init
		visualPre = std::make_unique<Sprite3D>(gfx, std::vector<std::string>{"asset\\Images\\EnemyTestTwo.png"});
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
	std::unique_ptr<StateMachine<EnemyTestTwo>> FSM;
};

/*------------------------------------------------------------------------------
   EnemyTestTwo_IdleState
------------------------------------------------------------------------------*/
class EnemyTestTwo_IdleState : public State<EnemyTestTwo>
{
public:
	void OnEnter(EnemyTestTwo* owner) override;
	void OnExit(EnemyTestTwo* owner) override {};
	void Update(EnemyTestTwo* owner) override;
	void Draw(EnemyTestTwo* owner) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_IDLE]; }
};

/*------------------------------------------------------------------------------
   EnemyTestTwo_ChaseState
------------------------------------------------------------------------------*/
class EnemyTestTwo_ChaseState : public State<EnemyTestTwo>
{
public:
	void OnEnter(EnemyTestTwo* owner) override;
	void OnExit(EnemyTestTwo* owner) override {};
	void Update(EnemyTestTwo* owner) override;
	void Draw(EnemyTestTwo* owner) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_CHASE]; }
};

/*------------------------------------------------------------------------------
   EnemyTestTwo_AttackState
------------------------------------------------------------------------------*/
class EnemyTestTwo_AttackState : public State<EnemyTestTwo>
{
public:
	void OnEnter(EnemyTestTwo* owner) override;
	void OnExit(EnemyTestTwo* owner) override;
	void Update(EnemyTestTwo* owner) override;
	void Draw(EnemyTestTwo* owner) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_ATTACK]; }
private:
	unsigned int FrameNoOld{ 0 };
	bool HaveHit{ false };
};

/*------------------------------------------------------------------------------
   EnemyTestTwo_HurtState
------------------------------------------------------------------------------*/
class EnemyTestTwo_HurtState : public State<EnemyTestTwo>
{
public:
	void OnEnter(EnemyTestTwo* owner) override;
	void OnExit(EnemyTestTwo* owner) override;
	void Update(EnemyTestTwo* owner) override;
	void Draw(EnemyTestTwo* owner) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_HURT]; }
private:
	Attack_Type_Tag BeAttackedTypeOld{ Attack_Type_None };
};

/*------------------------------------------------------------------------------
   EnemyTestTwo_DeathState
------------------------------------------------------------------------------*/
class EnemyTestTwo_DeathState : public State<EnemyTestTwo>
{
public:
	void OnEnter(EnemyTestTwo* owner) override;
	void OnExit(EnemyTestTwo* owner) override {};
	void Update(EnemyTestTwo* owner) override;
	void Draw(EnemyTestTwo* owner) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_DEATH]; }
};