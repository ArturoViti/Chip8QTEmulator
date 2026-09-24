#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>

#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace {
    constexpr int INSTRUCTIONS_PER_FRAME = 10;   // ~600 instructions at 60 Hz
    constexpr int FRAME_MS = 16;                 // ~60 Hz
}

MainWindow::MainWindow(Chip8 &chip8, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , chip8(chip8)
    , display(new Chip8Display(chip8, this))
{
    ui->setupUi(this);
    setWindowTitle("Chip8QTEmulator - Play your favorite CHIP-8 classic games");

    setCentralWidget(display);
    adjustSize();

    connect(ui->actionDeveloper, &QAction::triggered, this, [] {
        QDesktopServices::openUrl(QUrl("https://www.linkedin.com/in/arturo-viti-a426bb174/"));
    });
    connect(ui->actionGitHub, &QAction::triggered, this, [] {
        QDesktopServices::openUrl(QUrl("https://github.com/ArturoViti"));
    });
    connect(ui->actionLoad_ROM_File, &QAction::triggered, this, [this] {
        const QString fileName = QFileDialog::getOpenFileName(
            this, tr("Apri ROM CHIP-8"), QDir::homePath(),
            tr("CHIP-8 ROM (*.ch8 *.c8);;Tutti i file (*)")
        );

        if (!fileName.isEmpty())
            loadRom(fileName);
    });

    connect(&cpuTimer, &QTimer::timeout, this, &MainWindow::step);
}

void MainWindow::loadRom(const QString &path) {
    cpuTimer.stop();

    try
    {
        this->chip8.reset();
        this->chip8.load(path.toStdString());
        display->setFocus();
        display->update();
        cpuTimer.start(FRAME_MS);
    }
    catch (const std::exception &e)
    {
        QMessageBox::critical(this, "Errore", e.what());
    }
}

void MainWindow::step() {
    try
    {
        for ( int i = 0; i < INSTRUCTIONS_PER_FRAME; ++i )
            chip8.fetchAndExec();

        chip8.updateTimers();
        display->update();
    }
    catch (const std::exception &e)
    {
        cpuTimer.stop();
        QMessageBox::critical(this, "Errore di esecuzione", e.what());
    }
}

MainWindow::~MainWindow() { delete ui; }