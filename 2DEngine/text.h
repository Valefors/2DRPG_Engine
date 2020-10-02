#ifndef TEXT_H
#define TEXT_H

struct Glyph
{
	int xOffset;
	int yOffset;
	int width;
	int height;
	int textureId;
}; 

struct stbtt_fontinfo stbtt_fontinfo;

struct Font
{
	buf(char) fontPath;
	stbtt_fontinfo *fontInfo;
	int height;
	float scale;
	int ascent;
	int descent;
	int advance;
	Glyph **glyphs;
	bool *loaded;
};

struct Text
{
	Font *font;
	int *codes;
	buf(Sprite *) sprites;
	ivec2 position;
	int width;
	int height;
	int widthLimit;
	vec3 color;
	int nbCharToDisplay;
	int nbMaxCharToDisplay;
};

enum TextSize
{
	TEXT_SIZE_SMALL = 16,
	TEXT_SIZE_NORMAL = 32,
	TEXT_SIZE_BIG = 64,
	TEXT_SIZE_HUGE = 128
};

Text *create_text();
void set_text_position(Text *text, ivec2 position);
void set_text_font(Text *text, const char *fontPath, int textHeight);
void set_text_string(Text *text, const char *string);
void set_text_width_limit(Text *text, int limit);
void free_text(Text *text);

class TextManager
{
	public:
		void init();
};
#endif /* end of include guard: TEXT_H */