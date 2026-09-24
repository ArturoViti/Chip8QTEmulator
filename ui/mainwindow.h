#pragma once
#include <QMainWindow>
#include <QTimer>
#include "Chip8/Chip8.h"
#include "Chip8/Chip8Display.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(Chip8 &chip8, QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    void loadRom(const QString &path);
    void step();

    Ui::MainWindow *ui;
    Chip8 &chip8;
    Chip8Display *display;
    QTimer cpuTimer;
};