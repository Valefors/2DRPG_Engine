#define STB_IMAGE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION

#include <stdbool.h>
#include <stddef.h>

#include "stb_image.h"
#include "stb_truetype.h"

static buf(Font *) fonts;
static buf(unsigned char *) ttfBuffers;
static buf(buf(char)) ttfFilesPaths;

static void free_font(Font *font)
{
	buf_free(font->fontPath);
	xfree(font->fontInfo);
	for (unsigned int i = 0; i < 0xFFFF; i++)
	{
		if (font->loaded[i])
		{
			xfree(font->glyphs[i]);
		}
	}
	xfree(font->glyphs);
	xfree(font->loaded);
	xfree(font);
}

void free_text()
{
	for (unsigned int i = 0; i < buf_len(ttfBuffers); i++)
	{
		xfree(ttfBuffers[i]);
		buf_free(ttfFilesPaths[i]);
	}
	buf_free(ttfBuffers);
	buf_free(ttfFilesPaths);
	
	for (unsigned int i = 0; i < buf_len(fonts); i++)
	{
		free_font(fonts[i]);
	}
	buf_free(fonts);
}

static void load_glyph(Font *font, int code)
{
	int leftSideBearing;
	int advance;
	int x0;
	int y0;
	int x1;
	int y1;
	stbtt_GetCodepointBitmapBox(font->fontInfo, code, font->scale, font->scale, &x0, &y0, &x1, &y1);
	font->glyphs[code] = xmalloc(sizeof (*font->glyphs[code]));
	font->glyphs[code]->width = x1 - x0;
	font->glyphs[code]->height = y1 - y0;

	unsigned char *bitmap = xmalloc(font->glyphs[code]->height * font->glyphs[code]->width * sizeof (*bitmap));
	stbtt_MakeCodepointBitmap(font->fontInfo, bitmap, font->glyphs[code]->width, font->glyphs[code]->height, font->glyphs[code]->width, font->scale, font->scale, code);

	unsigned int textureId;
	glGenTextures(1, &textureId);
	font->glyphs[code]->textureId = textureId;
	glBindTexture(GL_TEXTURE_2D, font->glyphs[code]->textureId);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, font->glyphs[code]->width, font->glyphs[code]->height, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);

	xfree(bitmap);

	font->glyphs[code]->yOffset = y0 + font->ascent - font->descent;

	stbtt_GetCodepointHMetrics(font->fontInfo, code, &advance, &leftSideBearing);
	font->glyphs[code]->xOffset = ceil((advance - leftSideBearing) * font->scale);

	font->loaded[code] = true;
}

static void load_font(const char *fontPath, int textHeight)
{
	stbtt_fontinfo *fontInfo = xmalloc(sizeof (*fontInfo));
	unsigned char *ttfBuffer = NULL;
	for (unsigned int i = 0; i < buf_len(ttfFilesPaths); i++)
	{
		if (strmatch(fontPath, ttfFilesPaths[i]))
		{
			ttfBuffer = ttfBuffers[i];
		}
	}
	if (!ttfBuffer)
	{
		ttfBuffer = (unsigned char *)file_to_string(fontPath);
		buf_add(ttfBuffers, ttfBuffer);
		buf_add(ttfFilesPaths, strclone(fontPath));
	}
	stbtt_InitFont(fontInfo, ttfBuffer, stbtt_GetFontOffsetForIndex(ttfBuffer, 0));
	int ascent;
	int descent;
	stbtt_GetFontVMetrics(fontInfo, &ascent, &descent, NULL);

	Font *font = xmalloc(sizeof (*font));
	font->glyphs = xmalloc(0xFFFF * sizeof (*font->glyphs));
	font->loaded = xmalloc(0xFFFF * sizeof (*font->loaded));
	for (int i = 0; i < 0xFFFF; i++)
	{
		font->loaded[i] = false;
	}
	font->fontInfo = fontInfo;
	font->fontPath = strclone(fontPath);
	font->height = textHeight;
	font->scale = stbtt_ScaleForPixelHeight(font->fontInfo, (float)textHeight);
	font->ascent = ceil(ascent * font->scale);
	font->descent = ceil(descent * font->scale);
	for (int code = 0; code < 128; code++)
	{
		load_glyph(font, code);
	}

	buf_add(fonts, font);
}

Text *create_text()
{
	Text *text = xmalloc(sizeof (*text));
	text->font = NULL;
	text->codes = NULL;
	text->sprites = NULL;
	text->widthLimit = -1;
	text->position.x = 0;
	text->position.y = 0;
	return text;
}

