#include "mainwindow.h"
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDebug>
#include <QPdfWriter>
#include <QPainter>
#include <QFileDialog>
#include <QProgressBar>
#include <QMessageBox>
#include <QComboBox>

#include <vector>
#include <string>

#include "workthread.h"
#include "poppler-image.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , labSelectPdfFilePath(new QLabel(QStringLiteral("选择DPF文件")))
    , edtSelectPdfFilePath(new QLineEdit())
    , btnSelectPdfFilePath(new QPushButton(QStringLiteral("...")))
    , labOutputDir(new QLabel(QStringLiteral("选择保存目录")))
    , edtOutputDir(new QLineEdit())
    , btnOutputDir(new QPushButton(QStringLiteral("...")))
    , btn(new QPushButton(QStringLiteral("生成")))
    , main(new QWidget(this))
    , mainLayout(new QVBoxLayout)
    , workThread (new WorkThread())
    , progressBar(new QProgressBar())
    , labCombobox(new QLabel(QStringLiteral("图片类型")))
    , combobox(new QComboBox())
    , labReadability(new QLabel(QStringLiteral("清晰度：")))
    , comboboxReadability(new QComboBox())
{
    initUI();
    initConnect();
    initData();
}

MainWindow::~MainWindow() {}

void MainWindow::slotDoWork()
{
     
    QString filePath = edtSelectPdfFilePath->text();
    QString saveDir =edtOutputDir->text();
    if(filePath.isEmpty()){
        QMessageBox::information(this,"提示","dpf文件没有选择");
        return;
    }
     if(saveDir.isEmpty()){
        QMessageBox::information(this,"提示","保存目录没有选择");
        return;
    }
    QString out_format =  combobox->currentText();
    btn->setDisabled(true);
    workThread->setParam(filePath,saveDir);
    workThread->setOutFormat(out_format);
    workThread->setReadability((readability)comboboxReadability->currentData().toInt());
    workThread->start();
}

void MainWindow::slotSelectPdfFilePath()
{
    QString strSelectPath = QFileDialog::getOpenFileName();
    edtSelectPdfFilePath->setText(strSelectPath);
}

void MainWindow::slotOutputDir()
{
    QString strOutputDir = QFileDialog::getExistingDirectory();
    edtOutputDir->setText(strOutputDir);
}

void MainWindow::slotupdateProgress(int value)
{
    progressBar->setValue(value);
}

void MainWindow::soltFinishWork()
{
    QMessageBox::information(this,"提示","任务完成！");
    btn->setDisabled(false);
}

void MainWindow::initUI()
{
    setWindowTitle(QStringLiteral("PDT转图工具"));
    setWindowIcon(QIcon(":/images/logo.ico"));
    QGridLayout *mainLayout = new QGridLayout();
     mainLayout->addWidget(labSelectPdfFilePath,0,0);
     mainLayout->addWidget(edtSelectPdfFilePath,0,1);
     mainLayout->addWidget(btnSelectPdfFilePath,0,2);

    mainLayout->addWidget(labOutputDir,1,0);
    mainLayout->addWidget(edtOutputDir,1,1);
    mainLayout->addWidget(btnOutputDir,1,2);
    
    mainLayout->addWidget(labCombobox ,2,0);
    mainLayout->addWidget(combobox,2,1);

    mainLayout->addWidget(labReadability ,3,0);
    mainLayout->addWidget(comboboxReadability,3,1);
    mainLayout->addWidget(btn,3,2);

    mainLayout->addWidget(progressBar,4,0,3,0);
    progressBar->setValue(0);
    btn->setStyleSheet("QPushButton { background-color: green; color: white; }"
                               "QPushButton:hover { background-color: darkgreen; }"
                               "QPushButton:pressed { background-color: darkgreen; }");
    main->setLayout(mainLayout);
    main->setMinimumWidth(600);
    setCentralWidget(main);
}

void MainWindow::initConnect()
{
    connect(btn,&QPushButton::clicked,this,&MainWindow::slotDoWork);
    connect(btnSelectPdfFilePath,&QPushButton::clicked,this,&MainWindow::slotSelectPdfFilePath);
    connect(btnOutputDir,&QPushButton::clicked,this,&MainWindow::slotOutputDir);
    connect(workThread,&WorkThread::updateProgress,this,&MainWindow::slotupdateProgress);
    connect(workThread,&WorkThread::finishWork,this,&MainWindow::soltFinishWork);
}

void   MainWindow::initData(){
    std::vector<std::string> supported_image_formats = poppler::image::supported_image_formats();
    for(auto item : supported_image_formats){
    combobox->addItem(QString(item.c_str()),QVariant(QString(item.c_str())));
    }
    comboboxReadability->addItem(QString("普通清晰度"),QVariant(LOW_DEFINITION));
    comboboxReadability->addItem(QString("标准清晰度"),QVariant(NORMAL_DEFINITION));
    comboboxReadability->addItem(QString("高级清晰度"),QVariant(HIGHT_DEFINITION));
    comboboxReadability->setCurrentIndex(1);
}