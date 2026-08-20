#include "font.h"
#include "SDL_ttf.h"
#include "image.h"
#ifdef _WIN32
#include <Windows.h>
#endif

UnicodeBlock unicode_blocks[] = {
	// Línguas latinas
	{0x0000, 0x007F, 0, 0, 0, 0}, // Basic Latin
	{0x0080, 0x00FF, 0, 0, 0, 0}, // Latin-1 Supplement
	{0x0100, 0x017F, 0, 0, 0, 0}, // Latin Extended-A
	{0x2000, 0x206F, 0, 0, 0, 0}, // General Punctuation
	{0x0180, 0x024F, 0, 0, 0, 0}, // Latin Extended-B
	{0x1E00, 0x1EFF, 0, 0, 0, 0}, // Latin Extended Additional

	// Coreano
	{0x1100, 0x11FF, 0, 0, 0, 0}, // Hangul Jamo
	{0x3130, 0x318F, 0, 0, 0, 0}, // Hangul Compatibility Jamo
	{0xAC00, 0xD7AF, 0, 0, 0, 0}, // Hangul Syllables

	// Russo
	{0x0400, 0x04FF, 0, 0, 0, 0}, // Cyrillic
	{0x0500, 0x052F, 0, 0, 0, 0}, // Cyrillic Supplement
	{0x2DE0, 0x2DFF, 0, 0, 0, 0}, // Cyrillic Extended-A
	{0xA640, 0xA69F, 0, 0, 0, 0}, // Cyrillic Extended-B
	{0x1C80, 0x1C8F, 0, 0, 0, 0}, // Cyrillic Extended-C

	// Chinês (Simplificado e Tradicional) + Japonês
	{0x3000, 0x303F, 0, 0, 0, 0}, // CJK Symbols and Punctuation
	{0x3400, 0x4DBF, 0, 0, 0, 0}, // CJK Unified Ideographs Extension A
	{0x4E00, 0x9FFF, 0, 0, 0, 0}, // CJK Unified Ideographs
	//{0x20000, 0x2A6DF, 0, 0, 0, 0}, // CJK Unified Ideographs Extension B
	//{0x2A700, 0x2B73F, 0, 0, 0, 0}, // CJK Unified Ideographs Extension C
	//{0x2B740, 0x2B81F, 0, 0, 0, 0}, // CJK Unified Ideographs Extension D
	//{0x2B820, 0x2CEAF, 0, 0, 0, 0}, // CJK Unified Ideographs Extension E
	//{0x2CEB0, 0x2EBEF, 0, 0, 0, 0}, // CJK Unified Ideographs Extension F
	//{0x30000, 0x3134F, 0, 0, 0, 0}, // CJK Unified Ideographs Extension G

	// Japonês
	{0x3040, 0x309F, 0, 0, 0, 0}, // Hiragana
	{0x30A0, 0x30FF, 0, 0, 0, 0}, // Katakana
	{0x31F0, 0x31FF, 0, 0, 0, 0}, // Katakana Phonetic Extensions
};

const int num_blocks = sizeof(unicode_blocks) / sizeof(unicode_blocks[0]);

#define UTF8_NUM_BUFFS 4
#define UTF8_BUFFERLEN 1024

static char* get_utf8_buffer(void) {
	static char utf8_buffers[UTF8_NUM_BUFFS][UTF8_BUFFERLEN];
	static int buffer_idx = 0;
	buffer_idx = (buffer_idx + 1) & (UTF8_NUM_BUFFS - 1);
	return utf8_buffers[buffer_idx];
}

