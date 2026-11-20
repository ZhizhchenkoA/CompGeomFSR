#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QCloseEvent>
#include <QMessageBox>
#include <QSettings>
#include <QTime>
#include <QApplication>
#include "geometrypage.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Сигналы страницы -> журнал и статусбар.
    connect(ui->geometryPage, &GeometryPage::logMessage, this, &MainWindow::log);
    connect(ui->geometryPage, &GeometryPage::cursorMoved, this, [this](const QPointF& p) {
        ui->coordinatesLabel->setText(QString("x: %1,  y: %2")
                                          .arg(p.x(), 0, 'f', 1)
                                          .arg(p.y(), 0, 'f', 1));
    });

    readSettings();
    statusBar()->showMessage("Готово. Выберите задачу и кликайте по холсту.", 5000);
    log("Окно построено. Задача 1 (предикат поворота) готова к сдаче.");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::log(const QString& text)
{
    ui->logEdit->appendPlainText(QTime::currentTime().toString("hh:mm:ss") + "  " + text);
    statusBar()->showMessage(text, 3000);
}

void MainWindow::on_actionClear_triggered()
{
    ui->geometryPage->clearScene();
    log("Сцена очищена.");
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "О программе",
                       "<h3>Geometry Workbench</h3>"
                       "<p>Компьютерная геометрия: задачи 1–13 на одной странице.</p>"
                       "<p>Алгоритмы — чистый C++ (src/lib/geometry), "
                       "интерфейс — Qt (src/apps/GeometryWorkbench).</p>");
}

void MainWindow::on_actionAboutQt_triggered()
{
    QApplication::aboutQt();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    writeSettings();
    event->accept();
}

void MainWindow::readSettings()
{
    QSettings settings;
    restoreGeometry(settings.value("window/geometry").toByteArray());
    restoreState(settings.value("window/state").toByteArray());
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.setValue("window/geometry", saveGeometry());
    settings.setValue("window/state", saveState());
}
