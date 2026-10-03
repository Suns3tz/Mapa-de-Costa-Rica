section .data

section .bss

section .text
    global oct1
    global oct2
    global oct3
    global oct4
    global oct5
    global oct6
    global oct7
    global oct8
    
    extern plot
   

; vars en C vienen así:
; x0 en edi
; y0 en esi
; x1 en edx
; y1 en ecx

; delta_e va a estar en  ebx
; delta_ne va a estar en ebp
; xp va a estar en r12d
; yp va a estar en r13d
; d va a estar en r14d
; x1 va a estar en r15d

; - - - - - OCTANTE 1
oct1:
    ; guardar registros que hay que conservar
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    ; alinear stack antes de llamar funciones C
    sub rsp, 8

    ; delta_e = 2*(y1-y0)
    mov eax, ecx
    sub eax, esi

    shl eax, 1
    mov ebx, eax

    ; delta_ne = 2*((y1-y0)-(x1-x0))
    mov eax, ecx
    sub eax, esi

    mov r8d, edx
    sub r8d, edi

    sub eax, r8d

    shl eax, 1
    mov ebp, eax

    ;xp = x0, yp = y0
    mov r12d, edi
    mov r13d, esi

    ; d = 2*(y1-y0) - (x1-x0)
    mov eax, ecx
    sub eax, esi

    shl eax, 1

    mov esi, edx
    sub esi, edi

    sub eax, esi
    mov r14d, eax

    ; Guardar x1
    mov r15d, edx

    ; plot(xp, yp)
    mov edi, r12d
    mov esi, r13d
    call plot

; entrar en el while
_colorLoop:
    cmp r12d, r15d
    ; while xp < x1 siga, else termina
    jge _final

    cmp r14d, 0
    ; if d >= 0 se colorea NE else se colorea E
    jl _colorE
    jmp _colorNE

_colorE:
    ; pintar E, xp++, d = d+delta_e
    add r12d, 1
    add r14d, ebx

    jmp _plot

_colorNE:
    ; pintar NE, xp++, yp++, d = d+delta_ne
    add r12d, 1
    add r13d, 1
    add r14d, ebp

    jmp _plot

_plot:
    mov edi, r12d
    mov esi, r13d

    call plot

    jmp _colorLoop

_final:
    add rsp, 8

    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    pop rbx

    ret

; - - - - - OCTANTE 2
oct2:
    ; guardar registros que hay que conservar
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    ; alinear stack antes de llamar funciones C
    sub rsp, 8

    ; delta_n = 2*(x0-x1)
    mov eax, edi
    sub eax, edx

    shl eax, 1
    mov ebx, eax

    ; delta_ne = 2*((y1-y0)-(x1-x0))
    mov eax, ecx
    sub eax, esi

    mov r8d, edx
    sub r8d, edi

    sub eax, r8d

    shl eax, 1
    mov ebp, eax

    ;xp = x0, yp = y0
    mov r12d, edi
    mov r13d, esi

    ; d = (y1-y0) - 2*(x1-x0)
    mov eax, ecx
    sub eax, esi

    mov esi, edx
    sub esi, edi

    shl esi, 1

    sub eax, esi
    mov r14d, eax

    ; Guardar y1
    mov r15d, ecx

    ; plot(xp, yp)
    mov edi, r12d
    mov esi, r13d
    call plot

; entrar en el while
_colorLoop2:
    cmp r13d, r15d
    ; while yp < y1 siga, else termina
    jge _final

    cmp r14d, 0
    ; if d =< 0 se colorea NE else se colorea N
    jle _colorNE2
    jmp _colorN2

_colorN2:
    ; pintar N, yp++, d = d+delta_n
    add r13d, 1
    add r14d, ebx

    jmp _plot2

_colorNE2:
    ; pintar NE, xp++, yp++, d = d+delta_ne
    add r12d, 1
    add r13d, 1
    add r14d, ebp

    jmp _plot2

_plot2:
    mov edi, r12d
    mov esi, r13d

    call plot

    jmp _colorLoop2

; - - - - - OCTANTE 3
oct3:
    ; guardar registros que hay que conservar
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    ; alinear stack antes de llamar funciones C
    sub rsp, 8

    ; delta_n = 2*(x0-x1)
    mov eax, edi
    sub eax, edx

    shl eax, 1
    mov ebx, eax

    ; delta_no = 2*((y0-y1)+(x0-x1))
    mov eax, esi
    sub eax, ecx

    mov r8d, edi
    sub r8d, edx

    add eax, r8d

    shl eax, 1
    mov ebp, eax

    ;xp = x0, yp = y0
    mov r12d, edi
    mov r13d, esi

    ; d = (y0-y1) - 2*(x1-x0)
    mov eax, esi
    sub eax, ecx

    mov esi, edx
    sub esi, edi

    shl esi, 1

    sub eax, esi
    mov r14d, eax

    ; Guardar y1
    mov r15d, ecx

    ; plot(xp, yp)
    mov edi, r12d
    mov esi, r13d
    call plot

