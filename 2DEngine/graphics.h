class Graphics 
{
    private :
        static std::vector<Sprite*> spriteToDraw;
        static std::vector<Sprite*> backgroundToDraw;

    public:
        static void initGraphics();
        static void updateGraphics();
        static void addToBackgroundDrawList(Sprite* sprite);
        static void addToSortedDrawList(Sprite* sprite);
        static void removeFromBackgroundDrawList(Sprite* sprite);
        static void removeFromDrawList(Sprite* sprite);
        static unsigned int getTexture(std::string texturePath);
};