Uint32 utf8_decode_nth(const char* input,
	int        index,
	size_t     length)
{
	const unsigned char* p = (const unsigned char*)input;
	const unsigned char* limit = p + length;
	Uint32 code = ' ';

	while (p < limit && *p)
	{
		if (p[0] < 0x80) {
			code = p[0];
			p += 1;
		}
		else if ((p[0] & 0xE0) == 0xC0 &&
			(size_t)(limit - p) >= 2 &&
			(p[1] & 0xC0) == 0x80)
		{
			code = ((p[0] & 0x1F) << 6) |
				(p[1] & 0x3F);
			p += 2;
		}
		else if ((p[0] & 0xF0) == 0xE0 &&
			(size_t)(limit - p) >= 3 &&
			(p[1] & 0xC0) == 0x80 &&
			(p[2] & 0xC0) == 0x80)
		{
			code = ((p[0] & 0x0F) << 12) |
				((p[1] & 0x3F) << 6) |
				(p[2] & 0x3F);
			p += 3;
		}
		else if ((p[0] & 0xF8) == 0xF0 &&
			(size_t)(limit - p) >= 4 &&
			(p[1] & 0xC0) == 0x80 &&
			(p[2] & 0xC0) == 0x80 &&
			(p[3] & 0xC0) == 0x80)
		{
			code = ((p[0] & 0x07) << 18) |
				((p[1] & 0x3F) << 12) |
				((p[2] & 0x3F) << 6) |
				(p[3] & 0x3F);
			p += 4;
		}
		else {
			code = '?';
			p += 1;
		}

		if (index-- == 0)
			return code;
	}

	return ' ';
}

#ifdef _WIN32
char* to_utf8(const char* str) {
	if (!str) return NULL;

	char* utf8_buf = get_utf8_buffer();

	wchar_t wide_str[1024];

	int wide_len = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);
	if (wide_len == 0 || wide_len > 1024) {
		utf8_buf[0] = '\0';
		return utf8_buf;
	}
	MultiByteToWideChar(CP_ACP, 0, str, -1, wide_str, wide_len);

	int utf8_len = WideCharToMultiByte(CP_UTF8, 0, wide_str, -1, NULL, 0, NULL, NULL);
	if (utf8_len == 0 || utf8_len > UTF8_BUFFERLEN) {
		utf8_buf[0] = '\0';
		return utf8_buf;
	}
	WideCharToMultiByte(CP_UTF8, 0, wide_str, -1, utf8_buf, utf8_len, NULL, NULL);

	return utf8_buf;
}
#else
char* to_utf8(const char* str) {
	if (!str) return NULL;
	char* utf8_buf = get_utf8_buffer();
	q_strlcpy(utf8_buf, str, UTF8_BUFFERLEN);
	return utf8_buf;
}
#endif


void get_texture_data(GLuint textureID, SDL_Surface** surface)
{
	glBindTexture(GL_TEXTURE_2D, textureID);
	GLint width, height;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
	unsigned char* pixelBuffer = (unsigned char*)malloc(width * height * 4);
	if (*pixelBuffer) {
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelBuffer);
		*surface = SDL_CreateRGBSurfaceFrom(pixelBuffer, width, height, 32, width * 4, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
		free(pixelBuffer);
	}
}

void dump_texture_to_file(GLuint textureID, const char* filename)
{
	glBindTexture(GL_TEXTURE_2D, textureID);
	GLint width, height;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);
	unsigned char* pixelBuffer = (unsigned char*)malloc(width * height * 4);
	if (pixelBuffer) {
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelBuffer);
		SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(pixelBuffer, width, height, 32, width * 4,
			0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
		if (surface) {
			SDL_SaveBMP(surface, filename);
			SDL_FreeSurface(surface);
		}
		free(pixelBuffer);
	}
}

int copy_single_glyph(UnicodeBlock* block, SDL_Surface* source, SDL_Surface* destination, int columns, Uint32 codepoint)
{
	if (!source)
	{
		return 1;
	}
	int srcRow = codepoint / columns;
	int srcCol = codepoint % columns;
	SDL_Rect srcRect = { srcCol * CHAR_WIDTH ,srcRow * CHAR_HEIGHT ,CHAR_WIDTH, CHAR_HEIGHT };

	int dstIndex = codepoint - block->start;
	int dstRow = dstIndex / columns;
	int dstCol = dstIndex % columns;
	SDL_Rect dstRect = { dstCol * CHAR_WIDTH ,dstRow * CHAR_HEIGHT ,CHAR_WIDTH, CHAR_HEIGHT };
	SDL_BlitSurface(source, &srcRect, destination, &dstRect);
	return 0;
}

