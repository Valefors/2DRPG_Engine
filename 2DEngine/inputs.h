enum KeyCode
{
    //Next values are +1 because enum
    SPACE = 32,
    INPUT_0 = 48,
    INPUT_1,
    INPUT_2,
    INPUT_3,
    INPUT_4,
    INPUT_5,
    INPUT_6,
    INPUT_7,
    INPUT_8,
    INPUT_9,
    A = 65,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,
    ESCAPE = 256,
    ENTER,
    TAB,
    BACKSPACE,
    INSER,
    DEL,
    RIGHT_ARROW,
    LEFT_ARROW,
    DOWN_ARROW,
    UP_ARROW,
    PAGE_UP,
    PAGE_DOWN,
    CAPS_LOCK = 280,
    F1 = 290,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    NUMPAD_0 = 320,
    NUMPAD_1,
    NUMPAD_2,
    NUMPAD_3,
    NUMPAD_4,
    NUMPAD_5,
    NUMPAD_6,
    NUMPAD_7,
    NUMPAD_8,
    NUMPAD_9,
    DOT = 330,
    DIVIDE,
    MULTIPLY,
    SUBTRACT,
    ADD,
    SHIFT = 340,
    CONTROL_LEFT = 341,
    ALT = 342,
    CONTROL_RIGHT = 345,
    LEFT_MOUSE_BUTTON,
    RIGHT_MOUSE_BUTTON,
    MIDDLE_MOUSE_BUTTON,
    SIDE_MOUSE_BUTTON_1,
    SIDE_MOUSE_BUTTON_2,
    KEYCODE_COUNT
};

class InputsManager
{
    public:
        //getKey is defined in window.cpp
        static bool getKey(KeyCode keyCode);
};