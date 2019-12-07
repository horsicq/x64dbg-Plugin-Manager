#ifndef DIALOGREMOVEMODULEPROCESS_H
#define DIALOGREMOVEMODULEPROCESS_H

#include <QDialog>
#include <QThread>
#include <QTimer>
#include "../removemoduleprocess.h"
#include "../utils.h"

namespace Ui {
class DialogRemoveModuleProcess;
}

class DialogRemoveModuleProcess : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRemoveModuleProcess(QWidget *parent, Utils::MDATA *pMData);
    ~DialogRemoveModuleProcess();

private slots:
    void on_pushButtonCancel_clicked();
    void onCompleted(qint64 nElapsed);
    void timerSlot();

signals:
    void errorMessage(QString sMessage);

private:
    Ui::DialogRemoveModuleProcess *ui;
    Utils::MDATA *pMData;

    RemoveModuleProcess *pRemoveModuleProcess;
    QThread *pThread;
    bool bIsRun;
    QTimer *pTimer;
};

#endif // DIALOGREMOVEMODULEPROCESS_H
