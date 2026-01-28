.export _reset, __STARTUP__ : absolute = 1
.import _main

.segment "HEADER"
    .byte "NES", $1a
    .byte $02 ; 32KB PRG
    .byte $01 ; 8KB CHR
    .byte $00 ; Mapper 0
    .byte $00 ; Mirroring
    .byte $00, $00, $00, $00, $00, $00, $00, $00 ; Padding até 16 bytes

.segment "STARTUP"
_reset:
    sei
    cld
    ldx #$ff
    txs
@wait1: bit $2002
    bpl @wait1
@wait2: bit $2002
    bpl @wait2
    jmp _main

.segment "VECTORS"
    .word 0      ; NMI
    .word _reset ; RESET
    .word 0      ; IRQ

.segment "CHARS"
    .res 16, $00 ; Tile 0 (Céu)
    .res 16, $ff ; Tile 1 (Chão)
    .res 8192 - 32, $00