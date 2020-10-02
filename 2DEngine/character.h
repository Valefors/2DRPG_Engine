class Character
{
    private:
        static iVec2 _position;
        static Sprite* _sprite;
        static Hitbox* _hitbox;
        static int _characterSpeed;
    
    public:
        static void Init(iVec2 position, iVec2 size, std::string texture, int speed);
        static void UpdateCharacter();
        static iVec2 GetPosition();
        static void SetPosition(iVec2 pos);
        static Sprite* GetSprite();
        static Hitbox* GetCollider();
};