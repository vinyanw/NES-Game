void main(void) {
    unsigned char x = 0;
    unsigned int i;

    // 1. Espera o PPU estar pronto
    while(!(*((volatile unsigned char*)0x2002) & 0x80));

    // 2. Define as Cores (Paleta)
    *((volatile unsigned char*)0x2006) = 0x3f; // Endereço da Paleta
    *((volatile unsigned char*)0x2006) = 0x00;
    *((volatile unsigned char*)0x2007) = 0x21; // Cor 0: Azul Céu
    *((volatile unsigned char*)0x2006) = 0x3f;
    *((volatile unsigned char*)0x2006) = 0x01;
    *((volatile unsigned char*)0x2007) = 0x17; // Cor 1: Marrom Chão

    // 3. Desenha o chão na linha 20 da tela
    // O endereço 0x2280 corresponde à linha 20 da Nametable A
    *((volatile unsigned char*)0x2006) = 0x22;
    *((volatile unsigned char*)0x2006) = 0x80;
    for(i=0; i<64; i++) {
        *((volatile unsigned char*)0x2007) = 0x01; // Coloca o tile do chão
    }

    // 4. Ativa o Background
    *((volatile unsigned char*)0x2001) = 0x08;

    while(1) {
        // Espera o fim do desenho da tela (V-Blank)
        while(!(*((volatile unsigned char*)0x2002) & 0x80));
        
        x++; // Aumenta o deslocamento
        
        *((volatile unsigned char*)0x2002);     // Reset do latch do PPU
        *((volatile unsigned char*)0x2005) = x;    // Scroll Horizontal
        *((volatile unsigned char*)0x2005) = 0x00; // Scroll Vertical
    }
}