#ifndef MAIN_H
#define MAIN_H
#include <3ds/types.h>

static inline char *strcpy(char *dst, char *src)
{
    char * cp = dst;  
    while( ((*cp++) = (*src++)) );        /* Copy src over dst */
    return( dst );  
}

static inline int strlen(char *s)
{
	int len;

	for (len = 0; s[len]; len++)
		;

	return len;
}

static inline void * memset(void *s_in, int d, int len)
{
	char * s = (char*)s_in;
	int i = 0;
	for(; i < len;i++)
		*(s + i) = d;
	return s + len;
}

static inline void * memcpy(void * d_in, void * s_in, int len)
{
	char * d = (char*)d_in;
	char * s = (char*)s_in;
	int i = 0;
	for (; i < len; i++)
		*d++ = *s++;
	return d;
}

typedef struct
{
    u32 code: 16;
    u32 index: 16;
} GlyphInfo;

typedef struct
{
	GlyphInfo * GlyphInfoTable;
	int GlyphMaxNum;
	int UsedNum;
	void * OriginFontFileObject;
	int GlyphSize;
} FontContext;

int ConvertCode2GlyphIndex(u8* thisptr, u32 code);
int DrawFontTexture12(FontContext * ctx, u8 * textureBuffer, int code, int glyphIndex);
int DrawFontTexture16(FontContext * ctx, u8 * textureBuffer, int code, int glyphIndex);
int GetGlyphBuffer(FontContext * ctx, u8 * glyphBuffer, int code, int glyphIndex, int * needDraw);
int CalcETC1A4Offset(int x, int y, int width);
void * GetAllocer();
void * AllocFromHeap(void* a0, int a1, int a2, int a3, int a4);
int UserFileSystem$$TryOpenFile(void ** iFile, u16 * path, int mode);

#endif