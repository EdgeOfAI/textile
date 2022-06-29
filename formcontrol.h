#ifndef FORMCONTROL_H
#define FORMCONTROL_H

#include <QDialog>

namespace Ui {
class FormControl;
}

class FormControl : public QDialog
{
    Q_OBJECT

public:
    explicit FormControl(QWidget *parent = nullptr);
    ~FormControl();

private:
    Ui::FormControl *ui;
};

#endif // FORMCONTROL_H
