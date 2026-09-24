#include "Chip8Display.h"

void Chip8Display::closeEvent(QCloseEvent *event) {
    this->chip8.reset();
    close();
}