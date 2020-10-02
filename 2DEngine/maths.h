struct iVec2
{
    int x;
    int y;
};

iVec2 operator+(const iVec2& a, const iVec2& b);
iVec2 operator-(const iVec2& a, const iVec2& b);
iVec2 operator*(const iVec2& a, const iVec2& b);
iVec2 operator/(const iVec2& a, const iVec2& b);

//Not an array => need calculs to access the correct value
//Not a dynamical array => rows - columns aren't in the same way as a matrice
//Struct : values are stocked next to each other in memory
struct mat4
{
    float e00; float e10; float e20; float e30;
    float e01; float e11; float e21; float e31;
    float e02; float e12; float e22; float e32;
    float e03; float e13; float e23; float e33;

    mat4();

    static mat4 GetOrthogonalProjectionMat4(float width, float height);

    void Translate(const iVec2& translationVec);
    void Scale(const iVec2& scaleVec);
};

