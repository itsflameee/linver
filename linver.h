#ifndef LINVER_H
#define LINVER_H

#include <QMainWindow>
#include <QMap>

QT_BEGIN_NAMESPACE
namespace Ui { class linver; }
QT_END_NAMESPACE

class linver : public QMainWindow
{
    Q_OBJECT

public:
    linver(QWidget *parent = nullptr);
    ~linver();

private:
    Ui::linver *ui;

    QMap<QString, QString> readOsRelease();
    void setupDistroLogo();
    void setupTexts();
};

#endif // LINVER_H
