#include <QApplication>
#include <QPushButton>
#include <QTimer>
#include <QMessageBox>
#include <QDir>

#include "Chip8/Chip8.h"
#include "Chip8/Chip8Display.h"
#include <Exception/IllegalInstructionException.h>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    #ifdef Q_OS_MACOS
        a.setWindowIcon(QIcon(":/icons/chip8-macos-1024.png"));
    #else
        a.setWindowIcon(QIcon(":/icons/chip8-256.png"));
    #endif

    Chip8 chip8;
    Chip8Display display(chip8);
    QTimer cpuTimer;
    constexpr int INSTRUCTIONS_PER_FRAME = 10;   // ~600 istruzioni/s a 60 Hz

    QObject::connect(&cpuTimer, &QTimer::timeout, [&]() {
        try
        {
            for (int i = 0; i < INSTRUCTIONS_PER_FRAME; ++i)
                chip8.fetchAndExec();

            chip8.updateTimers();
            display.update();
        }
        catch (std::exception &e) {
            cpuTimer.stop();
            qDebug() << "error" << e.what();
            QMessageBox::critical(&display, "Istruzione illegale", e.what());
        }
    });

    QPushButton button("Carica ROM", nullptr);
    button.resize(200, 100);

    QObject::connect(&button, &QPushButton::clicked, [&]() {
        cpuTimer.stop();
        try {
            const QString romPath = QDir(QCoreApplication::applicationDirPath())
                                        .filePath("roms/INVADERS");
            chip8.load(romPath.toStdString());
            display.show();
            cpuTimer.start(16);  // ~60 Hz
        } catch (const std::exception &e) {
            QMessageBox::critical(&button, "Errore", e.what());
        }
    });

    button.show();
    /*MainWindow mainWin;
    mainWin.show();*/
    return QApplication::exec();
}
