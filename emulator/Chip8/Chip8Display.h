#ifndef CHIP8QTEMULATOR_CHIP8DISPLAY_H
#define CHIP8QTEMULATOR_CHIP8DISPLAY_H

#include <QWidget>
#include <QKeyEvent>
#include <QPainter>
#include "Chip8/Chip8.h"

class Chip8Display : public QWidget {

    public:
        explicit Chip8Display(Chip8 &chip8, QWidget *parent = nullptr): QWidget(parent), chip8(chip8) {
            setFocusPolicy(Qt::StrongFocus);
            setMinimumSize(CHIP8CONF::DISPLAY_WIDTH * 4, CHIP8CONF::DISPLAY_HEIGHT * 4);

            // Set 
        }

        QSize sizeHint() const override {
            return { CHIP8CONF::DISPLAY_WIDTH * 12, CHIP8CONF::DISPLAY_HEIGHT * 12 };
        }

    protected:
        void paintEvent(QPaintEvent *) override;
        void keyPressEvent(QKeyEvent *e) override   { setKey(e, 1); }
        void keyReleaseEvent(QKeyEvent *e) override { setKey(e, 0); }
        void closeEvent(QCloseEvent *event) override;

    private:
        Chip8 &chip8;
        void setKey( QKeyEvent *e, uint8_t value ) const;
        static int keyIndex(const int key);
};

#endif