static void update_text(Text *text)
{
	text->width = 0;
	int currentLineWidth = 0;
	int currentLine = 1;
	int displayedSpriteCount = 0;

	for (unsigned int currentCodeIndex = 0; currentCodeIndex < buf_len(text->codes); currentCodeIndex++)
	{
		int currentCode = text->codes[currentCodeIndex];

		if (currentLineWidth == 0)
		{
			if (currentCode == ' ' && currentLine != 1)
			{
				continue;
			}
		}

		if (!text->font->loaded[currentCode])
		{
			load_glyph(text->font, currentCode);
		}

		if (displayedSpriteCount == buf_len(text->sprites))
		{
			buf_add(text->sprites, create_sprite(SPRITE_GLYPH));
		}

		Sprite *currentSprite = text->sprites[displayedSpriteCount];

		currentSprite->width = text->font->glyphs[currentCode]->width;
		currentSprite->height = text->font->glyphs[currentCode]->height;
		currentSprite->textureId = text->font->glyphs[currentCode]->textureId;
		currentSprite->color = text->color;

		currentSprite->position.x = text->position.x + currentLineWidth;
		currentSprite->position.y = text->position.y + (currentLine - 1) * (text->font->ascent - text->font->descent) + text->font->glyphs[currentCode]->yOffset;

		if (currentCodeIndex == buf_len(text->codes) - 1)
		{
			currentLineWidth += text->font->glyphs[currentCode]->xOffset + ceil(stbtt_GetCodepointKernAdvance(text->font->fontInfo, currentCode, 0) * text->font->scale);
		} else {
			currentLineWidth += text->font->glyphs[currentCode]->xOffset + ceil(stbtt_GetCodepointKernAdvance(text->font->fontInfo, currentCode, text->codes[currentCodeIndex + 1]) * text->font->scale);
		}

		displayedSpriteCount++;

		if (text->widthLimit != -1 && currentLineWidth > text->widthLimit)
		{
			int oldCodeIndex = currentCodeIndex;
			int oldSpriteCount = displayedSpriteCount;
			int oldLineWidth = currentLineWidth;
			bool newLine = false;
			while (true)
			{
				if (currentCodeIndex == 0)
				{
					currentCodeIndex = oldCodeIndex;
					displayedSpriteCount = oldSpriteCount;
					currentLineWidth = oldLineWidth;
					break;
				}

				if (currentCodeIndex == buf_len(text->codes) - 1)
				{
					currentLineWidth -= text->font->glyphs[text->codes[currentCodeIndex]]->xOffset + ceil(stbtt_GetCodepointKernAdvance(text->font->fontInfo, text->codes[currentCodeIndex], 0) * text->font->scale);
				} else {
					currentLineWidth -= text->font->glyphs[text->codes[currentCodeIndex]]->xOffset + ceil(stbtt_GetCodepointKernAdvance(text->font->fontInfo, text->codes[currentCodeIndex], text->codes[currentCodeIndex + 1]) * text->font->scale);
				}

				if (text->codes[currentCodeIndex] == ' ')
				{
					displayedSpriteCount++;
					newLine = true;
					break;
				}

				if (currentLineWidth <= 0)
				{
					currentCodeIndex = oldCodeIndex;
					displayedSpriteCount = oldSpriteCount;
					currentLineWidth = oldLineWidth;
					break;
				}

				currentCodeIndex--;
				displayedSpriteCount--;
			}
			if (newLine)
			{
				displayedSpriteCount--;
				if (currentLineWidth > text->width)
				{
					text->width = currentLineWidth;
				}
				currentLineWidth = 0;
				currentLine++;
			}
		}
	}
	if (currentLineWidth > text->width)
	{
		text->width = currentLineWidth;
	}
	text->height = currentLine * (text->font->ascent - text->font->descent) - 2;
	text->nbMaxCharToDisplay = displayedSpriteCount;
}

void set_text_width_limit(Text *text, int limit)
{
	text->widthLimit = limit;
	if (text->codes)
	{
		update_text(text);
	}
}

void set_text_position(Text *text, ivec2 position)
{
	text->position = position;
	text->position.y -= 2;
	if (text->codes)
	{
		update_text(text);
	}
}

void set_text_font(Text *text, const char *fontPath, int textHeight)
{
	text->font = NULL;
	for (unsigned int i = 0; i < buf_len(fonts); i++)
	{
		if (strmatch(fonts[i]->fontPath, fontPath))
		{
			if (fonts[i]->height == textHeight)
			{
				text->font = fonts[i];
				break;
			}
		}
	}
	if (!text->font)
	{
		if (textHeight == TEXT_SIZE_SMALL)
		{
			load_font(fontPath, TEXT_SIZE_SMALL);
		} else if (textHeight == TEXT_SIZE_NORMAL) {
			load_font(fontPath, TEXT_SIZE_NORMAL);
		} else if (textHeight == TEXT_SIZE_BIG) {
			load_font(fontPath, TEXT_SIZE_BIG);
		} else if (textHeight == TEXT_SIZE_HUGE) {
			load_font(fontPath, TEXT_SIZE_HUGE);
		} else {
			error("unsupported text height %d.", textHeight);
		}
		text->font = fonts[buf_len(fonts) - 1];
	}

	text->height = text->font->ascent - text->font->descent - 2;

	if (text->codes)
	{
		update_text(text);
	}
}

void set_text_string(Text *text, const char *string)
{
	buf_free(text->codes);
	text->codes = NULL;

	if (string)
	{
		text->codes = utf8_decode(string);
	}

	if (text->codes)
	{
		update_text(text);
		text->nbCharToDisplay = text->nbMaxCharToDisplay;
	}
}

void free_text(Text *text)
{
	buf_free(text->codes);
	for (unsigned int i = 0; i < buf_len(text->sprites); i++)
	{
		free_sprite(text->sprites[i]);
	}
	buf_free(text->sprites);
	xfree(text);
}

void draw()
{
	mat4 model = mat4_identity();
	model = mat4_translate(&model, sprite->position);
	model = mat4_scale(&model, (float)sprite->width, (float)sprite->height);
	
	glUseProgram(glyphShaderProgramId);
	set_mat4_uniform_to_shader(glyphShaderProgramId, "projection", &projection);
	set_mat4_uniform_to_shader(glyphShaderProgramId, "model", &model);
	glActiveTexture(GL_TEXTURE0);
	set_int_uniform_to_shader(glyphShaderProgramId, "glyphTextureId", 0);
	glBindTexture(GL_TEXTURE_2D, sprite->textureId);
	set_color_uniform_to_shader(glyphShaderProgramId, "textColor", sprite->color);
	set_float_uniform_to_shader(glyphShaderProgramId, "opacity", sprite->opacity);
	
	glBindVertexArray(vao);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
}