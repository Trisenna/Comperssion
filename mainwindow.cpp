#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {
    ui->setupUi(this);
    com = new Compression();
    connect(com, &Compression::error, this, &MainWindow::clear);
    connect(com, SIGNAL(mysignal(double)), this, SLOT(myslot(double)));
    ui->progressBar->setMaximum(100);
    ui->progressBar->setMinimum(0);
    ui->progressBar->setValue(0);

}

MainWindow::~MainWindow() {
    delete ui;
    delete com;
}

void MainWindow::clear() {
    ui->lineEdit->clear();
    path.clear();
}

void MainWindow::myslot(double per) {
    if(per > ui->progressBar->value())
        ui->progressBar->setValue(per);
}

void MainWindow::on_pushButton_open_clicked() {
    path = QFileDialog::getOpenFileName(this, QString("选择文件"));

    ui->lineEdit->setText(path);
}



void MainWindow::on_pushButton_compression_clicked() {
    pathAfter = ui->lineEdit_2->text();
    com->zip(path, pathAfter);
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    this->path.clear();
    this->pathAfter.clear();
    ui->progressBar->setValue(0);
}

void MainWindow::on_pushButton_decompression_clicked() {
    pathAfter = ui->lineEdit_2->text();
    com->unzip(path, pathAfter);
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();

    this->path.clear();
    this->pathAfter.clear();
    ui->progressBar->setValue(0);
}


void MainWindow::on_pushButton_open_2_clicked() {

}


void MainWindow::on_pushButton_clicked() {
    pathAfter = QFileDialog::getExistingDirectory(this, QString("决定操作后的文件位置"));
    ui->lineEdit_2->setText(pathAfter);
}

