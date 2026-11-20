#pragma once

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

/**
 * @brief Главное окно приложения.
 * Меню, панель, док журнала и вкладки описаны в `mainwindow.ui`.
 * Слоты вида `on_actionXxx_triggered` подключаются автоматически.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    void on_actionClear_triggered();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void on_actionAboutQt_triggered();

    void log(const QString& text);

private:
    void readSettings();
    void writeSettings();

    Ui::MainWindow* ui;
};
