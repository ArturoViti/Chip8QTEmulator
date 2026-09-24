#include "Chip8Display.h"

void Chip8Display::closeEvent(QCloseEvent *event) { this->chip8.reset(); close(); }

void Chip8Display::paintEvent(QPaintEvent *)  {
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);

    const int scale = std::max(
        1, std::min(width()  / CHIP8CONF::DISPLAY_WIDTH, height() / CHIP8CONF::DISPLAY_HEIGHT)
    );
    const int offX = (width()  - CHIP8CONF::DISPLAY_WIDTH  * scale) / 2;
    const int offY = (height() - CHIP8CONF::DISPLAY_HEIGHT * scale) / 2;

    for ( int x = 0; x < CHIP8CONF::DISPLAY_WIDTH; ++x )
        for ( int y = 0; y < CHIP8CONF::DISPLAY_HEIGHT; ++y )
            if ( chip8.GB[x][y] )
                painter.fillRect(offX + x * scale, offY + y * scale, scale, scale, Qt::white);
}


void Chip8Display::setKey( QKeyEvent *e, const uint8_t value ) const {
    if ( e->isAutoRepeat() ) return;
    if ( const int k = keyIndex(e->key()); k >= 0 ) chip8.KB[k] = value;
    else e->ignore();
}


int Chip8Display::keyIndex( const int key ) {
    switch (key)
    {
        case Qt::Key_1: return 0x1; case Qt::Key_2: return 0x2;
        case Qt::Key_3: return 0x3; case Qt::Key_4: return 0xC;
        case Qt::Key_Q: return 0x4; case Qt::Key_W: return 0x5;
        case Qt::Key_E: return 0x6; case Qt::Key_R: return 0xD;
        case Qt::Key_A: return 0x7; case Qt::Key_S: return 0x8;
        case Qt::Key_D: return 0x9; case Qt::Key_F: return 0xE;
        case Qt::Key_Z: return 0xA; case Qt::Key_X: return 0x0;
        case Qt::Key_C: return 0xB; case Qt::Key_V: return 0xF;
        default: return -1;
    }
}