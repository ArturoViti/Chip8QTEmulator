#ifndef CHIP8QTEMULATOR_CHIP8DISPLAY_H
#define CHIP8QTEMULATOR_CHIP8DISPLAY_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include "Chip8/Chip8.h"

class Chip8Display : public QWidget {
    public:
        explicit Chip8Display(Chip8 &chip8, const int scale = 20, QWidget *parent = nullptr)
            : QWidget(parent), chip8(chip8), scale(scale) {
                setFixedSize(CHIP8CONF::DISPLAY_WIDTH * scale, CHIP8CONF::DISPLAY_HEIGHT * scale);
                setWindowTitle("CHIP-8");
        }

    protected:
        void paintEvent(QPaintEvent *) override {
            QPainter painter(this);
            painter.fillRect(rect(), Qt::black);

            for (int x = 0; x < CHIP8CONF::DISPLAY_WIDTH; ++x)
                for (int y = 0; y < CHIP8CONF::DISPLAY_HEIGHT; ++y)
                    if (chip8.GB[x][y])
                        painter.fillRect(x * scale, y * scale, scale, scale, Qt::white);
        }
        void keyPressEvent(QKeyEvent *e) override   { setKey(e, 1); }
        void keyReleaseEvent(QKeyEvent *e) override { setKey(e, 0); }
        void closeEvent(QCloseEvent *event) override;

    private:
        Chip8 &chip8;
        int scale;
        void setKey(QKeyEvent *e, uint8_t value) {
            if (e->isAutoRepeat()) return;            // ignora la ripetizione del SO
            if (const int k = keyIndex(e->key()); k >= 0) chip8.KB[k] = value;
            else QWidget::keyPressEvent(e);
        }

        static int keyIndex(const int key) {
            switch (key) {
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
};

#endif