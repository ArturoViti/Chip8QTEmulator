#include <QDesktopServices>
#include <QUrl>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QAudioFormat>
#include <QAudioSink>
#include <QMediaDevices>


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "Chip8/Chip8Sound.h"
#include "Logging/LogHandler.h"

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
            this, tr("Open CHIP-8 ROM"), QDir::homePath(),
            tr("All Files (*);;CHIP-8 ROM (*.ch8 *.c8)")
        );

        if ( !fileName.isEmpty() )
            loadRom(fileName);
    });

    cpuTimer.setTimerType(Qt::PreciseTimer);
    connect(&cpuTimer, &QTimer::timeout, this, &MainWindow::step);

    // Setup Sound
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);

    const QAudioDevice device = QMediaDevices::defaultAudioOutput();
    if (!device.isFormatSupported(format))
        qCritical(Log::EmulatorLog) << "Sound not work";

    this->sound = new Chip8Sound(440.0, format, this);
    this->audioSink = new QAudioSink(device, format, this);
    this->audioSink->setBufferSize(format.bytesForDuration(50000));
    this->audioSink->start(this->sound);
}

void MainWindow::loadRom(const QString &path) {
    cpuTimer.stop();
    sound->stop();

    try
    {
        this->chip8.reset();
        this->chip8.load(path.toStdString());
        qInfo(Log::EmulatorLog) << "ROM Loaded Successfully." << path.toStdString();
        display->setFocus();
        display->update();
        cpuTimer.start(FRAME_MS);
        qInfo(Log::Chip8Log) << "Chip8 Started.";
    }
    catch (const std::exception &e) { qFatal(Log::EmulatorLog) << e.what(); }
}

void MainWindow::step() {
    try
    {
        for ( int i = 0; i < INSTRUCTIONS_PER_FRAME; ++i )
            chip8.fetchAndExec();

        chip8.updateTimers();
        this->sound->beepFor(chip8.getSoundTimer());   // PRIMA del decremento
        display->update();
    }
    catch ( const std::exception &e ) { cpuTimer.stop(); sound->stop(); qFatal(Log::Chip8Log) << e.what(); }
}

MainWindow::~MainWindow() {
    qInfo(Log::EmulatorLog) << "Emulator ended.";
    delete ui;
}