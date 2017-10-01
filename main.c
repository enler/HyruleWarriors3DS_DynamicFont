#include "main.h"

static inline int CalcGlyphIndex(u32 code, u32 start)
{
	u32 h = code >> 8;
	u32 l = code & 0xFF;
	h -= 0x88;
	if (l > 0x7F)
	{
		l--;
	}
	l -= 0x40;
	return h * 0xBC + l - 0x5E + start;
}

static inline int SearchCode(FontContext * ctx, u16 code, int * unusedIndex)
{
	s32 i = 0;
	for(; i < ctx->UsedNum; i++)
	{
		if(ctx->GlyphInfoTable[i].code == code)
			return i;
	}
	if(ctx->UsedNum == ctx->GlyphMaxNum)
		*unusedIndex = ctx->GlyphMaxNum - 1;
	else
	{
		ctx->GlyphInfoTable[ctx->UsedNum].index = ctx->UsedNum;
		*unusedIndex = ctx->UsedNum;
	}
	return - 1;
}

static inline void MoveGlyphInfoToStart(FontContext * ctx, int index)
{
	if((u32)index < ctx->GlyphMaxNum)
	{
		GlyphInfo final = ctx->GlyphInfoTable[index];//an easy LRU cache
		int i;
		for(i = index - 1; i >= 0; i--)
		{
			ctx->GlyphInfoTable[i + 1] = ctx->GlyphInfoTable[i];
		}
		ctx->GlyphInfoTable[0] = final;
	}
}

static inline void ReadGlyphFromFont(FontContext * ctx, int glyphIndex, u8 * glyphBuffer)
{
	if(ctx->OriginFontFileObject == 0)
	{
		memset(glyphBuffer, 0, ctx->GlyphSize);
	}
	else
	{
		int (*IFile$$TryRead)(void *, s32 *, s64, void *, size_t) = **(void***)ctx->OriginFontFileObject;
		if (IFile$$TryRead)
		{
			s32 pOut = 0;
			IFile$$TryRead(ctx->OriginFontFileObject, &pOut, glyphIndex * ctx->GlyphSize, glyphBuffer, ctx->GlyphSize);
		}
	}
}

int GetGlyphBuffer(FontContext * ctx, u8 * glyphBuffer, int code, int glyphIndex, int * needDraw)
{
	int index;
	int unusedIndex;
	index = SearchCode(ctx, code, &unusedIndex);
	*needDraw = 0;
	if(index < 0)
	{
		ctx->GlyphInfoTable[unusedIndex].code = code;
		ReadGlyphFromFont(ctx, glyphIndex, glyphBuffer);
		index = unusedIndex;
		if(ctx->UsedNum < ctx->GlyphMaxNum)
			ctx->UsedNum++;
		*needDraw = 1;
	}
	MoveGlyphInfoToStart(ctx, index);
	return ctx->GlyphInfoTable[0].index;
}

int CalcETC1A4Offset(int x, int y, int width)
{
	int x_s = x & ~7;
	int y_s = y & ~7;
	int blkID = y_s * width / 64 + x_s / 8;
	int subOffset = (((y & 0x7) >> 2) * 2 + ((x & 0x7) >> 2)) * 0x10;
	int offset = subOffset + blkID * 0x40;
	return offset;
}

FontContext * InitFontContext(int GlyphMaxNum, u16 * fontPath, int glyphSize)
{
	FontContext * ctx = (FontContext*)AllocFromHeap(GetAllocer(), 0x10, sizeof(FontContext), 1, 0);
	memset(ctx, 0, sizeof(FontContext));
	ctx->GlyphInfoTable = (GlyphInfo*)AllocFromHeap(GetAllocer(), 0x10, sizeof(GlyphInfo) * GlyphMaxNum, 1, 0);
	ctx->GlyphMaxNum = GlyphMaxNum;
	UserFileSystem$$TryOpenFile(&ctx->OriginFontFileObject, fontPath, 1);
	ctx->GlyphSize = glyphSize;
	return ctx;
}

int HOOK_ConvertCode2GlyphIndex(u8 * thisptr, u32 code)
{
	static FontContext * fontContext12 = (FontContext *)0;
	static FontContext * fontContext16 = (FontContext *)0;
	if (code < 0x889F || *(thisptr + 0x20B9C) || code > 0x93F4)
	{
		return ConvertCode2GlyphIndex(thisptr, code);
	}
	
	if (fontContext12 == 0)
	{
		fontContext12 = InitFontContext(0x59B, L"rom:/Font12.bin", 12 * 12);
	}
	if (fontContext16 == 0)
	{
		fontContext16 = InitFontContext(0x2AE, L"rom:/Font16.bin", 16 * 16);
	}
	int glyphIndex = CalcGlyphIndex(code, 0);
	int isFont12 = 0;
	
	if (*(thisptr + 0x21034) || (!*(thisptr + 0x20B9C) && !*(thisptr + 0x20B75)))
	{
		isFont12 = 1;
	}
	
	u8 * textureBuffer = *(*((u8***)thisptr + (isFont12 ? 0x3 : 0x4)) + 0x2);
	if (isFont12)
		return DrawFontTexture12(fontContext12, textureBuffer, code, glyphIndex);
	else
		return DrawFontTexture16(fontContext16, textureBuffer, code, glyphIndex);
}