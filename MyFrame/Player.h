#pragma once
#include "Character.h"
#include "RenderGraph.h"
#include "ObjectCodex.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Sprite3D.h"
#include "Player_ResourceSystem.h"

// プレイヤー状態のID
enum PLAYER_STATE_ID {
	PLAYER_IDLE,
	PLAYER_MOVE,
	PLAYER_DASH,
	PLAYER_ATTACK,
	PLAYER_SKILL,
	PLAYER_HURT,
	PLAYER_DEATH,
	PLAYER_CHANGE,
};
// プレイヤー状態の名前
static const std::string PLAYER_STATE[] = {
	"PLAYER_IDLE",
	"PLAYER_MOVE",
	"PLAYER_DASH",
	"PLAYER_ATTACK",
	"PLAYER_SKILL",
	"PLAYER_HURT",
	"PLAYER_DEATH",
	"PLAYER_CHANGE",
};
class Player_IdleState;
class Player_MoveState;
class Player_DashState;
class Player_AttackState;
class Player_SkillState;
class Player_HurtState;
class Player_DeathState;
class Player_ChangeState;

class Player : public Character
{
public:
	Player(Graphics& gfx, Rgph::RenderGraph& rg, Keyboard* kbd, Mouse* mouse, Object_Type_Tag tag, XMFLOAT3 position)
		:
		Character(tag),
		pKbd(kbd),
		pMouse(mouse),
		visualPre(gfx, std::vector<std::string>{
		"asset\\Images\\Player\\Player_Usual.png",
		"asset\\Images\\Player\\Player_Red.png",
		"asset\\Images\\Player\\Player_Green.png",
		"asset\\Images\\Player\\Player_Blue.png",
		"asset\\Images\\Player\\Player_Fever.png"},std::to_string(tag))
	{
		// FSM state init
		FSM = std::make_unique<StateMachine<Player>>(this);
		FSM->AddState(PLAYER_STATE[PLAYER_IDLE],	std::make_unique<Player_IdleState>());
		FSM->AddState(PLAYER_STATE[PLAYER_MOVE],	std::make_unique<Player_MoveState>());
		FSM->AddState(PLAYER_STATE[PLAYER_DASH],	std::make_unique<Player_DashState>());
		FSM->AddState(PLAYER_STATE[PLAYER_ATTACK],	std::make_unique<Player_AttackState>());
		FSM->AddState(PLAYER_STATE[PLAYER_SKILL],	std::make_unique<Player_SkillState>());
		FSM->AddState(PLAYER_STATE[PLAYER_HURT],	std::make_unique<Player_HurtState>());
		FSM->AddState(PLAYER_STATE[PLAYER_DEATH],	std::make_unique<Player_DeathState>());
		FSM->AddState(PLAYER_STATE[PLAYER_CHANGE],	std::make_unique<Player_ChangeState>());

		// FSM state transitions
		SetupTransitions();
		// init the state
		FSM->ChangeState(PLAYER_STATE[PLAYER_IDLE]);

		// parameters init
		SetPosition(position);
		SetSize({ 10.0f, 10.0f, 0.0f });
		SetCollisionSize({ 5.0f, 8.0f, 1.0f });
		SetCollisionOnOff(true);
		SetHpMax(3.0f);
		ResetHpCurrent();
		SetMoveAccel(0.1f);
		SetAttackCollisionSize({ 10.0f,5.0f,2.0f });
		SetAttackInterval(1.5f);

		// game resources
		pWeapon = std::make_unique<WeaponSlots>();
		pResource = std::make_unique<ResourceBars>(pWeapon.get());

		// graphics init
		visualPre.SetPosition({ 0.0f,5.0f,6.0f });
		visualPre.SetScale(transInfo.scale);
		visualPre.LinkTechniques(rg);

		// collider init
#ifdef _DEBUG
		boxColliderWire = std::make_unique<CubeWireframe>(gfx, XMFLOAT3(1.0f, 0.0f, 0.0f));
		boxColliderWire->LinkTechniques(rg);
#endif
	}
	void OnEnable(void) override
	{
		SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
		FSM->ChangeState(PLAYER_STATE[PLAYER_IDLE]);
		ResetHpCurrent();
		SetIsDeath(false);
		SetIsGameOver(false);
		pResource->Reset();
		pWeapon->Reset();
	}
	void Update(float dt) override;
	void Submit(void) override;
	Keyboard* GetKbd(void) const { return pKbd; }
	Mouse* GetMouse(void) const { return pMouse; }
	float GetHpDrawParameter(void)
	{
		if (HpDraw != HpCurrent)
			HpDraw += (HpCurrent - HpDraw) * 0.2f;
		return HpDraw / HpMax;
	}
	ResourceBars* GetResourceBars(void) const	{ return pResource.get(); }
	WeaponSlots* GetWeaponSlots(void) const		{ return pWeapon.get(); }
	void SetIsDash(bool state)	{ IsDash = state; }
	bool GetIsDash(void) const	{ return IsDash; }
	void SetIsSkill(bool state)	{ IsSkill = state; }
	bool GetIsSkill(void) const { return IsSkill; }
	void SetIsChange(bool state) { IsChange = state; }
	bool GetIsChange(void) const { return IsChange; }
	void SetIsFever(bool state) { IsFever = state; }
	bool GetIsFever(void) const { return IsFever; }
	void SetFeverType(WEAPON_TYPE_ID type)	{ FeverType = type; }
	WEAPON_TYPE_ID GetFeverType(void) const { return FeverType; }
	void SetIsGameOver(bool state) { IsGameOver = state; }
	bool GetIsGameOver(void) const { return IsGameOver; }
	void DoMove(float ratio);
	bool AttackCollide(float damage, XMFLOAT3 repel) override;
	Sprite3D& GetVisualPre(void) { return visualPre; }
private:
	void SetupTransitions(void) override;
private:
	Sprite3D visualPre;
	Keyboard* pKbd;
	Mouse* pMouse;
	std::unique_ptr<StateMachine<Player>> FSM;
	std::unique_ptr<ResourceBars> pResource;
	std::unique_ptr<WeaponSlots> pWeapon;
	float HpDraw{ 0.0f };
	bool IsDash{ false };
	bool IsSkill{ false };
	bool IsChange{ false };
	bool IsFever{ false };
	WEAPON_TYPE_ID FeverType{ WEAPON_TYPE_0 };
	bool IsGameOver{ false };
};

