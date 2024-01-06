#ifndef ERRORCOUNTERDIALOG_H
#define ERRORCOUNTERDIALOG_H

#include <QDialog>
#include <QMap>

namespace Ui {
class ErrorCounterDialog;
}

class ErrorCounterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ErrorCounterDialog(QWidget *parent = nullptr);
    ~ErrorCounterDialog();

protected:
    void showEvent(QShowEvent *event) override;

public slots:
    void increaseErrorCount(int error_code);

private:
    Ui::ErrorCounterDialog *ui;
    QMap<int,int> m_error_count_map;
};

#endif // ERRORCOUNTERDIALOG_H
