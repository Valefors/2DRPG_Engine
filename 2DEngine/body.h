class Body
{
    private:
        Hitbox* _hitbox;
        Sprite* _sprite;

    public:
        Body(iVec2 position, std::string texturePath, iVec2 spriteSize, iVec2 hitboxSize);
        ~Body();
};