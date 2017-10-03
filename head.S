#define FUNC(name) \
	.global name; \
	.type name, %function; \
name:

.section .text.start
.arm

_text_start:
	ldr pc, =HOOK_ConvertCode2GlyphIndex

#ifdef VER10

FUNC(ConvertCode2GlyphIndex)
	mov r12, r0
	ldr pc, ConvertCode2GlyphIndexAddr

ConvertCode2GlyphIndexAddr:
	.word 0x16DD6C
#endif

#if (defined VER11) || (defined VER16)

FUNC(ConvertCode2GlyphIndex)
	STMFD SP!, {R4-R6}
	ldr pc, ConvertCode2GlyphIndexAddr

#endif

#ifdef VER11
ConvertCode2GlyphIndexAddr:
	.word 0x16F5CC
#endif

#ifdef VER16
ConvertCode2GlyphIndexAddr:
	.word 0x175040
#endif