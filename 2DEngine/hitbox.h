class Hitbox
{
    private:
        iVec2 _position;
        iVec2 _dimension;
    
    public :
        Hitbox(iVec2 position, iVec2 size);
        ~Hitbox();
        void SetPosition(iVec2 position);
    
    friend class Physics;
};