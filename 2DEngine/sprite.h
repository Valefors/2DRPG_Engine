class Sprite
{
    private :
        iVec2 _position;
        iVec2 _size;
        unsigned int _textureID;
        bool _isBackground;

    public :
        Sprite(iVec2 position, iVec2 size, std::string textureFilePath, bool isBackground);
        ~Sprite();
        void SetImage(std::string texturePath);
        void SetPosition(iVec2 position);
        
    friend class Graphics;
};