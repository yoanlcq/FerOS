	.macro .cst sym, value
	.global \sym
	.set \sym, \value
	.type \sym, @common
	.endm

	.macro .fn sym
	.global \sym
	.type \sym, @function
\sym:
	.endm
