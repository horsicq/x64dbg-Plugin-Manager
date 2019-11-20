#ifndef DIALOGREMOVEMODULEPROCESS_H
#define DIALOGREMOVEMODULEPROCESS_H

#include <QDialog>

namespace Ui {
class DialogRemoveModuleProcess;
}

class DialogRemoveModuleProcess : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRemoveModuleProcess(QWidget *parent = nullptr);
    ~DialogRemoveModuleProcess();

private:
    Ui::DialogRemoveModuleProcess *ui;
};

#endif // DIALOGREMOVEMODULEPROCESS_H
