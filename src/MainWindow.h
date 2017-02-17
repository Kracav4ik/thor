#pragma once

#include <QMainWindow>

#include "ui_thor.h"


class MainWindow : public QMainWindow, private Ui::MainWindow {
public:
    MainWindow();

};