/*------------------------------------------------------------------------------
   Player_IdleState
------------------------------------------------------------------------------*/
class Player_IdleState : public State<Player>
{
public:
	void OnEnter(Player* owner) override;
	void OnExit(Player* owner) override {};
	void Update(Player* owner) override;
	void Draw(Player* owner) override;
	std::string GetName() const override { return PLAYER_STATE[PLAYER_IDLE]; }
};

/*------------------------------------------------------------------------------
   Player_MoveState
------------------------------------------------------------------------------*/
class Player_MoveState : public State<Player>
{
public:
	void OnEnter(Player* owner) override;
	void OnExit(Player* owner) override {};
	void Update(Player* owner) override;
	void Draw(Player* owner) override;
	std::string GetName() const override { return PLAYER_STATE[PLAYER_MOVE]; }
};

/*------------------------------------------------------------------------------
   Player_DashState
------------------------------------------------------------------------------*/
class Player_DashState : public State<Player>
{
public:
	void OnEnter(Player* owner) override;
	void OnExit(Player* owner) override;
	void Update(Player* owner) override;
	void Draw(Player* owner) override;
	std::string GetName() const override { return PLAYER_STATE[PLAYER_DASH]; }
private:
	unsigned int FrameNoOld{ 0 };
	bool isInput{ false };
};

/*------------------------------------------------------------------------------
   Player_AttackState
------------------------------------------------------------------------------*/
class Player_AttackState : public State<Player>
{
public:
	void OnEnter(Player* owner) override;
	void OnExit(Player* owner) override;
	void Update(Player* owner) override;
	void Draw(Player* owner) override;
	std::string GetName() const override { return PLAYER_STATE[PLAYER_ATTACK]; }
private:
	unsigned int FrameNoOld{ 0 };
	bool isCombo{ false };
};

/*------------------------------------------------------------------------------
   Player_SkillState
------------------------------------------------------------------------------*/
class Player_SkillState : public State<Player>
{
public:
	void OnEnter(Player* owner) override;
	void OnExit(Player* owner) override;
	void Update(Player* owner) override;
	void Draw(Player* owner) override;
	std::string GetName() const override { return PLAYER_STATE[PLAYER_SKILL]; }
private:
	unsigned int FrameNoOld{ 0 };
	WEAPON_TYPE_ID WeaponType{ WEAPON_TYPE_NONE };
};

/*------------------------------------------------------------------------------
   Player_HurtState
------------------------------------------------------------------------------*/
class Player_HurtState : public State<Player>
{
public:
	void OnEnter(Player* owner) override;
	void OnExit(Player* owner) override;
	void Update(Player* owner) override;
	void Draw(Player* owner) override;
	std::string GetName() const override { return PLAYER_STATE[PLAYER_HURT]; }
};

/*------------------------------------------------------------------------------
   Player_DeathState
------------------------------------------------------------------------------*/
class Player_DeathState : public State<Player>
{
public:
	void OnEnter(Player* owner) override;
	void OnExit(Player* owner) override {};
	void Update(Player* owner) override;
	void Draw(Player* owner) override;
	std::string GetName() const override { return PLAYER_STATE[PLAYER_DEATH]; }
};

/*------------------------------------------------------------------------------
   Player_ChangeState
------------------------------------------------------------------------------*/
class Player_ChangeState : public State<Player>
{
public:
	void OnEnter(Player* owner) override;
	void OnExit(Player* owner) override;
	void Update(Player* owner) override;
	void Draw(Player* owner) override;
	std::string GetName() const override { return PLAYER_STATE[PLAYER_CHANGE]; }
};