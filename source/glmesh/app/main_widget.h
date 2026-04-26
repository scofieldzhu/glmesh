

#ifndef __main_widget_h__
#define __main_widget_h__

#include <QMainWindow>
#include "ui_main_widget.h"

class MainWidget : public QMainWindow
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    ~MainWidget();

private slots:
    void onLoadPlyBtnClicked();

private:
    Ui::MainWidget ui_;
};

#endif