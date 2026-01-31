#pragma once
#include "Enemy.h"
#include <random>

class EnemyTestThree_IdleState;
class EnemyTestThree_ChaseState;
class EnemyTestThree_AttackState;
class EnemyTestThree_HurtState;
class EnemyTestThree_DeathState;

class EnemyTestThree : public Enemy
{
public:
	EnemyTestThree(Graphics& gfx, Rgph::RenderGraph& rg, Object_Type_Tag tag, XMFLOAT3 position)
		:
		Enemy(tag)
	{
		// FSM state init
		FSM = std::make_unique<StateMachine<EnemyTestThree>>(this);
		FSM->AddState(ENEMY_STATE[ENEMY_IDLE],	 std::make_unique<EnemyTestThree_IdleState>());
		FSM->AddState(ENEMY_STATE[ENEMY_CHASE],  std::make_unique<EnemyTestThree_ChaseState>());
		FSM->AddState(ENEMY_STATE[ENEMY_ATTACK], std::make_unique<EnemyTestThree_AttackState>());
		FSM->AddState(ENEMY_STATE[ENEMY_HURT],	 std::make_unique<EnemyTestThree_HurtState>());
		FSM->AddState(ENEMY_STATE[ENEMY_DEATH],  std::make_unique<EnemyTestThree_DeathState>());

		// FSM state transitions init
		SetupTransitions();
		// init the state
		FSM->ChangeState(ENEMY_STATE[ENEMY_IDLE]);

		// parameters init
		SetPosition(position);
		SetSize({ 3.0f, 3.0f, 0.0f });
		SetCollisionSize({ 3.0f, 3.0f, 2.0f });
		SetCollisionOnOff(true);
		SetHpMax(3.0f);
		ResetHpCurrent();
		SetMoveAccel(0.035f);
		SetAttackCollisionSize({ 3.0f,2.0f,1.0f });
		SetAttackInterval(2.0f);
		SetEnemyType(ENEMY_TYPE_3);
		SetSearchArea({ 20.0f,6.0f,20.0f });

		// graphics init
		visualPre = std::make_unique<Sprite3D>(gfx, std::vector<std::string>{"asset\\Images\\EnemyTestThree.png"});
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
	std::unique_ptr<StateMachine<EnemyTestThree>> FSM;
};

/*------------------------------------------------------------------------------
   EnemyTestThree_IdleState
------------------------------------------------------------------------------*/
class EnemyTestThree_IdleState : public State<EnemyTestThree>
{
public:
	void OnEnter(EnemyTestThree* owner) override;
	void OnExit(EnemyTestThree* owner) override {};
	void Update(EnemyTestThree* owner) override;
	void Draw(EnemyTestThree* owner) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_IDLE]; }
};

/*------------------------------------------------------------------------------
   EnemyTestThree_ChaseState
------------------------------------------------------------------------------*/
class EnemyTestThree_ChaseState : public State<EnemyTestThree>
{
public:
	void OnEnter(EnemyTestThree* owner) override;
	void OnExit(EnemyTestThree* owner) override {};
	void Update(EnemyTestThree* owner) override;
	void Draw(EnemyTestThree* owner) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_CHASE]; }
};

/*------------------------------------------------------------------------------
   EnemyTestThree_AttackState
------------------------------------------------------------------------------*/
class EnemyTestThree_AttackState : public State<EnemyTestThree>
{
public:
	void OnEnter(EnemyTestThree* owner) override;
	void OnExit(EnemyTestThree* owner) override;
	void Update(EnemyTestThree* owner) override;
	void Draw(EnemyTestThree* owner) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_ATTACK]; }
private:
	unsigned int FrameNoOld{ 0 };
	bool HaveHit{ false };
};

/*------------------------------------------------------------------------------
   EnemyTestThree_HurtState
------------------------------------------------------------------------------*/
class EnemyTestThree_HurtState : public State<EnemyTestThree>
{
public:
	void OnEnter(EnemyTestThree* owner) override;
	void OnExit(EnemyTestThree* owner) override;
	void Update(EnemyTestThree* owner) override;
	void Draw(EnemyTestThree* owner) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_HURT]; }
private:
	Attack_Type_Tag BeAttackedTypeOld{ Attack_Type_None };
};

/*------------------------------------------------------------------------------
   EnemyTestThree_DeathState
------------------------------------------------------------------------------*/
class EnemyTestThree_DeathState : public State<EnemyTestThree>
{
public:
	void OnEnter(EnemyTestThree* owner) override;
	void OnExit(EnemyTestThree* owner) override {};
	void Update(EnemyTestThree* owner) override;
	void Draw(EnemyTestThree* owner) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_DEATH]; }
};