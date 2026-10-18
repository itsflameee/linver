#include "linver.h"
#include "ui_linver.h"

#include <QFile>
#include <QTextStream>
#include <QIcon>
#include <QPixmap>
#include <QSysInfo>
#include <unistd.h>

linver::linver(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::linver)
{
    ui->setupUi(this);

    // Оставляем только кнопку закрытия на окне
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);

    // Закрытие приложения по нажатию на кнопку OK
    connect(ui->ok, &QDialogButtonBox::accepted, this, &QMainWindow::close);

    // Заполнение логотипа и всех текстовых полей
    setupDistroLogo();
    setupTexts();
}

linver::~linver()
{
    delete ui;
}

QMap<QString, QString> linver::readOsRelease()
{
    QMap<QString, QString> data;
    QFile file("/etc/os-release");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (line.contains('=')) {
                QString key = line.section('=', 0, 0).trimmed();
                QString val = line.section('=', 1).trimmed();
                val.remove('"');
                val.remove('\'');
                data[key] = val;
            }
        }
    }
    return data;
}

void linver::setupDistroLogo()
{
    auto osData = readOsRelease();
    QString logoName = osData.value("LOGO");
    QPixmap pixmap;

    if (!logoName.isEmpty()) {
        QIcon icon = QIcon::fromTheme(logoName);
        if (!icon.isNull()) {
            pixmap = icon.pixmap(ui->logo_label->size());
        }
        if (pixmap.isNull()) {
            QString path = QString("/usr/share/pixmaps/%1.png").arg(logoName);
            if (QFile::exists(path)) {
                pixmap.load(path);
            }
        }
    }

    if (pixmap.isNull()) {
        pixmap = QIcon::fromTheme("distributor-logo").pixmap(ui->logo_label->size());
    }

    if (!pixmap.isNull()) {
        ui->logo_label->setPixmap(pixmap.scaled(ui->logo_label->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}

void linver::setupTexts()
{
    auto osData = readOsRelease();

    // 1. Извлечение параметров системы
    QString osName = osData.value("PRETTY_NAME", osData.value("NAME", "Linux"));
    QString osVersion = osData.value("VERSION_ID", "rolling");
    QString kernelVer = QSysInfo::kernelVersion();

    // Вендор
    QString osVendor = osData.value("HOME_URL", "The Open Source Community");

    // Графическое окружение
    QString de = qEnvironmentVariable("XDG_CURRENT_DESKTOP");
    if (de.isEmpty()) {
        de = qEnvironmentVariable("DESKTOP_SESSION");
    }
    if (de.isEmpty()) {
        de = "TTY";
    }

    // Имя пользователя и хост
    QString user = qEnvironmentVariable("USER");
    if (user.isEmpty()) {
        user = "root";
    }
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        hostname[sizeof(hostname) - 1] = '\0';
    } else {
        strncpy(hostname, "localhost", sizeof(hostname));
    }
    QString host = QString::fromLocal8Bit(hostname);


    // 2. Установка значений по объектам формы

    // Блок версий
    ui->osrelease->setText(osName);
    ui->version->setText(QString("Version %1 (Kernel %2)").arg(osVersion, kernelVer));
    ui->osvendor->setText(QString("© %1. All rights reversed.").arg(osVendor));

    // Средний блок дисклеймера
    ui->text_microslop->setText(
        QString("The %1 %2 operating system and %3 are not protected by trademark "
                "and other pending or existing intellectual property rights in the "
                "United States and other countries/regions.\n\n"
                "The %1 is distributed under the GNU GPL.")
            .arg(osName, osVersion, de)
        );

    // Нижний блок пользователя и хоста
    ui->text_license->setText(
        QString("This product is not licensed under the Microsoft Software License Terms to:\n\n%1\n%2")
            .arg(user, host)
        );
}
