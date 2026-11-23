#include "geometrycanvas.h"

#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPalette>
#include <QPolygonF>
#include <QPainterPath>
#include <cmath>

namespace {
// QMouseEvent::position() есть только в Qt 6; в Qt 5 — localPos().
QPointF mousePos(const QMouseEvent* event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}
} // namespace

GeometryCanvas::GeometryCanvas(QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setMinimumSize(420, 320);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
}

int GeometryCanvas::addPoint(const QPointF& screenPos)
{
    m_points.push_back(screenPos);
    m_pointColors.push_back(m_defaultPointColor);
    m_pointLabels.emplace_back();
    update();
    return static_cast<int>(m_points.size()) - 1;
}

void GeometryCanvas::setPointPos(int index, const QPointF& screenPos)
{
    if (index < 0 || index >= static_cast<int>(m_points.size())) {
        return;
    }
    m_points[index] = screenPos;
    update();
}

void GeometryCanvas::setPointColor(int index, const QColor& color)
{
    if (index < 0 || index >= static_cast<int>(m_pointColors.size())) {
        return;
    }
    m_pointColors[index] = color;
    update();
}

void GeometryCanvas::setPointLabel(int index, const QString& label)
{
    if (index < 0 || index >= static_cast<int>(m_pointLabels.size())) {
        return;
    }
    m_pointLabels[index] = label;
    update();
}

void GeometryCanvas::addSegment(int a, int b, const QColor& color)
{
    m_segments.push_back({a, b, color});
    update();
}

void GeometryCanvas::setIntersectionMarker(const QPointF& screenPos, bool visible)
{
    m_intersectionMarker = screenPos;
    m_intersectionMarkerVisible = visible;
    update();
}

void GeometryCanvas::setOverlapHighlight(const QPointF& first, const QPointF& second, bool visible)
{
    m_overlapFirst = first;
    m_overlapSecond = second;
    m_overlapHighlightVisible = visible;
    update();
}

void GeometryCanvas::clearIntersectionVisuals()
{
    m_intersectionMarkerVisible = false;
    m_overlapHighlightVisible = false;
    update();
}

void GeometryCanvas::setConvexHull(const std::vector<QPointF>& vertices)
{
    m_convexHull = vertices;
    update();
}

void GeometryCanvas::clearConvexHull()
{
    m_convexHull.clear();
    update();
}

void GeometryCanvas::setDelaunayTriangles(const std::vector<std::array<QPointF, 3>>& triangles,
                                          const std::vector<std::pair<QPointF, double>>& circles)
{
    m_delaunayTriangles = triangles;
    m_circumcircles = circles;
    update();
}

void GeometryCanvas::clearDelaunayTriangles()
{
    m_delaunayTriangles.clear();
    m_circumcircles.clear();
    update();
}

void GeometryCanvas::setCircumcirclesVisible(bool visible)
{
    m_circumcirclesVisible = visible;
    update();
}

void GeometryCanvas::setPolygonOperation(const std::vector<QPointF>& first,
                                         const std::vector<QPointF>& second,
                                         const std::vector<std::vector<QPointF>>& result,
                                         const QColor& resultColor)
{
    m_polygonFirst = first;
    m_polygonSecond = second;
    m_polygonResult = result;
    m_polygonResultColor = resultColor;
    update();
}

void GeometryCanvas::clearPolygonOperation()
{
    m_polygonFirst.clear();
    m_polygonSecond.clear();
    m_polygonResult.clear();
    update();
}
void GeometryCanvas::setBezierCurves(const std::vector<std::vector<QPointF>>& curves,
                                     const std::vector<std::vector<QPointF>>& levels,
                                     const QPointF& marker, bool markerVisible) {
    m_bezierCurves=curves; m_bezierLevels=levels; m_bezierMarker=marker; m_bezierMarkerVisible=markerVisible; update();
}
void GeometryCanvas::clearBezierCurves(){m_bezierCurves.clear();m_bezierLevels.clear();m_bezierMarkerVisible=false;update();}

void GeometryCanvas::setTurnGuideVisible(bool visible)
{
    m_turnGuideVisible = visible;
    update();
}

void GeometryCanvas::clearSegments()
{
    m_segments.clear();
    update();
}

