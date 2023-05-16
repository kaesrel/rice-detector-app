#ifndef DLGPREFERENCES_H
#define DLGPREFERENCES_H

#include <QDialog>
#include <QLineEdit>

namespace Ui {
class dlgPreferences;
}

class dlgPreferences : public QDialog
{
    Q_OBJECT

public:
    explicit dlgPreferences(QWidget *parent = nullptr);
    ~dlgPreferences();
private:
//    void browse(QLineEdit *lineEdit, const QString &filter,const QString &settingStr);

private slots:
//    void on_pushButton_clicked();

//    void on_pushButton_2_clicked();

//    void on_pushButton_3_clicked();

    void on_buttonBox_accepted();

//    void on_pushButton_clicked();

private:
    Ui::dlgPreferences *ui;
};

#endif // DLGPREFERENCES_H
