class Camera
{
    public:
        static const iVec2& getPosition();
        static void updateCamera();
    
    private:
        static iVec2 _position;
        static int _cameraSpeed;
};