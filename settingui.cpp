#include "settingui.h"
#include "ui_settingui.h"
#include "zaxiscontrol.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QString>

namespace {
constexpr const char * APP_SETTING = "setting";
constexpr const char * SETTING_ICON = ":/Resource/app/setting/icon.png";
}

settingUi::settingUi(QWidget *parent)
    : softwareUiBase(parent)
    , ui(new Ui::settingUi)
{
    ui->setupUi(this);
    // 路径非法提示信号在 zAxisControl 已注入后由按钮逻辑按需建立连接。
}

settingUi::~settingUi()
{
    delete ui;

}

const QString &settingUi::getSoftname()
{
    static const QString softName = QString::fromUtf8(APP_SETTING);
    return softName;
}

const QPixmap &settingUi::getSofticon()
{
    static const QPixmap softIcon(QString::fromUtf8(SETTING_ICON));
    return softIcon;
}

softwareUiBase *settingUi::getSingleton()
{
    static auto settingApp = new settingUi();
    return settingApp;
}

void settingUi::on_exitButton_clicked()
{
    close();
    emit exit();
}

void settingUi::on_applySwitchDurationButton_clicked()
{
    // 应用壁纸切换时长：UI 用“秒”，内部定时器使用“毫秒”。
    if (!_zAxisCtrl) {
        QMessageBox::warning(this, tr("提示"), tr("zAxisControl 未初始化，无法应用设置。"));
        return;
    }
    const int seconds = ui->switchDurationSpinBox ? ui->switchDurationSpinBox->value() : 5;
    emit _zAxisCtrl->switchWpDur(seconds * 1000);
}


void settingUi::on_changeWallpaperPathButton_clicked()
{
    // 选择壁纸目录并通知外部切换路径。
    if (!_zAxisCtrl) {
        QMessageBox::warning(this, tr("提示"), tr("zAxisControl 未初始化，无法更改壁纸路径。"));
        return;
    }
    const QString dir = QFileDialog::getExistingDirectory(
        this, tr("选择壁纸目录"), QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty()) {
        return;
    }

    // 更新界面显示，便于用户确认当前应用的路径。
    if (ui->wallpaperPathLineEdit) {
        ui->wallpaperPathLineEdit->setText(dir);
    }

    // 路径非法时给出提示，使用 UniqueConnection 防止重复连接弹多次。
    connect(_zAxisCtrl, &zAxisControl::isPathInvalid, this, [this]() {
        QMessageBox::warning(this, tr("路径无效"), tr("该目录下未找到可用壁纸文件，请重新选择。"));
    }, Qt::UniqueConnection);

    emit _zAxisCtrl->switchWallpaperPath(dir);
}

void settingUi::on_switchDurationSpinBox_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
}

