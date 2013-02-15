place	equ	80
stack	segment para stack 'stack'
	dw	128	dup(0)
stack	ends
data	segment	para public 'data'
bpb	db 0DFh,02,25h,02,09h,2Ah,0FFh,50h,0F6h,0Fh,08,27h,40h	;360K
	db 0DFh,02,25h,02,0Fh,1Bh,0FFh,54h,0F6h,0Fh,08,4Fh,00h  ;1.2M
	db 0DFh,02,25h,02,09h,2Ah,0FFh,50h,0F6h,0Fh,08,4Fh,80h	;720K
	db 0DFh,02,25h,02,12h,1Bh,0FFh,65h,0F6h,0Fh,08,4Fh,00h	;1.44M
	;13*i----i=0-3
head	db	0		;0--1 only
track	db	place           ;0--39  or 0--79
disk    db      1
;====================
;  0---A;    1---B;
;====================
sectors	db	15
disktype dw	1
;=======================
;     0-----360M 5.25
;     1-----1.2M 5.25
;     2-----720K 3.5
;     3-----1.44 3.5
;=======================

param	db place,00,01,02
	db place,00,02,02
	db place,00,03,02
	db place,00,04,04
	db place,00,05,02
	db place,00,06,02
	db place,00,07,02
	db place,00,08,02
	db place,00,09,03
	db place,00,10,02
	db place,00,11,02
	db place,00,12,02
	db place,00,13,02
	db place,00,14,02
	db place,00,15,05
	db place,00,16,02
	db place,00,17,02
	db place,00,18,05
	db 0,0,0,0
buff	db 4096 dup(0)
	db 100 dup('z')
way0	dw	6 dup (0)
way1	dw	6 dup (0)
way2	dw	6 dup (0)
buff1	db	256 dup(0)
disp1	db	'Failure',0dh,0ah,'$'
disp2	db 	'Success',00dh,0ah,'$'
mess_3	db	'File Format is Error',0dh,0ah,'$'
fl	db	'start.bin',0,0,0
fl_1	db	'pswd.dat',0,0,0
data	ends
code	segment	para public 'code'
	assume cs:code,ds:data,ss:stack
main	proc	far
	push	ds
	xor	ax,ax
	push	ax
	mov	ax,351eh
	int	21h
	mov	cs:seg_1e,es
	mov	cs:off_1e,bx
        mov	ax,data
	mov	ds,ax
	mov	es,ax
	;mov	dx,offset buff1
	;mov	ax,440dh
	;mov	cx,860h
	;mov	bl,disk
	;inc	bl
	;int	21h
	mov	ax,disktype
	call	set_bpb
	;mov	ax,106h
	;mov	word ptr ds:bpb+16,ax
	mov	cx,5            ;try 5 times format
test_f:	push	cx
	mov	ax,0f02h
	mov	word ptr ds:bpb+16,ax
	mov	ah,0
	mov	dl,disk
	int	13h
	call	format
	pop	cx
	jc	error		;format failure
	mov	bx,5		;read 5 times
test_r: push	bx
	mov	ax,0f05h
	mov	word ptr ds:bpb+16,ax
	mov	ah,0
	mov	dl,disk
	int	13h
	call	read
	pop	bx
	jnc	next		;no error means not source disk
	cmp	ah,10h		;CRC error means data in memory
	jnz	next
	call	recod		;record the buff with 1 1.5 2 ways
	dec	bx
	jnz	test_r
	call	check		;compare the result
	jc 	ok_bye		;lock success
next:
	loop	test_f
error:	mov 	dx,offset disp1
	jmp	bye
ok_bye:
	mov	ax,way0[2]
	call	print
	mov	ax,way1[2]
	call	print
	mov	ax,way2[2]
	call	print
	call	res_bpb
	mov	dl,disk
	mov 	ax,1700h
	int	13h
	mov 	ah,0
	int	13h
        mov	dx,offset fl_1
	mov	ah,3ch
	mov	cx,0				;hide
	int	21h
	mov     bx,ax
	mov	dx,offset buff
	mov	cx,1000h
	mov	ah,40h
	int	21h
	mov	ah,3eh
	int	21h
	mov	dx,offset fl
	mov	ax,3d02h
	int	21h
	mov	bx,ax
	mov	si,0
rd_nx:	mov	ah,3fh
	mov	dx,offset buff
	mov 	cx,1000h
	int 	21h
	push	ax
	call	find
	jc	find_1
	pop	ax
	inc	si
	cmp	ax,1000h
	jz	rd_nx
	mov	ah,3eh
	int	21h
	mov	dx,offset mess_3
	jmp	bye
find_1:
	mov	ax,way0[2]
	mov	word ptr buff[di],ax
	mov	ax,way1[2]
	mov	word ptr buff[di+2],ax
	mov	ax,way2[2]
	mov	word ptr buff[di+4],ax
	mov	al,track
	mov	ah,sectors
	mov	word ptr buff[di+6],ax
	mov	ax,disktype
	mov	word ptr buff[di+8],ax
	or	si,si
	jz	no_loop
	mov	cx,si
	xor	si,si
