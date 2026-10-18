#include "Player.h"
#include "core/Matrix4x4.h"
#include "core/global_consts.h"

void Player::Init()
{
    viewMat = Matrix4x4_CreateViewMatrix(pos, target, worldUp);
}

void Player::UpdateViewMat()
{
    viewMat = Matrix4x4_CreateViewMatrix(pos, target, worldUp);
}

void Player::Update(float xMov, float zMov, float xCamMov, float yCamMov)
{
    Vector3 xAx {viewMat.m11, viewMat.m21, viewMat.m31};
    Vector3 yAx {viewMat.m12, viewMat.m22, viewMat.m32};
    Vector3 zAx {viewMat.m13, viewMat.m23, viewMat.m33};
    Vector3 posChangeVec {xMov * xAx + zMov * zAx};
    Vector3 camChangeVec {posChangeVec + xCamMov * xAx + yCamMov * yAx};
    pos += posChangeVec;
    target += camChangeVec;
    UpdateViewMat();
}