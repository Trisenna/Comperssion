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
    option = 0;
    path = QFileDialog::getOpenFileName(this, QString("选择文件"));

    ui->lineEdit->setText(path);
}



void MainWindow::on_pushButton_compression_clicked() {
    if(option == 0) {
        pathAfter = ui->lineEdit_2->text();
        com->zip(path, pathAfter);
        ui->lineEdit->clear();
        ui->lineEdit_2->clear();
        ui->lineEdit_5->clear();
        ui->lineEdit_6->clear();
        ui->lineEdit->clear();
        ui->lineEdit_2->clear();
        this->path.clear();
        this->pathAfter.clear();
        ui->progressBar->setValue(0);
    } else {
        pathAfter = ui->lineEdit_6->text();
        com->zipFolder(path, pathAfter);
        ui->lineEdit_5->clear();
        ui->lineEdit_6->clear();
        ui->lineEdit_5->clear();
        ui->lineEdit_6->clear();
        ui->lineEdit->clear();
        ui->lineEdit_2->clear();
        this->path.clear();
        this->pathAfter.clear();
        ui->progressBar->setValue(0);
    }
    option = 0;
}

void MainWindow::on_pushButton_decompression_clicked() {
    if(option == 0) {
        pathAfter = ui->lineEdit_2->text();
        com->unzip(path, pathAfter);
        ui->lineEdit->clear();
        ui->lineEdit_2->clear();
        ui->lineEdit_5->clear();
        ui->lineEdit_6->clear();
        ui->lineEdit->clear();
        ui->lineEdit_2->clear();
        this->path.clear();
        this->pathAfter.clear();
        ui->progressBar->setValue(0);
    } else {
        pathAfter = ui->lineEdit_6->text();
        com->unzipDir(path, pathAfter);
        ui->lineEdit_5->clear();
        ui->lineEdit_6->clear();
        ui->lineEdit->clear();
        ui->lineEdit_2->clear();

        this->path.clear();
        this->pathAfter.clear();
        ui->progressBar->setValue(0);
    }
}


void MainWindow::on_pushButton_open_2_clicked() {

}


void MainWindow::on_pushButton_clicked() {
    option = 0;
    pathAfter = QFileDialog::getExistingDirectory(this, QString("决定操作后的文件位置"));
    ui->lineEdit_2->setText(pathAfter);
}


void MainWindow::on_pushButton_open_3_clicked() {
    option = 1;
    path = QFileDialog::getExistingDirectory(this, QString("选择文件夹"));

    ui->lineEdit_5->setText(path);
}


void MainWindow::on_pushButton_3_clicked() {
    option = 1;
    pathAfter = QFileDialog::getExistingDirectory(this, QString("决定操作后的文件位置"));
    ui->lineEdit_6->setText(pathAfter);
}

