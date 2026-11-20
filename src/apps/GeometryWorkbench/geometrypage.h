#pragma once

#include <QWidget>
#include <vector>

#include "geometry/point.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class GeometryPage;
}
QT_END_NAMESPACE

/**
 * @brief Главная страница задач 1–13.
 * Холст хранит точки в экранных координатах; страница переводит их
 * в геометрию через `geometry_qt` и вызывает чистые алгоритмы библиотек.
 */
class GeometryPage : public QWidget {
    Q_OBJECT
public:
    explicit GeometryPage(QWidget* parent = nullptr);
    ~GeometryPage() override;

    void clearScene();

signals:
    void logMessage(const QString& text);
    void cursorMoved(const QPointF& geometryPos);

private slots:
    void on_onlineCheckBox_toggled(bool checked);
    void on_clearButton_clicked();
    void on_calculateButton_clicked();
    void on_circumcirclesCheckBox_toggled(bool checked);
    void on_bezierSlider_valueChanged(int value);

    // Сигналы холста подключаются автоматически (connectSlotsByName):
    // имя виджета "canvas" + имя сигнала.
    void on_canvas_clicked(const QPointF& screenPos);
    void on_canvas_pointMoved(int index, const QPointF& screenPos);
    void on_canvas_cursorMoved(const QPointF& screenPos);

private:
    void onTaskChanged(int index);
    void selectTask(int task);
    void showLongArithmeticPage(bool visible);

    // Задача 1: предикат поворота.
private:
    void handleTurnClick(const QPointF& screenPos);
    void handleTurnDrag(int index, const QPointF& screenPos);
    void computeTurn();

    // Задача 2: пересечение отрезков AB и CD.
    void handleIntersectionClick(const QPointF& screenPos);
    void handleIntersectionDrag(int index, const QPointF& screenPos);
    void computeIntersection();

    // Задача 5: выпуклая оболочка.
    void handleHullClick(const QPointF& screenPos);
    void handleHullDrag(int index, const QPointF& screenPos);
    void computeHull();

    // Задача 6: триангуляция Делоне.
    void handleDelaunayClick(const QPointF& screenPos);
    void handleDelaunayDrag(int index, const QPointF& screenPos);
    void computeDelaunay();

    // Задачи 7--9: операции над выпуклыми многоугольниками.
    void handlePolygonClick(const QPointF& screenPos);
    void handlePolygonDrag(int index, const QPointF& screenPos);
    void computePolygonOperation();

    // Задача 13: кривые Безье.
    void handleBezierClick(const QPointF& screenPos);
    void handleBezierDrag(int index, const QPointF& screenPos);
    void computeBezier();

    Ui::GeometryPage* ui;
    std::vector<geometry::Point> m_points; // геометрические координаты (Y вверх)
    std::vector<geometry::Point> m_polygonA;
    std::vector<geometry::Point> m_polygonB;
    std::vector<std::pair<int, int>> m_polygonPointBindings;
    int m_currentTask = 1;
};
