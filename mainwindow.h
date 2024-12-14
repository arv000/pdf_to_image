#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLineEdit;
class QPushButton;
class QWidget;
class QVBoxLayout;
class QLabel;
class QProgressBar;
class QComboBox;

class WorkThread;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void slotDoWork();
    void slotSelectPdfFilePath();
    void slotOutputDir();
    void slotupdateProgress(int value);
    void soltFinishWork();
private:
    void initUI();
    void initConnect();
    void initData();
    
    QLabel *labSelectPdfFilePath;
    QLineEdit *edtSelectPdfFilePath;
    QPushButton *btnSelectPdfFilePath;
    QLabel *labOutputDir;
    QLineEdit *edtOutputDir;
    QPushButton *btnOutputDir;
    QPushButton *btn;
    QWidget *main;
    QVBoxLayout *mainLayout;
    WorkThread *workThread;
    QProgressBar *progressBar;
    QLabel *labCombobox;
    QComboBox *combobox;
    QLabel *labReadability;
    QComboBox *comboboxReadability;
};
#endif // MAINWINDOW_H
