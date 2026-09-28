#include <QApplication>
#include <QPushButton>
#include <QTimer>
#include <QMessageBox>
#include <QDir>

#include "Chip8/Chip8.h"
#include "Chip8/Chip8Display.h"
#include "ui/mainwindow.h"
#include <Exception/IllegalInstructionException.h>

#include "Logging/LogHandler.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    #ifdef Q_OS_MACOS
        a.setWindowIcon(QIcon(":/icons/chip8-macos-1024.png"));
    #else
        a.setWindowIcon(QIcon(":/icons/chip8-256.png"));
    #endif
    a.setApplicationName("Chip8QTEmulator");
    a.setApplicationDisplayName("Chip8QTEmulator");

    qInstallMessageHandler(Log::messageHandler);
    qSetMessagePattern(Log::LOG_FORMAT.data());
    qInfo(Log::EmulatorLog) << "Emulator started.";

    Chip8 chip8;
    MainWindow mainWin(chip8);
    mainWin.show();

    return QApplication::exec();
}
