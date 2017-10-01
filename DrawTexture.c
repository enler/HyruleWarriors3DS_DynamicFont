#include "main.h"

static inline void DrawFontTexture(u8 * textureBuffer, u8 * glyphBuffer, int glyphWidth, int glyphHeight, int glyphIndex)
{
	int glyphNumPerLine = 0x400 / glyphWidth;
	int y = (glyphIndex / glyphNumPerLine) * glyphHeight;
	int x = (glyphIndex % glyphNumPerLine) * glyphWidth;
	y = 0x100 - y - glyphHeight;
	for (int i = 0; i < glyphWidth; i += 4)
	{
		for (int j = 0; j < glyphHeight; j += 4)
		{
			int glyphOffset = ((j >> 2) * (glyphWidth >> 2) + (i >> 2)) * 0x10;
			int textureOffset = CalcETC1A4Offset(x + i, y + j, 0x400);
			memcpy(textureBuffer + textureOffset, glyphBuffer + glyphOffset, 0x10);
		}
	}
}

int DrawFontTexture12(FontContext * ctx, u8 * textureBuffer, int code, int glyphIndex)
{
	u8 glyphBuffer[12 * 12];
	int needDraw;
	int index = GetGlyphBuffer(ctx, glyphBuffer, code, glyphIndex, &needDraw);
	if (needDraw)
		DrawFontTexture(textureBuffer, glyphBuffer, 12, 12, index + 0x11F);
	return index + 0x11F;
}

int DrawFontTexture16(FontContext * ctx, u8 * textureBuffer, int code, int glyphIndex)
{
	u8 glyphBuffer[16 * 16];
	int needDraw;
	int index = GetGlyphBuffer(ctx, glyphBuffer, code, glyphIndex, &needDraw);
	if (needDraw)
		DrawFontTexture(textureBuffer, glyphBuffer, 16, 16, index + 0x10F);
	return index + 0x10F;
}