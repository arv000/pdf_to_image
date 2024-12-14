#include "workthread.h"
#include <poppler-document.h>
#include <poppler-image.h>
#include <poppler-page.h>
#include <poppler-page-renderer.h>
#include <poppler-page-transition.h>
#include <qdebug.h>
WorkThread::WorkThread()

{

}

void WorkThread::setParam(QString filePath, QString outputDir)
{
    strFilePath = filePath;
    strOutputDir = outputDir;
}
void  WorkThread::setOutFormat(QString out_format){
    strOutFormat = out_format;
}
void WorkThread::setReadability(readability r){
    eReadability = r;
}
double WorkThread::getReadabilityValue(){
    double res = 300;
    switch (eReadability)
    {
    case LOW_DEFINITION:
        res = 72;
        break;
    case NORMAL_DEFINITION:
        res = 300;
        break;
    case HIGHT_DEFINITION:
        res = 450;
        break;
    default:
        break;
    }
    return res;
}
void WorkThread::run()
{
    // 打开 PDF 文档
    poppler::document* document = poppler::document::load_from_file(strFilePath.toStdString());
    if (!document || document->is_locked()) {
        qInfo() << "Error: Unable to open the PDF file or it is locked.\n";
        delete document;
        return ;
    }

    int pageCount = document->pages();
    qInfo() << "Total pages: " << pageCount << "\n";
    double res = getReadabilityValue();
    // 遍历每一页并保存为单独的 PDF 文件
    for (int i = 0; i < pageCount; ++i) {
        poppler::page* page = document->create_page(i);
        if (!page) {
            qInfo() << "Error: Unable to load page " << i + 1 << ".\n";
            continue;
        }
        // 渲染页面到 PDF
        poppler::page_renderer d;
        poppler::image image =  d.render_page(page,res,res);
        QString savePath = strOutputDir +"\\" + QString::number(i)+"."+ strOutFormat;
        image.save(savePath.toStdString(),strOutFormat.toStdString());
        emit updateProgress((i+1)*100/pageCount);
    }

    emit finishWork();
    delete document; // 释放文档
    return ;
}