add_si:	add	si,1000h
	loop	add_si
no_loop:
	mov	ax,4200h
	mov	dx,si
	xor	cx,cx
	int	21h
	mov	ah,40h
	mov	dx,offset buff
	pop	cx
	int	21h
	mov	ah,3eh
	int 	21h

        mov 	dx,offset disp2
bye:	mov	ah,9
	int	21h
	call	res_bpb
	mov	ax,0f02h
	mov	word ptr ds:bpb+16,ax
	mov	ax,1700h
	mov	dl,disk
	int 	13h
	;mov	dx,offset buff1
	;mov	ax,440dh
	;mov	cx,840h
	;mov	bl,disk
	;inc	bl
	;int	21h
	retf
main	endp
find	proc	near
	mov	cx,2048
	mov	di,offset buff
	mov	ax,'TS'
con_find:
	cld
	repne	scasw
	jz	ok_find
	clc
	ret
ok_find:
	cmp	word ptr ds:[di],'CA'
	jnz	con_find
	cmp	word ptr ds:[di+2],'_K'
	jnz	con_find
	cmp	word ptr ds:[di+4],'00'
	jnz	con_find
	sub	di,(offset buff+2)
	stc
	ret
find	endp
print	proc	near
	mov	cx,4
pr_l:	rol	ax,1
	rol	ax,1
	rol	ax,1
	rol	ax,1
	mov	dl,al
	and	dl,0fh
	cmp	dl,9
	jbe	next_p
	add	dl,7
next_p:	add	dl,30h
	push	ax
	mov	ah,2
	int	21h
	pop	ax
	loop	pr_l
	mov	ah,2
        mov	dl,10
	int	21h
	mov	dl,13
	int 	21h
	ret
print	endp
set_bpb	proc	near
	mov	bx,13
	mul	bx
	mov	dx,offset bpb
	add	dx,ax
	mov	ax,251eh
	int	21h
	ret
set_bpb	endp
seg_1e	dw	?
off_1e	dw	?
res_bpb	proc	near
	push	ds
	mov	dx,cs:off_1e
	mov	ds,cs:seg_1e
	mov	ax,251eh
	int	21h
	pop	ds
	ret
res_bpb	endp
format  proc	near
	mov	cx,5
rep_5:	push	cx
	mov	ax,1703h
	int	13h
	mov	ch,track	;track
	mov	cl,01h		;begin sector
	mov	dh,head		;head
	mov	dl,disk           ;disk No.
	mov 	bx,offset param
	mov	ah,5h
	mov	al,sectors
	int	13h
	pop	cx
	jnb	format_bye
	loop	rep_5
format_bye:
	ret
format	endp
read	proc	near
	mov	cx,1
loop_1:	push	cx
	mov	ax,1703h
	int	13h
	mov	ch,track		;track
	mov	cl,sectors		;begin sector
	mov	dh,head			;head
	mov	dl,disk		        ;disk No.
	mov 	bx,offset buff
	mov	ax,201h
	int	13h
	pop	cx
	jc	rts
	loop	loop_1
rts:	ret
read	endp
old	dw	?
recod proc near
	push	bx
	push	cx
	shl	bx,1
;=========way1================
	xor	ax,ax
	mov	way1[bx],ax
	mov	si,offset buff
	mov	cx,2048
	cld
rep_r1:	lodsw
	add	way1[bx],ax
	loop	rep_r1
;==========way0===============
	xor	ax,ax
	mov	way0[bx],ax
	mov	cs:old,ax
	mov	si,offset buff
	mov	cx,2048
	cld
rep_r0:	lodsw
	cmp	ax,cs:old
	mov	cs:old,ax
	jz	no_add
	add	way0[bx],ax
no_add:
	loop	rep_r0
;===========way 2============
xor	ax,ax
	mov	way2[bx],ax
	mov	si,offset buff
	mov	cx,1365			;4096/3
	cld
rep_r2:	lodsw
	add	way2[bx],ax
	lodsb
	add	byte ptr way2[bx],al
	loop	rep_r2
;===========way end============
	pop	cx
	pop	bx
	ret
recod endp
check	proc	near
	push	ax
	push	bx
	push	cx
	mov	cx,4
lop_chk:mov	bx,cx
	add	bx,cx
	mov	ax,way0[10]
	cmp	ax,way0[bx]
	jnz	fail
	mov	ax,way1[10]
	cmp	ax,way1[bx]
	jnz	fail
	mov	ax,way2[10]
	cmp	ax,way2[bx]
	jnz	fail
	loop	lop_chk
	stc
	jmp	chk_bye
fail:	clc
chk_bye:
	pop	cx
	pop	bx
	pop	ax
	ret
check	endp
code	ends
	end	main


