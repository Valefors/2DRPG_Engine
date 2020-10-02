class Physics
{
    private:
        static std::vector<Hitbox*> hitboxList;
        static Hitbox* _characterHitbox;

    public:
        static void init();
        static bool isCollision(Hitbox* h1, Hitbox* h2);
        static void addToHitboxList(Hitbox* hitbox);
        static void removeFromHitboxList(Hitbox* hitbox);
        static void updatePhysics();
        static void correctCharacterPosition(Hitbox* collidedHitbox);
};