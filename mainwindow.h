#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include"compression.h"
#include<QFileDialog>
#include<QMenu>
#include<QAction>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void clear();
  private slots:
    void on_pushButton_open_clicked();

    void on_pushButton_compression_clicked();

    void on_pushButton_decompression_clicked();

    void myslot(double per);

    void on_pushButton_open_2_clicked();

    void on_pushButton_clicked();

  private:
    Ui::MainWindow *ui;
    Compression* com;
    QString path;
    QString pathAfter;
};

#endif // MAINWINDOW_H