int copy_glyph(UnicodeBlock* block, SDL_Surface* glyph, SDL_Surface* destination, int columns, Uint32 codepoint)
{
	if (!glyph)
	{
		return 1;
	}
	int index = codepoint - block->start;
	int row = index / columns;
	int col = index % columns;
	SDL_Rect dstRect = { col * CHAR_WIDTH + (CHAR_WIDTH - glyph->w) / 2,row * CHAR_HEIGHT + (CHAR_HEIGHT - glyph->h) / 2,glyph->w, glyph->h };
	SDL_BlitSurface(glyph, NULL, destination, &dstRect);
	return 0;
}

void select_font(void)
{
	int startPos = COM_CheckParm("-language");
	if (startPos != 0 && startPos < sizeof(com_argv) - 1) {
		const char* language = com_argv[startPos + 1];
		if (
			strcmp(language, "loc_chinese.txt") == 0 ||
			strcmp(language, "loc_chinese2.txt") == 0 ||
			strcmp(language, "loc_japanese.txt") == 0 ||
			strcmp(language, "loc_korean.txt") == 0
			)
		{
			font_index = 1;
			return;
		}
		font_index = 0;
	}
}

int generate_font_pngs(void)
{
	TTF_Font* font = NULL;

	SDL_Surface* consoleSurface = NULL;
	byte* consoleData = NULL;
	int consoleWidth = 0;
	int consoleHeight = 0;
	enum srcformat consoleFmt;
	qboolean consoleMalloced = false;

	for (int i = 0; i < num_blocks; i++)
	{
		UnicodeBlock* block = &unicode_blocks[i];
		Uint32 num_chars = block->end - block->start + 1;
		const int rows = (int)ceil((double)num_chars / COLUMNS);

		char* relative_filename = va("fonts\\glyphs%i_%i", font_index, i);

		SDL_Surface* surface = SDL_CreateRGBSurface(
			0,
			COLUMNS * CHAR_WIDTH,
			rows * CHAR_HEIGHT,
			32,
			0x00FF0000,
			0x0000FF00,
			0x000000FF,
			0xFF000000
		);

		if (!surface)
		{
			if (consoleSurface) SDL_FreeSurface(consoleSurface);
			if (consoleMalloced && consoleData) free(consoleData);
			if (font) TTF_CloseFont(font);
			return 0;
		}

		SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 0, 0, 0, 0));

		for (Uint32 codepoint = block->start; codepoint <= block->end; codepoint++)
		{
			if ((i == 0 || i == 1) && codepoint - block->start < 33)
			{
				if (!consoleSurface)
				{
					consoleData = Image_LoadImage("gfx/console",
						&consoleWidth,
						&consoleHeight,
						&consoleFmt,
						&consoleMalloced);
					if (consoleData)
					{
						consoleSurface = SDL_CreateRGBSurfaceFrom(
							consoleData,
							consoleWidth,
							consoleHeight,
							32,
							consoleWidth * 4,
							0x00FF0000,
							0x0000FF00,
							0x000000FF,
							0xFF000000
						);
					}
				}

				if (!consoleSurface)
				{
					SDL_FreeSurface(surface);
					if (consoleSurface) SDL_FreeSurface(consoleSurface);
					if (consoleMalloced && consoleData) free(consoleData);
					if (font) TTF_CloseFont(font);
					return 0;
				}

				copy_single_glyph(block, consoleSurface, surface, COLUMNS, codepoint);
			}
			else
			{
				if (!font)
				{
					if (TTF_Init() == -1)
					{
						SDL_FreeSurface(surface);
						if (consoleSurface) SDL_FreeSurface(consoleSurface);
						if (consoleMalloced && consoleData) free(consoleData);
						return 0;
					}
#ifdef BDDPRE4
					const char* font_filename = va("%s\\bddpre4\\fonts\\font%i.ttf",
#else
					const char* font_filename = va("%s\\id1\\fonts\\font%i.ttf",
#endif
						to_utf8(com_basedir),
						font_index);
					font = TTF_OpenFont(font_filename, FONT_SIZE);
					if (!font)
					{
						SDL_FreeSurface(surface);
						if (consoleSurface) SDL_FreeSurface(consoleSurface);
						if (consoleMalloced && consoleData) free(consoleData);
						return 0;
					}

					TTF_SetFontStyle(font, TTF_STYLE_BOLD);
					TTF_SetFontSize(font, FONT_SIZE);
				}

				SDL_Surface* glyph = TTF_RenderGlyph32_Blended(
					font,
					codepoint,
					(SDL_Color) {
					255, 255, 255, 255
				}
				);

				if (copy_glyph(block, glyph, surface, COLUMNS, codepoint))
				{
					if (glyph) SDL_FreeSurface(glyph);
					continue;
				}

				SDL_FreeSurface(glyph);
			}
		}

		{
			char* absolute_filename = va("%s.png", relative_filename);
			Image_WritePNG(absolute_filename,
				surface->pixels,
				surface->w,
				surface->h,
				32,
				true);
		}

		SDL_FreeSurface(surface);
	}

	if (consoleSurface)
		SDL_FreeSurface(consoleSurface);
	if (consoleMalloced && consoleData)
		free(consoleData);
	if (font)
		TTF_CloseFont(font);

	return 1;
}