; entrar en el while
_colorLoop3:
    cmp r13d, r15d
    ; while yp < y1 siga, else termina
    jge _final

    cmp r14d, 0
    ; if d > 0 se colorea NO else se colorea N
    jle _colorN3
    jmp _colorNO3

_colorN3:
    ; pintar N, yp++, d = d+delta_n
    add r13d, 1
    add r14d, ebx

    jmp _plot3

_colorNO3:
    ; pintar NO, xp--, yp++, d = d+delta_no
    sub r12d, 1
    add r13d, 1
    add r14d, ebp

    jmp _plot3

_plot3:
    mov edi, r12d
    mov esi, r13d

    call plot

    jmp _colorLoop3

; - - - - - OCTANTE 4
oct4:
    ; guardar registros que hay que conservar
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    ; alinear stack antes de llamar funciones C
    sub rsp, 8

    ; delta_o = 2*(y0-y1)
    mov eax, esi
    sub eax, ecx

    shl eax, 1
    mov ebx, eax

    ; delta_no = 2*((y0-y1)+(x0-x1))
    mov eax, esi
    sub eax, ecx

    mov r8d, edi
    sub r8d, edx

    add eax, r8d

    shl eax, 1
    mov ebp, eax

    ;xp = x0, yp = y0
    mov r12d, edi
    mov r13d, esi

    ; d = 2*(y0-y1) - (x1-x0)
    mov eax, esi
    sub eax, ecx

    shl eax, 1

    mov esi, edx
    sub esi, edi

    sub eax, esi
    mov r14d, eax

    ; Guardar x1
    mov r15d, edx

    ; plot(xp, yp)
    mov edi, r12d
    mov esi, r13d
    call plot

; entrar en el while
_colorLoop4:
    cmp r12d, r15d
    ; while xp > x1 siga, else termina
    jle _final

    cmp r14d, 0
    ; if d =< 0 se colorea NO else se colorea O
    jle _colorNO4
    jmp _colorO4

_colorO4:
    ; pintar O, xp--, d = d+delta_o
    sub r12d, 1
    add r14d, ebx

    jmp _plot4

_colorNO4:
    ; pintar NO, xp--, yp++, d = d+delta_no
    sub r12d, 1
    add r13d, 1
    add r14d, ebp

    jmp _plot4

_plot4:
    mov edi, r12d
    mov esi, r13d

    call plot

    jmp _colorLoop4

; - - - - - OCTANTE 5
oct5:
    ; guardar registros que hay que conservar
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    ; alinear stack antes de llamar funciones C
    sub rsp, 8

    ; delta_S = 2*(y1-y0)
    mov eax, ecx
    sub eax, esi

    shl eax, 1
    mov ebx, eax

    ; delta_SO = 2*((y1-y0)-(x1-x0))
    mov eax, ecx
    sub eax, esi

    mov r8d, edx
    sub r8d, edi

    sub eax, r8d

    shl eax, 1
    mov ebp, eax

    ;xp = x0, yp = y0
    mov r12d, edi
    mov r13d, esi

    ; d = 2*(y1-y0) - (x1-x0)
    mov eax, ecx
    sub eax, esi

    shl eax, 1

    mov esi, edx
    sub esi, edi

    sub eax, esi
    mov r14d, eax

    ; Guardar x1
    mov r15d, edx

    ; plot(xp, yp)
    mov edi, r12d
    mov esi, r13d
    call plot

    ; entrar en el while
_colorLoop5:
    cmp r12d, r15d
    ; Terminar cuando xp <= x1
    jle _final

    cmp r14d, 0
    ; if d>=0 se colorea O else se colorea SO
    jge _colorO5
    jmp _colorSO5

_colorO5:
    ; pintar O, xp--, d = d+delta_O
    sub r12d, 1
    add r14d, ebx

    jmp _plot5

_colorSO5:
    ; pintar SO, xp--, yp--, d = d+delta_SO
    sub r12d, 1
    sub r13d, 1
    add r14d, ebp

    jmp _plot5

_plot5:
    mov edi, r12d
    mov esi, r13d

    call plot

    jmp _colorLoop5
    
