#pragma once
#include "Graphics.h"
#include <memory>
#include "Transformation.h"

using namespace DirectX;

enum Object_Type_Tag
{
	Item_Type_None = 0x000,

	character_Player = 0x999,

	character_Enemy_Test = 0x001,
	character_Enemy_Test_Two = 0x002,
	character_Enemy_Test_Three = 0x003,
	character_Enemy_Boss = 0x004,

	environment_Block = 0x101,
	environment_BlockTiled = 0x102,
};

class ObjectBase
{
public:
	ObjectBase(Object_Type_Tag tag = Item_Type_None)
		:
		Tag(tag)
	{}
	virtual void OnEnable(void) = 0;
	virtual void Update(float dt) = 0;
	virtual void Submit(void) = 0;
	Object_Type_Tag GetTag(void) const		{ return Tag; }					// オブジェクト種類をチェット
	bool IsActive(void) const				{ return IsUse; }				// 使用状態をチェット
	void Activate()							{ IsUse = true;	OnEnable(); }	// 使用始まる
	void Deactivate()						{ IsUse = false; }				// 使用終わる
	XMFLOAT3 GetPosition(void) const		{ return transInfo.position; }
	XMFLOAT3 GetRotation(void) const		{ return transInfo.rotation; }
	XMFLOAT3 GetSize(void) const			{ return transInfo.scale; }
	XMFLOAT3 GetCollisionSize(void) const	{ return CollisionSize; }
	void SetCollisionOnOff(bool OnOff)		{ OnCollision = OnOff; }
	bool GetCollisionOnOff(void) const		{ return OnCollision; }
protected:
	void Transform(float X, float Y, float Z)	{ transInfo.position.x += X; transInfo.position.y += Y; transInfo.position.z += Z; }
	void SetPosition(XMFLOAT3 position)			{ transInfo.position = position; }
	void Rotation(float X, float Y, float Z)	{ transInfo.rotation.x += X; transInfo.rotation.y += Y; transInfo.rotation.z += Z; }
	void SetRotate(XMFLOAT3 rotate)				{ transInfo.rotation = rotate; }
	void Scale(float X, float Y, float Z)		{ transInfo.scale.x += X; transInfo.scale.y += Y; transInfo.scale.z += Z; }
	void SetSize(XMFLOAT3 size)					{ transInfo.scale = size; }
	void SetCollisionSize(XMFLOAT3 size)		{ CollisionSize = size; }
protected:
	bool IsUse{ true };
	
	Object_Type_Tag Tag{ Item_Type_None };
	
	TransInfo transInfo;
	
	XMFLOAT3 CollisionSize{ 0.0f, 0.0f,0.0f };
	bool OnCollision{ false };
};