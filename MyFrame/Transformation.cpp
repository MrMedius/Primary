#include "Transformation.h"
#include "Math.h"

namespace dx = DirectX;

void Transformation::SetTranslation(float x, float y, float z) noexcept
{
    position = { x,y,z };
}

void Transformation::SetRotationRad(float roll, float pitch, float yaw) noexcept
{
    rotation = { roll, pitch, yaw };
}

void Transformation::SetRotationDegreeToRad(float roll, float pitch, float yaw) noexcept
{
    rotation = { wrap_angle(rotation.x),
                 wrap_angle(rotation.y),
                 wrap_angle(rotation.z) };
}

void Transformation::SetScale(float x, float y, float z) noexcept
{
    scale = { x,y,z };
}

void Transformation::SetScale(float size) noexcept
{
    SetScale(size, size, size);
}

void Transformation::Translate(float x, float y, float z) noexcept
{
    position.x += x;
    position.y += y;
    position.z += z;
}

void Transformation::RotateRad(float roll, float pitch, float yaw) noexcept
{
    rotation.x += roll;
    rotation.y += pitch;
    rotation.z += yaw;
}

void Transformation::RotateDegreeToRad(float roll, float pitch, float yaw) noexcept
{
    rotation.x += to_rad(roll);
    rotation.y += to_rad(pitch);
    rotation.z += to_rad(yaw);
}

void Transformation::Scale(float x, float y, float z) noexcept
{
    scale.x *= x;
    scale.y *= y;
    scale.z *= z;
}

void Transformation::Scale(float size) noexcept
{
    Scale(size, size, size);
}

dx::XMMATRIX Transformation::GetTransformXM() const noexcept
{
    return
        dx::XMMatrixScaling(scale.x, scale.y, scale.z) *
        dx::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *
        dx::XMMatrixTranslation(position.x, position.y, position.z);
}

dx::XMFLOAT3 Transformation::GetPosition() const noexcept
{
    return position;
}
dx::XMFLOAT3 Transformation::GetRotation() const noexcept
{
    return
    {
        wrap_angle(rotation.x),
        wrap_angle(rotation.y),
        wrap_angle(rotation.z)
    };
}
dx::XMFLOAT3 Transformation::GetScale() const noexcept
{
    return scale;
}