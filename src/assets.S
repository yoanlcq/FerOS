#if 0 /* Disable them for now, but that's how we would implement them */
.macro .asset sym:req, file:req
	.global \sym
	.type \sym, @common
\sym:
	.incbin "\file"
	.size \sym, (. - \sym)
.endm

.macro .rgb24image sym:req, w:req, h:req, file:req
	.global \sym
	.type \sym, @common
\sym:
	.incbin "\file"
	.size \sym, (. - \sym)
.endm



.section .assets
.asset asset_foo, 128, 128, "/tmp/foo.rgb"
.asset asset_bar, 128, 128, "/tmp/bar.rgb"
.asset asset_baz, 128, 128, "/tmp/baz.rgb"
.asset asset_qux, 128, 128, "/tmp/qux.rgb"
#endif
