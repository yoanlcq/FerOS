/* Useful assembly-side macros */

/* .cst - Define a public constant */
	.macro .cst sym, value
	.global \sym
	.set \sym, \value
	.type \sym, @common
	.endm

/* .endcst - End the definition of a public constant */
	.macro .endcst sym
	.size \sym, (. - \sym)
	.endm

/* .fn - Define a public function */
	.macro .fn sym
	.global \sym
	.type \sym, @function
\sym:
	.endm
/* .endfn - End the definition of a public function */
	.macro .endfn sym
	.size \sym, (. - \sym)
	.endm
