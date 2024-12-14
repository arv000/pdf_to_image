#ifndef WORKTHREAD_H
#define WORKTHREAD_H
#include <QThread>

enum readability{
    LOW_DEFINITION = 0, /* 普通清晰度 */
    NORMAL_DEFINITION,  /* 标准清晰度 */
    HIGHT_DEFINITION    /* 高级清晰度 */
};

class WorkThread: public QThread
{
    Q_OBJECT
public:
    WorkThread();
    void setParam(QString filePath,QString outputDir);
    void setOutFormat(QString out_format);
    void setReadability(readability r);
private:
    QString strFilePath;
    QString strOutputDir;
    QString strOutFormat;
    readability eReadability;
    double getReadabilityValue();
signals:
    void updateProgress(int value);
    void finishWork();
    // QThread interface
protected:
    void run() override;
};

#endif // WORKTHREAD_H
