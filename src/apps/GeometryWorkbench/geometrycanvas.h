#pragma once

#include <QColor>
#include <QPointF>
#include <QString>
#include <QWidget>
#include <array>
#include <vector>

class QPainter;
class QMouseEvent;
class QPaintEvent;

/**
 * @brief Холст геометрических сцен.
 * Точки хранятся в экранных координатах (ось `Y` вниз, как в Qt).
 * Холст рисует точки, отрезки, кривые и результаты операций.
 */
class GeometryCanvas : public QWidget {
    Q_OBJECT
public:
    /** @brief Создаёт интерактивный холст с координатной сеткой. */
    explicit GeometryCanvas(QWidget* parent = nullptr);

    /** @brief Добавляет перетаскиваемую точку и возвращает её индекс. */
    int addPoint(const QPointF& screenPos);
    /** @brief Обновляет позицию точки. */
    void setPointPos(int index, const QPointF& screenPos);
    /** @brief Настраивает цвет точки. */
    void setPointColor(int index, const QColor& color);
    /** @brief Устанавливает подпись точки. */
    void setPointLabel(int index, const QString& label);
    /** @brief Добавляет отрезок между индексами точек. */
    void addSegment(int a, int b, const QColor& color = QColor());
    void setIntersectionMarker(const QPointF& screenPos, bool visible);
    void setOverlapHighlight(const QPointF& first, const QPointF& second, bool visible);
    void clearIntersectionVisuals();
    void setConvexHull(const std::vector<QPointF>& vertices);
    void clearConvexHull();
    void setDelaunayTriangles(const std::vector<std::array<QPointF, 3>>& triangles,
                              const std::vector<std::pair<QPointF, double>>& circles);
    void clearDelaunayTriangles();
    void setCircumcirclesVisible(bool visible);
    void setPolygonOperation(const std::vector<QPointF>& first, const std::vector<QPointF>& second,
                             const std::vector<std::vector<QPointF>>& result,
                             const QColor& resultColor = QColor(220, 38, 38));
    void clearPolygonOperation();
    void setBezierCurves(const std::vector<std::vector<QPointF>>& curves,
                         const std::vector<std::vector<QPointF>>& levels,
                         const QPointF& marker, bool markerVisible);
    void clearBezierCurves();
    /** @brief Показывает направляющую прямую `AB` и полуплоскости задания 1. */
    void setTurnGuideVisible(bool visible);
    /** @brief Удаляет все отрезки. */
    void clearSegments();
    /** @brief Удаляет точки, отрезки и все визуальные результаты. */
    void clear();
    int pointCount() const;

signals:
    void clicked(const QPointF& screenPos);  
    void pointPressed(int index);
    void pointMoved(int index, const QPointF& screenPos);
    void pointReleased(int index);
    void cursorMoved(const QPointF& screenPos);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    int hitTest(const QPointF& screenPos) const;
    void drawPoint(QPainter& painter, int index) const;
    void drawTurnGuide(QPainter& painter) const;

    std::vector<QPointF> m_points;
    std::vector<QColor> m_pointColors;
    std::vector<QString> m_pointLabels;
    struct Segment { int a; int b; QColor color; };
    std::vector<Segment> m_segments;

    QColor m_defaultPointColor{37, 99, 235};
    QColor m_segmentColor{100, 116, 139};
    int m_dragIndex = -1;
    bool m_turnGuideVisible = false;
    bool m_intersectionMarkerVisible = false;
    QPointF m_intersectionMarker;
    bool m_overlapHighlightVisible = false;
    QPointF m_overlapFirst;
    QPointF m_overlapSecond;
    std::vector<QPointF> m_convexHull;
    std::vector<std::array<QPointF, 3>> m_delaunayTriangles;
    std::vector<std::pair<QPointF, double>> m_circumcircles;
    bool m_circumcirclesVisible = true;
    std::vector<QPointF> m_polygonFirst;
    std::vector<QPointF> m_polygonSecond;
    std::vector<std::vector<QPointF>> m_polygonResult;
    QColor m_polygonResultColor{220, 38, 38};
    std::vector<std::vector<QPointF>> m_bezierCurves;
    std::vector<std::vector<QPointF>> m_bezierLevels;
    QPointF m_bezierMarker;
    bool m_bezierMarkerVisible = false;

    static constexpr double kPointRadius = 6.0;
    static constexpr double kHitRadius = 12.0;
};