int setup_fonts(void)
{
	for (int i = 0; i < num_blocks; i++)
	{
		UnicodeBlock* block = &unicode_blocks[i];
		char* relative_filename = va("fonts/glyphs%i_%i", font_index, i);
		block->texture = TexMgr_LoadImage(NULL, relative_filename, 0, 0, SRC_EXTERNAL, NULL, relative_filename, 0, TEXPREF_NEAREST | TEXPREF_ALPHA);
		block->columns = COLUMNS;
		block->tex_width = block->texture->width;
		block->tex_height = block->texture->height;
	}
	return 1;
}


void draw_character_quad_ex_inner(int x, int y, Uint32 codepoint, int r, int g, int b)
{
	UnicodeBlock* block = NULL;
	for (int j = 0; j < num_blocks; j++) {
		if (codepoint >= unicode_blocks[j].start && codepoint <= unicode_blocks[j].end) {
			block = &unicode_blocks[j];
			break;
		}
	}
	if (!block) {
		return;
	}

	int index = codepoint - block->start;
	int row = index / block->columns;
	int col = index % block->columns;
	float s1 = (float)(col * CHAR_WIDTH) / block->tex_width;
	float s2 = (float)((col + 1) * CHAR_WIDTH) / block->tex_width;
	float t1 = (float)(row * CHAR_HEIGHT) / block->tex_height;
	float t2 = (float)((row + 1) * CHAR_HEIGHT) / block->tex_height;

	glEnable(GL_BLEND);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glColor4f(r / 255.0f, g / 255.0f, b / 255.0f, 1.0f);

	glEnable(GL_TEXTURE_2D);
	GL_Bind(block->texture);

	glBegin(GL_QUADS);
	glTexCoord2f(s1, t1); glVertex2i(x, y);
	glTexCoord2f(s2, t1); glVertex2i(x + DRAW_WIDTH, y);
	glTexCoord2f(s2, t2); glVertex2i(x + DRAW_WIDTH, y + DRAW_HEIGHT);
	glTexCoord2f(s1, t2); glVertex2i(x, y + DRAW_HEIGHT);
	glEnd();

	glDisable(GL_BLEND);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glColor4f(1, 1, 1, 1);
}


void draw_character_quad_ex(int x, int y, Uint32 codepoint, int r, int g, int b)
{
	draw_character_quad_ex_inner(x + 1, y + 1, codepoint, 0, 0, 0);
	draw_character_quad_ex_inner(x + 1, y, codepoint, 0, 0, 0);
	draw_character_quad_ex_inner(x, y + 1, codepoint, 0, 0, 0);
	draw_character_quad_ex_inner(x, y, codepoint, r, g, b);
}
