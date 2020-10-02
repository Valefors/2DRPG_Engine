#include "maths.h"

iVec2 operator+(const iVec2& a, const iVec2& b)
{
    return {a.x + b.x, a.y + b.y};
}

iVec2 operator-(const iVec2& a, const iVec2& b)
{
    return {a.x - b.x, a.y - b.y};
}

iVec2 operator*(const iVec2& a, const iVec2& b)
{
    return {a.x * b.x, a.y * b.y};
}

iVec2 operator/(const iVec2& a, const iVec2& b)
{
    return {a.x / b.x, a.y / b.y};
}

mat4::mat4()
:e00 (1.0f), e10 (0.0f), e20 (0.0f), e30 (0.0f),
 e01 (0.0f), e11 (1.0f), e21 (0.0f), e31 (0.0f),
 e02 (0.0f), e12 (0.0f), e22 (1.0f), e32 (0.0f),
 e03 (0.0f), e13 (0.0f), e23 (0.0f), e33 (1.0f)
 {}

 mat4 mat4::GetOrthogonalProjectionMat4(float width, float height)
 {
    mat4 orthoMat4;

    orthoMat4.e00 = 2.0f / width;
    orthoMat4.e11 = 2.0f / -height;
    orthoMat4.e22 = 0.0f;
    orthoMat4.e03 = -1.0f;
    orthoMat4.e13 = 1.0f;
    
    return orthoMat4;
 }

 void mat4::Translate(const iVec2& translationVec)
 {
    e03 += translationVec.x * e00;
    e13 += translationVec.y * e11;

 }

  void mat4::Scale(const iVec2& scaleVec)
 {
    e00 *= scaleVec.x;
    e11 *= scaleVec.y;
 }