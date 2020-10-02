class Map 
{
    private:
        static Sprite** _backgroundmap; //Array of sprites for background
        static void loadTileMap();
        static void loadObjectMap();
        
    public:
        static void init();
};