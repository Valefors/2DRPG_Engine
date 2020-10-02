class WindowManager
{
    private:
        static double deltaTime;
        static double previousTime;

        static void updateInputsKeys();

    public:
        static void initWindow(int pWidth, int pHeight);
        static bool updateWindow();
        static void swapWindowBuffers();
        static void askWindowToClose();

    friend class GlobalDatas; //GlobalDatas has access to private deltaTime
};