void GeometryCanvas::clear()
{
    m_points.clear();
    m_pointColors.clear();
    m_pointLabels.clear();
    m_segments.clear();
    m_dragIndex = -1;
    m_turnGuideVisible = false;
    m_intersectionMarkerVisible = false;
    m_overlapHighlightVisible = false;
    m_convexHull.clear();
    m_delaunayTriangles.clear();
    m_circumcircles.clear();
    m_polygonFirst.clear();
    m_polygonSecond.clear();
    m_polygonResult.clear();
    m_bezierCurves.clear(); m_bezierLevels.clear(); m_bezierMarkerVisible=false;
    update();
}

int GeometryCanvas::pointCount() const
{
    return static_cast<int>(m_points.size());
}

int GeometryCanvas::hitTest(const QPointF& screenPos) const
{
    int best = -1;
    double bestDistance = kHitRadius;
    for (int i = 0; i < static_cast<int>(m_points.size()); ++i) {
        const double dx = m_points[i].x() - screenPos.x();
        const double dy = m_points[i].y() - screenPos.y();
        const double distance = std::sqrt(dx * dx + dy * dy);
        if (distance <= bestDistance) {
            bestDistance = distance;
            best = i;
        }
    }
    return best;
}

void GeometryCanvas::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(250, 252, 255));

    painter.setPen(QPen(QColor(224, 231, 240), 1.0));
    constexpr int gridStep = 40;
    for (int x = 0; x <= width(); x += gridStep) {
        painter.drawLine(x, 0, x, height());
    }
    for (int y = 0; y <= height(); y += gridStep) {
        painter.drawLine(0, y, width(), y);
    }

    if (m_turnGuideVisible && m_points.size() >= 2) {
        drawTurnGuide(painter);
    }

    if (!m_convexHull.empty()) {
        const QColor hullColor(22, 163, 74);
        if (m_convexHull.size() >= 3) {
            painter.setPen(QPen(hullColor, 2.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter.setBrush(QColor(22, 163, 74, 38));
            QPolygonF polygon;
            for (const QPointF& vertex : m_convexHull) {
                polygon << vertex;
            }
            painter.drawPolygon(polygon);
        } else if (m_convexHull.size() == 2) {
            painter.setPen(QPen(hullColor, 4.0, Qt::SolidLine, Qt::RoundCap));
            painter.drawLine(m_convexHull[0], m_convexHull[1]);
        } else {
            painter.setPen(QPen(hullColor, 2.0));
            painter.setBrush(QColor(22, 163, 74, 50));
            painter.drawEllipse(m_convexHull.front(), 9.0, 9.0);
        }
    }

    if (m_circumcirclesVisible) {
        painter.setPen(QPen(QColor(100, 116, 139, 90), 1.0, Qt::DashLine));
        painter.setBrush(Qt::NoBrush);
        for (const auto& [center, radius] : m_circumcircles) {
            painter.drawEllipse(center, radius, radius);
        }
    }

    for (const auto& triangle : m_delaunayTriangles) {
        QPolygonF polygon;
        polygon << triangle[0] << triangle[1] << triangle[2];
        painter.setPen(QPen(QColor(13, 148, 136), 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.setBrush(QColor(20, 184, 166, 28));
        painter.drawPolygon(polygon);
    }

    const auto drawInputPolygon = [&painter](const std::vector<QPointF>& vertices, const QColor& color) {
        if (vertices.size() < 3) return;
        QPolygonF polygon;
        for (const QPointF& vertex : vertices) polygon << vertex;
        painter.setPen(QPen(color, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.setBrush(QColor(color.red(), color.green(), color.blue(), 45));
        painter.drawPolygon(polygon);
    };
    drawInputPolygon(m_polygonFirst, QColor(37, 99, 235));
    drawInputPolygon(m_polygonSecond, QColor(234, 88, 12));
    if (!m_polygonResult.empty()) {
        QPainterPath path;
        path.setFillRule(Qt::OddEvenFill);
        for (const auto& contour : m_polygonResult) {
            if (contour.size() < 3) continue;
            QPolygonF polygon;
            for (const QPointF& vertex : contour) polygon << vertex;
            path.addPolygon(polygon);
        }
        

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(m_polygonResultColor.red(), m_polygonResultColor.green(),
                                m_polygonResultColor.blue(), 145));
        painter.drawPath(path);

        painter.setPen(QPen(m_polygonResultColor.darker(185), 6.0,
                            Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.setBrush(Qt::NoBrush);
        for (const auto& contour : m_polygonResult) {
            if (contour.size() < 3) continue;
            QPolygonF polygon;
            for (const QPointF& vertex : contour) polygon << vertex;
            painter.drawPolygon(polygon);
        }
    }

    const QColor levelColors[] = {QColor(202, 138, 4, 150), QColor(34, 197, 94, 150), QColor(14, 116, 144, 150)};
    for (size_t i=1;i<m_bezierLevels.size();++i) {
        if (m_bezierLevels[i].size()<2) continue;
        QPolygonF line; for(const QPointF& p:m_bezierLevels[i]) line<<p;
        painter.setPen(QPen(levelColors[(i-1)%3],1.5)); painter.setBrush(Qt::NoBrush); painter.drawPolyline(line);
    }
    const QColor curveColors[] = {QColor(37,99,235), QColor(22,163,74), QColor(126,34,206)};
    for(size_t i=0;i<m_bezierCurves.size();++i){ if(m_bezierCurves[i].size()<2)continue; QPolygonF line;for(const QPointF&p:m_bezierCurves[i])line<<p;
        painter.setPen(QPen(curveColors[i%3],3.5,Qt::SolidLine,Qt::RoundCap,Qt::RoundJoin));painter.drawPolyline(line); }
    if (!m_bezierCurves.empty()) {
        const QRectF legend(12.0, height() - 142.0, 242.0, 130.0);
        painter.setPen(QPen(QColor(203, 213, 225), 1.0));
        painter.setBrush(QColor(255, 255, 255, 238));
        painter.drawRoundedRect(legend, 8.0, 8.0);
        const struct { QColor color; const char* text; } entries[] = {
            {QColor(37, 99, 235), "квадратичная, составная"},
            {QColor(22, 163, 74), "кубическая, составная"},
            {QColor(126, 34, 206), "одна кривая степени n"},
            {QColor(220, 38, 38), "точка де Кастельжо"},
            {QColor(148, 163, 184), "контрольная ломаная"},
        };
        painter.setFont(QFont(painter.font().family(), 10));
        for (int i = 0; i < 5; ++i) {
            const qreal y = legend.top() + 18.0 + i * 22.0;
            painter.setPen(QPen(entries[i].color, 4.0, Qt::SolidLine, Qt::RoundCap));
            painter.drawLine(QPointF(legend.left() + 12.0, y), QPointF(legend.left() + 31.0, y));
            painter.setPen(QColor(51, 65, 85));
            painter.drawText(QPointF(legend.left() + 40.0, y + 4.0), QString::fromUtf8(entries[i].text));
        }
    }

    // Отрезки.
    painter.setPen(QPen(m_segmentColor, 2.0));
    for (const auto& segment : m_segments) {
        const int a = segment.a;
        const int b = segment.b;
        if (a < 0 || b < 0 || a >= static_cast<int>(m_points.size()) ||
            b >= static_cast<int>(m_points.size())) {
            continue;
        }
        painter.setPen(QPen(segment.color.isValid() ? segment.color : m_segmentColor, 3.0));
        painter.drawLine(m_points[a], m_points[b]);
    }

    if (m_overlapHighlightVisible) {
        painter.setPen(QPen(QColor(220, 38, 38), 7.0, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(m_overlapFirst, m_overlapSecond);
    }

    // Точки поверх отрезков.
    for (int i = 0; i < static_cast<int>(m_points.size()); ++i) {
        drawPoint(painter, i);
    }

    if (m_intersectionMarkerVisible) {
        const QColor red(220, 38, 38);
        painter.setPen(QPen(red.darker(130), 2.0));
        painter.setBrush(red);
        painter.drawEllipse(m_intersectionMarker, 8.0, 8.0);
    }
    if(m_bezierMarkerVisible){painter.setPen(QPen(QColor(185,28,28),2));painter.setBrush(QColor(220,38,38));painter.drawEllipse(m_bezierMarker,8,8);}
}

void GeometryCanvas::drawTurnGuide(QPainter& painter) const
{
    const QPointF a = m_points[0];
    const QPointF b = m_points[1];
    const QPointF d = b - a;
    const double length = std::hypot(d.x(), d.y());
    if (length < 1e-6) {
        return; // у A = B нет направленной прямой и полуплоскостей
    }

    const QPointF direction = d / length;
    const QPointF leftNormal(direction.y(), -direction.x());
    const double reach = std::hypot(static_cast<double>(width()), static_cast<double>(height())) * 2.0;
    const QPointF lineStart = a - direction * reach;
    const QPointF lineEnd = a + direction * reach;

    QPolygonF leftArea;
    leftArea << lineStart << lineEnd << lineEnd + leftNormal * reach << lineStart + leftNormal * reach;
    QPolygonF rightArea;
    rightArea << lineEnd << lineStart << lineStart - leftNormal * reach << lineEnd - leftNormal * reach;
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(22, 163, 74, 24));
    painter.drawPolygon(leftArea);
    painter.setBrush(QColor(220, 38, 38, 20));
    painter.drawPolygon(rightArea);

    painter.setPen(QPen(QColor(148, 163, 184), 1.25, Qt::DashLine));
    painter.setBrush(Qt::NoBrush);
    painter.drawLine(lineStart, lineEnd);

    // Сам отрезок AB и стрелка направления расположены поверх прямой.
    painter.setPen(QPen(QColor(30, 41, 59), 2.5));
    painter.drawLine(a, b);
    const QPointF arrowBase = b - direction * 12.0;
    const QPointF arrowSide(-direction.y(), direction.x());
    QPolygonF arrow;
    arrow << b << arrowBase + arrowSide * 5.0 << arrowBase - arrowSide * 5.0;
    painter.setBrush(QColor(30, 41, 59));
    painter.drawPolygon(arrow);

    // Легенда закреплена в левом нижнем углу холста.
    const QRectF legend(12.0, height() - 106.0, 190.0, 94.0);
    painter.setPen(QPen(QColor(203, 213, 225), 1.0));
    painter.setBrush(QColor(255, 255, 255, 235));
    painter.drawRoundedRect(legend, 8.0, 8.0);
    const struct { QColor color; const char* text; } entries[] = {
        {QColor(22, 163, 74), "слева: 1"},
        {QColor(220, 38, 38), "справа: -1"},
        {QColor(234, 179, 8), "на прямой: 0"},
    };
    painter.setFont(QFont(painter.font().family(), 11));
    for (int i = 0; i < 3; ++i) {
        const qreal y = legend.top() + 20.0 + i * 26.0;
        painter.setPen(QPen(entries[i].color.darker(120), 1.0));
        painter.setBrush(entries[i].color);
        painter.drawRoundedRect(QRectF(legend.left() + 12.0, y - 10.0, 16.0, 16.0), 3.0, 3.0);
        painter.setPen(QColor(51, 65, 85));
        painter.drawText(QPointF(legend.left() + 38.0, y + 3.0), QString::fromUtf8(entries[i].text));
    }
}

void GeometryCanvas::drawPoint(QPainter& painter, int index) const
{
    const QPointF& p = m_points[index];
    QColor fill = m_defaultPointColor;
    if (index < static_cast<int>(m_pointColors.size()) && m_pointColors[index].isValid()) {
        fill = m_pointColors[index];
    }

    painter.setPen(QPen(fill.darker(140), 1.5));
    painter.setBrush(fill);
    painter.drawEllipse(p, kPointRadius, kPointRadius);

    if (index < static_cast<int>(m_pointLabels.size()) && !m_pointLabels[index].isEmpty()) {
        painter.setPen(Qt::black);
        painter.drawText(p + QPointF(kPointRadius + 4.0, -(kPointRadius + 4.0)),
                         m_pointLabels[index]);
    }
}

void GeometryCanvas::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        const QPointF pos = mousePos(event);
        const int index = hitTest(pos);
        if (index >= 0) {
            m_dragIndex = index;
            emit pointPressed(index);
        } else {
            emit clicked(pos);
        }
    }
    QWidget::mousePressEvent(event);
}

void GeometryCanvas::mouseMoveEvent(QMouseEvent* event)
{
    const QPointF pos = mousePos(event);
    emit cursorMoved(pos);

    if (m_dragIndex >= 0 && (event->buttons() & Qt::LeftButton)) {
        m_points[m_dragIndex] = pos;
        emit pointMoved(m_dragIndex, pos);
        update();
    } else {
        setCursor(hitTest(pos) >= 0 ? Qt::PointingHandCursor : Qt::ArrowCursor);
    }
    QWidget::mouseMoveEvent(event);
}

void GeometryCanvas::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && m_dragIndex >= 0) {
        const int index = m_dragIndex;
        m_dragIndex = -1;
        emit pointReleased(index);
    }
    QWidget::mouseReleaseEvent(event);
}