; - - - - - OCTANTE 6
oct6:
    ; guardar registros que hay que conservar
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    ; alinear stack antes de llamar funciones C
    sub rsp, 8

    ; delta_S = 2*(x0-x1)
    mov eax, edi
    sub eax, edx

    shl eax, 1
    mov ebx, eax

    ; delta_SO = 2*((x0-x1)-(y0-y1))
    mov eax, edi
    sub eax, edx

    mov r8d, esi
    sub r8d, ecx

    sub eax, r8d

    shl eax, 1
    mov ebp, eax

    ;xp = x0, yp = y0
    mov r12d, edi
    mov r13d, esi

    ; d = 2*(x0-x1) - (y0-y1)
    mov eax, edi
    sub eax, edx

    shl eax, 1

    sub eax, r8d
    mov r14d, eax

    ; Guardar y1
    mov r15d, ecx

    ; plot(xp, yp)
    mov edi, r12d
    mov esi, r13d
    call plot

    ; entrar en el while
_colorLoop6:
    cmp r13d, r15d
    ; while yp > y1 siga, else termina
    jle _final

    cmp r14d, 0
    ; if d > 0 se colorea SO else se colorea S
    jg _colorSO6
    jmp _colorS6

_colorS6:
    ; pintar S, yp--, d = d+delta_S
    sub r13d, 1
    add r14d, ebx

    jmp _plot6

_colorSO6:
    ; pintar SO, xp--, yp--, d = d+delta_SO
    sub r12d, 1
    sub r13d, 1
    add r14d, ebp

    jmp _plot6

_plot6:
    mov edi, r12d
    mov esi, r13d

    call plot

    jmp _colorLoop6



; - - - - - OCTANTE 7
oct7:

    ; guardar registros que hay que conservar
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    ; alinear stack antes de llamar funciones C
    sub rsp, 8

    ; delta_S = 2*(x1-x0)
    mov eax, edx
    sub eax, edi

    shl eax, 1
    mov ebx, eax

    ; delta_SE = 2*((x1-x0)-(y0-y1))
    mov eax, edx
    sub eax, edi

    mov r8d, esi
    sub r8d, ecx

    sub eax, r8d

    shl eax, 1
    mov ebp, eax

    ;xp = x0, yp = y0
    mov r12d, edi
    mov r13d, esi

    ; d = 2*(x1-x0) - (y0-y1)
    mov eax, edx
    sub eax, edi

    shl eax, 1
    
    mov r8d, esi
    sub r8d, ecx

    sub eax, r8d
    mov r14d, eax

    ; Guardar y1
    mov r15d, ecx

    ; plot(xp, yp)
    mov edi, r12d
    mov esi, r13d
    call plot

    ; entrar en el while
_colorLoop7:
    cmp r13d, r15d
    ; while yp > y1 siga, else termina
    jle _final

    cmp r14d, 0
    ; if d>=0 se colorea SE else se colorea S
    jge _colorSE7
    jmp _colorS7

_colorS7:
    ; pintar S, yp--, d = d+delta_S
    sub r13d, 1
    add r14d, ebx

    jmp _plot7

_colorSE7:
    ; pintar SO, xp++, yp--, d = d+delta_SE
    add r12d, 1
    sub r13d, 1
    add r14d, ebp

    jmp _plot7

_plot7:
    mov edi, r12d
    mov esi, r13d

    call plot

    jmp _colorLoop7

; - - - - - OCTANTE 8
oct8:
    ; guardar registros que hay que conservar
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    ; alinear stack antes de llamar funciones C
    sub rsp, 8

    ; delta_e = 2*(y0-y1)
    mov eax, esi
    sub eax, ecx

    shl eax, 1
    mov ebx, eax


    ; delta_se = 2*((y0-y1)-(x1-x0))
    mov eax, esi
    sub eax, ecx

    mov r8d, edx
    sub r8d, edi

    sub eax, r8d

    shl eax, 1
    mov ebp, eax


    ; xp = x0, yp = y0

    mov r12d, edi
    mov r13d, esi


    ; d = 2*(y0-y1) - (x1-x0)
    mov eax, esi
    sub eax, ecx

    shl eax, 1

    mov r8d, edx
    sub r8d, edi

    sub eax, r8d

    mov r14d, eax

    mov r15d, edx


    ; plot(xp, yp)
    mov edi, r12d
    mov esi, r13d
    call plot

; while (xp < x1)
_colorLoop8:
    cmp r12d, r15d
    jge _final

    ; if (d <= 0) ir E
    cmp r14d, 0
    jle _colorE8

    ; else ir SE
    jmp _colorSE8

; pintar E, xp++, d = d + delta_e
_colorE8:
    add r12d, 1
    add r14d, ebx

    jmp _plot8

; pintar SE, xp++, yp--, d = d + delta_se
_colorSE8:
    add r12d, 1
    sub r13d, 1

    add r14d, ebp
    jmp _plot8

; plot(xp, yp)
_plot8:
    mov edi, r12d
    mov esi, r13d

    call plot

    jmp _colorLoop8



section .note.GNU-stack noalloc noexec nowrite progbits
