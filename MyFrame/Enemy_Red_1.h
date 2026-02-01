#pragma once
#include "Enemy.h"
#include <random>

class Enemy_Red_1_IdleState;
class Enemy_Red_1_ChaseState;
class Enemy_Red_1_AttackState;
class Enemy_Red_1_HurtState;
class Enemy_Red_1_DeathState;

class Enemy_Red_1 : public Enemy
{
public:
	Enemy_Red_1(Graphics& gfx, Rgph::RenderGraph& rg, Object_Type_Tag tag, XMFLOAT3 position)
		:
		Enemy(tag)
	{
		// parameters init
		SetPosition(position);
		SetSize({ 8.0f, 8.0f, 0.0f });
		SetCollisionSize({ 3.0f, 8.0f, 1.5f });
		SetCollisionOnOff(true);
		SetHpMax(10.0f);
		ResetHpCurrent();
		SetMoveAccel(0.015f);
		SetAttackCollisionSize({ 3.0f,8.0f,2.0f });
		SetAttackInterval(2.0f);
		SetEnemyType(ENEMY_TYPE_RED);
		SetSearchArea({ 200.0f,10.0f,200.0f });

		// graphics init
		visualPre = std::make_unique<Sprite3D>(gfx, std::vector<std::string>{"asset\\Images\\\Enemy\\Enemy_Red_1.png"});
		visualPre->SetPosition(transInfo.position);
		visualPre->SetScale(transInfo.scale);
		visualPre->LinkTechniques(rg);

		// FSM state init
		FSM = std::make_unique<StateMachine<Enemy_Red_1>>(this);
		FSM->AddState(ENEMY_STATE[ENEMY_IDLE],	 std::make_unique<Enemy_Red_1_IdleState>());
		FSM->AddState(ENEMY_STATE[ENEMY_CHASE],  std::make_unique<Enemy_Red_1_ChaseState>());
		FSM->AddState(ENEMY_STATE[ENEMY_ATTACK], std::make_unique<Enemy_Red_1_AttackState>());
		FSM->AddState(ENEMY_STATE[ENEMY_HURT],	 std::make_unique<Enemy_Red_1_HurtState>());
		FSM->AddState(ENEMY_STATE[ENEMY_DEATH],  std::make_unique<Enemy_Red_1_DeathState>());

		// FSM state transitions init
		SetupTransitions();
		// init the state
		FSM->ChangeState(ENEMY_STATE[ENEMY_IDLE]);

		// collider init
#ifdef _DEBUG
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

		FSM->ChangeState(ENEMY_STATE[ENEMY_IDLE]);
		SetPosition(pos);
		ResetHpCurrent();
		SetIsDeath(false);
	};
	void Update(float dt) override;
	void Submit(void) override;
private:
	void SetupTransitions(void) override;
private:
	std::unique_ptr<StateMachine<Enemy_Red_1>> FSM;
};

/*------------------------------------------------------------------------------
   Enemy_Red_1_IdleState
------------------------------------------------------------------------------*/
class Enemy_Red_1_IdleState : public State<Enemy_Red_1>
{
public:
	void OnEnter(Enemy_Red_1* owner) override;
	void OnExit(Enemy_Red_1* owner) override {};
	void Update(Enemy_Red_1* owner, float dt) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_IDLE]; }
private:
	SpriteAnimeInfo pack{ 12, 10, 73, 24 };
};

/*------------------------------------------------------------------------------
   Enemy_Red_1_ChaseState
------------------------------------------------------------------------------*/
class Enemy_Red_1_ChaseState : public State<Enemy_Red_1>
{
public:
	void OnEnter(Enemy_Red_1* owner) override;
	void OnExit(Enemy_Red_1* owner) override {};
	void Update(Enemy_Red_1* owner, float dt) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_CHASE]; }
private:
	SpriteAnimeInfo pack{ 12, 10, 97, 24 };
};

/*------------------------------------------------------------------------------
   Enemy_Red_1_AttackState
------------------------------------------------------------------------------*/
class Enemy_Red_1_AttackState : public State<Enemy_Red_1>
{
public:
	void OnEnter(Enemy_Red_1* owner) override;
	void OnExit(Enemy_Red_1* owner) override;
	void Update(Enemy_Red_1* owner, float dt) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_ATTACK]; }
private:
	SpriteAnimeInfo pack{ 12, 10, 1, 24 };
	unsigned int FrameNoOld{ 0 };
	bool HaveHit{ false };
	XMFLOAT3 AttackVec{ 0.0f, 0.0f, 0.0f };
};

/*------------------------------------------------------------------------------
   Enemy_Red_1_HurtState
------------------------------------------------------------------------------*/
class Enemy_Red_1_HurtState : public State<Enemy_Red_1>
{
public:
	void OnEnter(Enemy_Red_1* owner) override;
	void OnExit(Enemy_Red_1* owner) override;
	void Update(Enemy_Red_1* owner, float dt) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_HURT]; }
private:
	SpriteAnimeInfo pack{ 12, 10, 25, 24 };
	Attack_Type_Tag BeAttackedTypeOld{ Attack_Type_None };
};

/*------------------------------------------------------------------------------
   Enemy_Red_1_DeathState
------------------------------------------------------------------------------*/
class Enemy_Red_1_DeathState : public State<Enemy_Red_1>
{
public:
	void OnEnter(Enemy_Red_1* owner) override;
	void OnExit(Enemy_Red_1* owner) override {};
	void Update(Enemy_Red_1* owner, float dt) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_DEATH]; }
private:
	SpriteAnimeInfo pack{ 12, 10, 49, 24 };
};