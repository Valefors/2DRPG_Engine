enum E_TokenType
{
    END_OF_FILE,
    END_OF_LINE,
    OBJECT_TYPE,
    NUMERIC,
    BB,
    TEXTURE_PATH
};

class Token
{
    public:
        static std::string tokenTypeNames[];
        E_TokenType tokenType;
        //union {std::string stringValue; double numericValue;};
        std::string stringValue; 
        double numericValue;
};