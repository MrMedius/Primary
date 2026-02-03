#pragma once
#include "Enemy.h"
#include <random>

class Enemy_Green_0_IdleState;
class Enemy_Green_0_ChaseState;
class Enemy_Green_0_AttackState;
class Enemy_Green_0_HurtState;
class Enemy_Green_0_DeathState;

class Enemy_Green_0 : public Enemy
{
public:
	Enemy_Green_0(Graphics& gfx, Rgph::RenderGraph& rg, Object_Type_Tag tag, XMFLOAT3 position)
		:
		Enemy(tag)
	{
		// parameters init
		SetPosition(position);
		SetSize({ 9.0f, 6.0f, 0.0f });
		SetCollisionSize({ 2.0f, 6.0f, 2.0f });
		SetCollisionOnOff(true);
		SetHpMax(5.0f);
		ResetHpCurrent();
		SetMoveAccel(0.025f);
		SetAttackCollisionSize({ 2.0f,3.0f,1.5f });
		SetAttackInterval(3.0f);
		SetEnemyType(ENEMY_TYPE_GREEN);
		SetSearchArea({ 15.0f,4.0f,15.0f });

		// graphics init
		visualPre = std::make_unique<Sprite3D>(gfx, std::vector<std::string>{"asset\\Images\\EnemyTestTwo.png"});
		visualPre->SetPosition(transInfo.position);
		visualPre->SetScale(transInfo.scale);
		visualPre->LinkTechniques(rg);

		// FSM state init
		FSM = std::make_unique<StateMachine<Enemy_Green_0>>(this);
		FSM->AddState(ENEMY_STATE[ENEMY_IDLE],	 std::make_unique<Enemy_Green_0_IdleState>());
		FSM->AddState(ENEMY_STATE[ENEMY_CHASE],  std::make_unique<Enemy_Green_0_ChaseState>());
		FSM->AddState(ENEMY_STATE[ENEMY_ATTACK], std::make_unique<Enemy_Green_0_AttackState>());
		FSM->AddState(ENEMY_STATE[ENEMY_HURT],	 std::make_unique<Enemy_Green_0_HurtState>());
		FSM->AddState(ENEMY_STATE[ENEMY_DEATH],  std::make_unique<Enemy_Green_0_DeathState>());

		// FSM state transitions init
		SetupTransitions();
		// init the state
		FSM->ChangeState(ENEMY_STATE[ENEMY_IDLE]);

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
	std::unique_ptr<StateMachine<Enemy_Green_0>> FSM;
};

/*------------------------------------------------------------------------------
   Enemy_Green_0_IdleState
------------------------------------------------------------------------------*/
class Enemy_Green_0_IdleState : public State<Enemy_Green_0>
{
public:
	void OnEnter(Enemy_Green_0* owner) override;
	void OnExit(Enemy_Green_0* owner) override {};
	void Update(Enemy_Green_0* owner, float dt) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_IDLE]; }
private:
	SpriteAnimeInfo pack{ 13, 5, 1, 8 };
};

/*------------------------------------------------------------------------------
   Enemy_Green_0_ChaseState
------------------------------------------------------------------------------*/
class Enemy_Green_0_ChaseState : public State<Enemy_Green_0>
{
public:
	void OnEnter(Enemy_Green_0* owner) override;
	void OnExit(Enemy_Green_0* owner) override {};
	void Update(Enemy_Green_0* owner, float dt) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_CHASE]; }
private:
	SpriteAnimeInfo pack{ 13, 5, 14, 10 };
};

/*------------------------------------------------------------------------------
   Enemy_Green_0_AttackState
------------------------------------------------------------------------------*/
class Enemy_Green_0_AttackState : public State<Enemy_Green_0>
{
public:
	void OnEnter(Enemy_Green_0* owner) override;
	void OnExit(Enemy_Green_0* owner) override;
	void Update(Enemy_Green_0* owner, float dt) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_ATTACK]; }
private:
	SpriteAnimeInfo pack{ 13, 5, 27, 10 };
	bool HaveHit{ false };
};

/*------------------------------------------------------------------------------
   Enemy_Green_0_HurtState
------------------------------------------------------------------------------*/
class Enemy_Green_0_HurtState : public State<Enemy_Green_0>
{
public:
	void OnEnter(Enemy_Green_0* owner) override;
	void OnExit(Enemy_Green_0* owner) override;
	void Update(Enemy_Green_0* owner, float dt) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_HURT]; }
private:
	SpriteAnimeInfo pack{ 13, 5, 40, 5 };
	Attack_Type_Tag BeAttackedTypeOld{ Attack_Type_None };
};

/*------------------------------------------------------------------------------
   Enemy_Green_0_DeathState
------------------------------------------------------------------------------*/
class Enemy_Green_0_DeathState : public State<Enemy_Green_0>
{
public:
	void OnEnter(Enemy_Green_0* owner) override;
	void OnExit(Enemy_Green_0* owner) override {};
	void Update(Enemy_Green_0* owner, float dt) override;
	std::string GetName() const override { return ENEMY_STATE[ENEMY_DEATH]; }
private:
	SpriteAnimeInfo pack{ 13, 5, 53, 13 };
};