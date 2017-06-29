%include"io.mac"
.DATA
	dois dw 2

.UDATA
	b resw 1
	h resw 1
	r resw 1

.CODE
	.STARTUP

	GetInt [b] ; INPUT B
	GetInt [h] ; INPUT H
	mov AX,[b] ; LOAD B
	mov BX,[h] ; MULT H part 1
	mul Bx; MULT H, salva em AX
	sub dx,dx
	mov BX,[dois]
	div BX; DIV 2
	mov word [r],AX ;STORE R
	PutInt [r]
	nwln
	.EXIT

