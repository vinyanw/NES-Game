// Defines para registradores do PPU e joypad
#define PPUCTRL   *((unsigned char*)0x2000)
#define PPUMASK   *((unsigned char*)0x2001)
#define PPUSTATUS *((unsigned char*)0x2002)
#define OAMADDR   *((unsigned char*)0x2003)
#define PPUSCROLL *((unsigned char*)0x2005)
#define PPUADDR   *((unsigned char*)0x2006)
#define PPUDATA   *((unsigned char*)0x2007)
#define JOYPAD1   *((unsigned char*)0x4016)
#define OAM_DMA   *((unsigned char*)0x4014)

// Bits do joypad
#define PAD_RIGHT  1
#define PAD_LEFT   2
#define PAD_DOWN   4
#define PAD_UP     8
#define PAD_START 16
#define PAD_SEL   32
#define PAD_B     64
#define PAD_A     128

// Macro para endereço no nametable
#define NTADR(x, y) (0x2000 + ((y) * 32) + (x))

unsigned char pad;

// Funções auxiliares
void ppu_off(void) {
    PPUMASK = 0;
}

void ppu_wait_vblank(void) {
    while ((PPUSTATUS & 0x80) == 0);
}

void ppu_addr(unsigned int adr) {
    PPUADDR = adr >> 8;
    PPUADDR = adr & 0xFF;
}

void ppu_data(const unsigned char* data, unsigned int len) {
    unsigned int i;
    for (i = 0; i < len; ++i) {
        PPUDATA = data[i];
    }
}

void vram_fill(unsigned int adr, unsigned char fill, unsigned int len) {
    unsigned int i;
    ppu_addr(adr);
    for (i = 0; i < len; ++i) {
        PPUDATA = fill;
    }
}

void pal_all(const unsigned char* pal) {
    ppu_addr(0x3f00);
    ppu_data(pal, 32);
}

void ppu_scroll(unsigned char x, unsigned char y) {
    PPUSCROLL = x;
    PPUSCROLL = y;
}

void ppu_on_all(void) {
    PPUCTRL = 0x80;  // NMI on, nametable 0, patterns $0000
    PPUMASK = 0x1E;  // bg + sprites + left 8px on
}

void read_pad(void) {
    unsigned char tmp = 0;
    unsigned char i;
    JOYPAD1 = 1;
    JOYPAD1 = 0;
    for (i = 0; i < 8; ++i) {
        tmp = (tmp << 1) | (JOYPAD1 & 1);
    }
    pad = tmp;
}

// Paleta
const unsigned char palette[32] = {
    0x0f,0x11,0x21,0x30, 0x0f,0x0f,0x0f,0x0f,
    0x0f,0x0f,0x0f,0x0f, 0x0f,0x0f,0x0f,0x0f,
    0x0f,0x16,0x26,0x36, 0x0f,0x0f,0x0f,0x0f,
    0x0f,0x0f,0x0f,0x0f, 0x0f,0x0f,0x0f,0x0f
};

// Tiles no segmento CHARS (CHR-ROM) - usando pragma simples
#pragma rodata-name ("CHARS")

const unsigned char chrtiles[48] = {
    // Tile 0: céu sólido azul
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    // Tile 1: chão sólido marrom
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    // Tile 2: boneco losango vermelho
    0x18,0x3C,0x7E,0xFF,0xFF,0x7E,0x3C,0x18,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

#pragma rodata-name ("RODATA")

void main(void) {
    unsigned char* oam = (unsigned char*)0x0200;
    unsigned char spr_x = 120;
    unsigned int i;

    ppu_off();
    pal_all(palette);

    vram_fill(NTADR(0,0), 0, 26*32);   // céu tile 0
    vram_fill(NTADR(0,26), 1, 64);     // chão tile 1 (2 linhas)
    vram_fill(0x23C0, 0x00, 64);       // atributos paleta 0

    ppu_scroll(0, 0);
    ppu_on_all();

    for (;;) {
        ppu_wait_vblank();
        read_pad();

        if (pad & PAD_RIGHT) ++spr_x;
        if (pad & PAD_LEFT) --spr_x;

        if (spr_x > 240) spr_x = 240;
        if (spr_x < 8) spr_x = 8;

        // Limpa OAM (64 sprites)
        for (i = 0; i < 64; ++i) {
            oam[i*4 + 0] = 0xF0;
        }

        // Sprite 0
        oam[0] = 200;     // Y
        oam[1] = 2;       // tile
        oam[2] = 0x00;    // attr
        oam[3] = spr_x;   // X

        OAMADDR = 0;
        OAM_DMA = 0x02;

        ppu_scroll(0, 0);
    }
}