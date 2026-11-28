#include "geometrypage.h"
#include "ui_geometrypage.h"

#include <QCheckBox>
#include <QColor>
#include <QComboBox>
#include <QLabel>
#include <QSizePolicy>
#include <QStackedWidget>
#include <QString>
#include <array>
#include <cmath>
#include <utility>

#include "geometry/convex_hull.h"
#include "bezier/bezier.h"
#include "geometry/convex_polygon_boolean.h"
#include "geometry/delaunay.h"
#include "geometry/predicates.h"
#include "geometry/segment_intersection.h"
#include "geometry_qt/conversion.h"
#include "geometrycanvas.h"

namespace
{
    // Радиус попадания совпадает с радиусом нарисованного маркера (6 px),
    // поэтому визуально касающаяся прямой точка считается лежащей на ней.
    constexpr double kVisualPointTolerance = 6.0;

} // namespace

GeometryPage::GeometryPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::GeometryPage)
{
    ui->setupUi(this);

    ui->mainSplitter->setStretchFactor(0, 0);
    ui->mainSplitter->setStretchFactor(1, 1);
    ui->mainSplitter->setSizes({430, 800});
    ui->controlPanel->setMinimumWidth(390);
    ui->contentStack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // QComboBox::currentIndexChanged перегружен (int / QString),
    // поэтому подключаем вручную, чтобы не было двойного срабатывания.
    connect(ui->taskComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &GeometryPage::onTaskChanged);

    selectTask(1);
}

GeometryPage::~GeometryPage()
{
    delete ui;
}

void GeometryPage::clearScene()
{
    m_points.clear();
    m_polygonA.clear();
    m_polygonB.clear();
    m_polygonPointBindings.clear();
    ui->canvas->clear();
    ui->resultLabel->setText("Результат: —");
}

void GeometryPage::selectTask(int task)
{
    m_currentTask = task;
    clearScene();
    showLongArithmeticPage(task == 3);
    ui->onlineCheckBox->setVisible(task != 3);

    switch (task)
    {
    case 1:
        ui->bezierSlider->hide();
        ui->pageTitleLabel->setText("Предикат поворота");
        ui->taskInfoLabel->setText(
            "<p>●&nbsp; Два клика — отрезок <b>AB</b>, третий — точка <b>P</b></p>"
            "<p>●&nbsp; Ответ: <b>1</b> (слева), <b>−1</b> (справа), <b>0</b> (на прямой)</p>"
            "<p>●&nbsp; Перетаскивайте точки; включите онлайн‑пересчёт</p>"
            "<p>●&nbsp; Точка <b>P</b> подсвечивается цветом ответа</p>");
        ui->onlineCheckBox->setEnabled(true);
        ui->taskInfoLabel->show();
        ui->hintLabel->show();
        ui->calculateButton->hide();
        ui->circumcirclesCheckBox->hide();
        ui->polygonComboBox->hide();
        break;
    case 2:
        ui->bezierSlider->hide();
        ui->pageTitleLabel->setText("Пересечение двух отрезков");
        ui->taskInfoLabel->clear();
        ui->taskInfoLabel->hide();
        ui->hintLabel->hide();
        ui->onlineCheckBox->setEnabled(true);
        ui->calculateButton->show();
        ui->circumcirclesCheckBox->hide();
        ui->polygonComboBox->hide();
        break;
    case 3:
        ui->bezierSlider->hide();
        ui->pageTitleLabel->setText("Предикат с длинной арифметикой");
        ui->taskInfoLabel->clear();
        ui->taskInfoLabel->hide();
        ui->hintLabel->hide();
        ui->onlineCheckBox->hide();
        ui->calculateButton->hide();
        ui->circumcirclesCheckBox->hide();
        ui->polygonComboBox->hide();
        break;
    case 5:
        ui->bezierSlider->hide();
        ui->pageTitleLabel->setText("Выпуклая оболочка");
        ui->taskInfoLabel->clear();
        ui->taskInfoLabel->hide();
        ui->hintLabel->hide();
        ui->onlineCheckBox->setEnabled(true);
        ui->calculateButton->show();
        ui->circumcirclesCheckBox->hide();
        ui->polygonComboBox->hide();
        break;
    case 6:
        ui->bezierSlider->hide();
        ui->pageTitleLabel->setText("Триангуляция Делоне");
        ui->taskInfoLabel->clear();
        ui->taskInfoLabel->hide();
        ui->hintLabel->hide();
        ui->onlineCheckBox->setEnabled(true);
        ui->calculateButton->show();
        ui->circumcirclesCheckBox->show();
        ui->polygonComboBox->hide();
        ui->canvas->setCircumcirclesVisible(ui->circumcirclesCheckBox->isChecked());
        break;
    case 7:
    case 8:
    case 9:
        ui->pageTitleLabel->setText(task == 7 ? "Пересечение выпуклых" : task == 8 ? "Объединение выпуклых"
                                                                                   : "Разность выпуклых");
        ui->taskInfoLabel->clear();
        ui->taskInfoLabel->hide();
        ui->hintLabel->hide();
        ui->onlineCheckBox->setEnabled(true);
        ui->calculateButton->show();
        ui->circumcirclesCheckBox->hide();
        ui->polygonComboBox->show();
        ui->bezierSlider->hide();
        break;
    case 13:
        ui->pageTitleLabel->setText("Кривые Безье");
        ui->taskInfoLabel->clear();
        ui->taskInfoLabel->hide();
        ui->hintLabel->hide();
        ui->onlineCheckBox->setEnabled(true);
        ui->calculateButton->show();
        ui->circumcirclesCheckBox->hide();
        ui->polygonComboBox->hide();
        ui->bezierSlider->show();
        break;
    case 4:
    case 10:
    case 11:
    case 12:
        ui->taskInfoLabel->setText("Задача в разработке. Будет добавлена позже.");
        ui->taskInfoLabel->show();
        ui->hintLabel->hide();
        ui->onlineCheckBox->setEnabled(false);
        ui->calculateButton->hide();
        ui->circumcirclesCheckBox->hide();
        ui->polygonComboBox->hide();
        ui->bezierSlider->hide();
        break;
    default:
        ui->taskInfoLabel->setText("Неизвестная задача.");
        ui->taskInfoLabel->show();
        ui->hintLabel->hide();
        ui->onlineCheckBox->setEnabled(false);
        ui->calculateButton->hide();
        ui->circumcirclesCheckBox->hide();
        ui->polygonComboBox->hide();
        ui->bezierSlider->hide();
    }
    ui->resultLabel->setText("Результат: —");
}

void GeometryPage::showLongArithmeticPage(bool visible)
{
    ui->contentStack->setCurrentIndex(visible ? 1 : 0);
}

void GeometryPage::onTaskChanged(int index)
{
    selectTask(index + 1);
}

void GeometryPage::on_onlineCheckBox_toggled(bool checked)
{
    if (checked)
    {
        if (m_currentTask == 1 && m_points.size() == 3)
        {
            computeTurn();
        }
        else if (m_currentTask == 2 && m_points.size() == 4)
        {
            computeIntersection();
        }
        else if (m_currentTask == 5)
        {
            computeHull();
        }
        else if (m_currentTask == 6)
        {
            computeDelaunay();
        }
        else if (m_currentTask >= 7 && m_currentTask <= 9)
        {
            computePolygonOperation();
        }
        else if (m_currentTask == 13)
        {
            computeBezier();
        }
    }
}

void GeometryPage::on_bezierSlider_valueChanged(int)
{
    if (m_currentTask == 13 && m_points.size() >= 2)
        computeBezier();
}

void GeometryPage::on_circumcirclesCheckBox_toggled(bool checked)
{
    ui->canvas->setCircumcirclesVisible(checked);
}

void GeometryPage::on_clearButton_clicked()
{
    clearScene();
    emit logMessage("Сцена очищена.");
}

void GeometryPage::on_calculateButton_clicked()
{
    if (m_currentTask == 2 && m_points.size() == 4)
    {
        computeIntersection();
    }
    else if (m_currentTask == 5)
    {
        computeHull();
    }
    else if (m_currentTask == 6)
    {
        computeDelaunay();
    }
    else if (m_currentTask >= 7 && m_currentTask <= 9)
    {
        computePolygonOperation();
    }
    else if (m_currentTask == 13)
    {
        computeBezier();
    }
}

// --- Холст: клики и перетаскивание ---

void GeometryPage::on_canvas_clicked(const QPointF &screenPos)
{
    if (m_currentTask == 1)
    {
        handleTurnClick(screenPos);
    }
    else if (m_currentTask == 2)
    {
        handleIntersectionClick(screenPos);
    }
    else if (m_currentTask == 5)
    {
        handleHullClick(screenPos);
    }
    else if (m_currentTask == 6)
    {
        handleDelaunayClick(screenPos);
    }
    else if (m_currentTask >= 7 && m_currentTask <= 9)
    {
        handlePolygonClick(screenPos);
    }
    else if (m_currentTask == 13)
    {
        handleBezierClick(screenPos);
    }
    // Для остальных задач обработчики добавляются по мере реализации.
}

void GeometryPage::on_canvas_pointMoved(int index, const QPointF &screenPos)
{
    if (m_currentTask == 1)
    {
        handleTurnDrag(index, screenPos);
    }
    else if (m_currentTask == 2)
    {
        handleIntersectionDrag(index, screenPos);
    }
    else if (m_currentTask == 5)
    {
        handleHullDrag(index, screenPos);
    }
    else if (m_currentTask == 6)
    {
        handleDelaunayDrag(index, screenPos);
    }
    else if (m_currentTask >= 7 && m_currentTask <= 9)
    {
        handlePolygonDrag(index, screenPos);
    }
    else if (m_currentTask == 13)
    {
        handleBezierDrag(index, screenPos);
    }
}

void GeometryPage::on_canvas_cursorMoved(const QPointF &screenPos)
{
    const geometry::Point g = geometry_qt::toGeometry(screenPos);
    emit cursorMoved(QPointF(g.x, g.y));
}

// --- Задача 1: предикат поворота ---

void GeometryPage::handleTurnClick(const QPointF &screenPos)
{
    if (m_points.size() >= 3)
    {
        return;
    }

    m_points.push_back(geometry_qt::toGeometry(screenPos));
    const int index = ui->canvas->addPoint(screenPos);

    static const char *kNames[] = {"A", "B", "P"};
    ui->canvas->setPointLabel(index, QString::fromLatin1(kNames[index]));

    if (m_points.size() == 2)
    {
        ui->canvas->addSegment(0, 1); // отрезок AB
        ui->canvas->setTurnGuideVisible(true);
    }
    if (m_points.size() == 3)
    {
        computeTurn();
    }
    else
    {
        ui->resultLabel->setText(QString("Результат: точек %1 из 3").arg(static_cast<qulonglong>(m_points.size())));
    }
}

void GeometryPage::computeTurn()
{
    const geometry::Point &a = m_points[0];
    const geometry::Point &b = m_points[1];
    const geometry::Point &p = m_points[2];

    const double value = geometry::cross(a, b, p);
    const double abX = b.x - a.x;
    const double abY = b.y - a.y;
    const bool degenerateSegment = abX * abX + abY * abY <= 1e-18;
    // cross имеет размерность «пиксель²». Преобразуем радиус маркера в
    // соответствующий допуск: расстояние до AB * длина AB.
    const int sign = geometry::turn(a, b, p,
                                    kVisualPointTolerance * std::hypot(abX, abY));

    QString text;
    QColor color;
    if (degenerateSegment)
    {
        text = "0 — A и B совпадают: направление не определено";
        color = QColor(148, 163, 184); // серый
    }
    else if (sign > 0)
    {
        text = "1 — P слева от AB";
        color = QColor(22, 163, 74); // зелёный
    }
    else if (sign < 0)
    {
        text = "−1 — P справа от AB";
        color = QColor(220, 38, 38); // красный
    }
    else
    {
        // Коллинеарны: уточняем, на отрезке точка или на прямой вне его.
        if (geometry::onSegment(a, b, p, kVisualPointTolerance))
        {
            text = "0 — P на отрезке AB";
            color = QColor(234, 179, 8); // жёлтый
        }
        else
        {
            text = "0 — P на прямой (вне отрезка)";
            color = QColor(148, 163, 184); // серый
        }
    }

    ui->canvas->setPointColor(2, color);
    ui->resultLabel->setText("Результат: " + text);
    emit logMessage(QString("Предикат поворота: %1 (векторное произведение = %2)")
                        .arg(text)
                        .arg(value, 0, 'f', 2));
}

void GeometryPage::handleTurnDrag(int index, const QPointF &screenPos)
{
    if (index < 0 || index >= static_cast<int>(m_points.size()))
    {
        return;
    }

    m_points[index] = geometry_qt::toGeometry(screenPos);

    // Направляющая строится по экранным координатам и обновляется даже
    // при выключенном пересчёте, чтобы сцена не расходилась с точками.
    if (m_points.size() >= 2)
    {
        ui->canvas->setTurnGuideVisible(true);
    }

    if (m_points.size() == 3 && ui->onlineCheckBox->isChecked())
    {
        computeTurn();
    }
}

// --- Задача 2: пересечение двух отрезков ---

void GeometryPage::handleIntersectionClick(const QPointF &screenPos)
{
    if (m_points.size() >= 4)
    {
        return;
    }

    m_points.push_back(geometry_qt::toGeometry(screenPos));
    const int index = ui->canvas->addPoint(screenPos);
    static const char *kNames[] = {"A", "B", "C", "D"};
    ui->canvas->setPointLabel(index, QString::fromLatin1(kNames[index]));

    if (m_points.size() == 2)
    {
        ui->canvas->addSegment(0, 1, QColor(37, 99, 235));
    }
    else if (m_points.size() == 4)
    {
        ui->canvas->addSegment(2, 3, QColor(234, 88, 12));
        if (ui->onlineCheckBox->isChecked())
        {
            computeIntersection();
        }
        else
        {
            ui->resultLabel->setText("Результат: нажмите «Рассчитать»");
        }
    }
    else
    {
        ui->resultLabel->setText(QString("Точек: %1 из 4").arg(static_cast<qulonglong>(m_points.size())));
    }
}

void GeometryPage::handleIntersectionDrag(int index, const QPointF &screenPos)
{
    if (index < 0 || index >= static_cast<int>(m_points.size()))
    {
        return;
    }
    m_points[index] = geometry_qt::toGeometry(screenPos);
    ui->canvas->clearIntersectionVisuals();
    if (m_points.size() == 4 && ui->onlineCheckBox->isChecked())
    {
        computeIntersection();
    }
}

void GeometryPage::computeIntersection()
{
    if (m_points.size() != 4)
    {
        return;
    }

    const geometry::SegmentIntersection result = geometry::intersectSegments(
        m_points[0], m_points[1], m_points[2], m_points[3], kVisualPointTolerance);
    ui->canvas->clearIntersectionVisuals();

    switch (result.kind)
    {
    case geometry::SegmentIntersectionKind::None:
        ui->resultLabel->setText("Результат: пересечения нет");
        emit logMessage("Пересечение отрезков: нет.");
        break;
    case geometry::SegmentIntersectionKind::Point:
    {
        const QPointF point = geometry_qt::toScreen(result.first);
        ui->canvas->setIntersectionMarker(point, true);
        ui->resultLabel->setText(QString("Результат: (%1; %2)")
                                     .arg(result.first.x, 0, 'f', 1)
                                     .arg(result.first.y, 0, 'f', 1));
        emit logMessage("Пересечение отрезков: точка.");
        break;
    }
    case geometry::SegmentIntersectionKind::Overlap:
        ui->canvas->setOverlapHighlight(geometry_qt::toScreen(result.first),
                                        geometry_qt::toScreen(result.second), true);
        ui->resultLabel->setText("Результат: общий участок");
        emit logMessage("Пересечение отрезков: общий участок.");
        break;
    }
}

// --- Задача 5: выпуклая оболочка ---

void GeometryPage::handleHullClick(const QPointF &screenPos)
{
    m_points.push_back(geometry_qt::toGeometry(screenPos));
    ui->canvas->addPoint(screenPos);
    ui->canvas->clearConvexHull();

    if (ui->onlineCheckBox->isChecked())
    {
        computeHull();
    }
    else
    {
        ui->resultLabel->setText(QString("Точек: %1").arg(static_cast<qulonglong>(m_points.size())));
    }
}

void GeometryPage::handleHullDrag(int index, const QPointF &screenPos)
{
    if (index < 0 || index >= static_cast<int>(m_points.size()))
    {
        return;
    }
    m_points[index] = geometry_qt::toGeometry(screenPos);
    ui->canvas->clearConvexHull();
    if (ui->onlineCheckBox->isChecked())
    {
        computeHull();
    }
}

void GeometryPage::computeHull()
{
    const std::vector<geometry::Point> hull = geometry::convexHull(m_points);
    std::vector<QPointF> screenHull;
    screenHull.reserve(hull.size());
    for (const geometry::Point &point : hull)
    {
        screenHull.push_back(geometry_qt::toScreen(point));
    }
    ui->canvas->setConvexHull(screenHull);
    ui->resultLabel->setText(QString("Вершин оболочки: %1").arg(static_cast<qulonglong>(hull.size())));
    emit logMessage(QString("Выпуклая оболочка: %1 вершин.").arg(static_cast<qulonglong>(hull.size())));
}

// --- Задача 6: триангуляция Делоне ---

void GeometryPage::handleDelaunayClick(const QPointF &screenPos)
{
    m_points.push_back(geometry_qt::toGeometry(screenPos));
    ui->canvas->addPoint(screenPos);
    ui->canvas->clearDelaunayTriangles();
    if (ui->onlineCheckBox->isChecked())
    {
        computeDelaunay();
    }
    else
    {
        ui->resultLabel->setText(QString("Точек: %1").arg(static_cast<qulonglong>(m_points.size())));
    }
}

void GeometryPage::handleDelaunayDrag(int index, const QPointF &screenPos)
{
    if (index < 0 || index >= static_cast<int>(m_points.size()))
    {
        return;
    }
    m_points[index] = geometry_qt::toGeometry(screenPos);
    ui->canvas->clearDelaunayTriangles();
    if (ui->onlineCheckBox->isChecked())
    {
        computeDelaunay();
    }
}

void GeometryPage::computeDelaunay()
{
    const std::vector<geometry::DelaunayTriangle> triangulation =
        geometry::delaunayTriangulation(m_points);
    std::vector<std::array<QPointF, 3>> triangles;
    std::vector<std::pair<QPointF, double>> circles;
    triangles.reserve(triangulation.size());
    circles.reserve(triangulation.size());
    for (const geometry::DelaunayTriangle &triangle : triangulation)
    {
        triangles.push_back({geometry_qt::toScreen(triangle.vertices[0]),
                             geometry_qt::toScreen(triangle.vertices[1]),
                             geometry_qt::toScreen(triangle.vertices[2])});
        circles.emplace_back(geometry_qt::toScreen(triangle.circumcircle.center),
                             triangle.circumcircle.radius);
    }
    ui->canvas->setDelaunayTriangles(triangles, circles);
    ui->resultLabel->setText(QString("Треугольников: %1").arg(static_cast<qulonglong>(triangles.size())));
    emit logMessage(QString("Триангуляция Делоне: %1 треугольников.").arg(static_cast<qulonglong>(triangles.size())));
}

// --- Задачи 7--9: операции над выпуклыми многоугольниками ---

void GeometryPage::handlePolygonClick(const QPointF &screenPos)
{
    const geometry::Point point = geometry_qt::toGeometry(screenPos);
    const int polygon = ui->polygonComboBox->currentIndex();
    std::vector<geometry::Point> &points = polygon == 0 ? m_polygonA : m_polygonB;
    points.push_back(point);
    const int canvasIndex = ui->canvas->addPoint(screenPos);
    ui->canvas->setPointColor(canvasIndex, polygon == 0 ? QColor(37, 99, 235) : QColor(234, 88, 12));
    m_polygonPointBindings.emplace_back(polygon, static_cast<int>(points.size()) - 1);
    ui->canvas->clearPolygonOperation();
    if (ui->onlineCheckBox->isChecked())
    {
        computePolygonOperation();
    }
    else
    {
        ui->resultLabel->setText(QString("A: %1, B: %2")
                                     .arg(static_cast<qulonglong>(m_polygonA.size()))
                                     .arg(static_cast<qulonglong>(m_polygonB.size())));
    }
}

void GeometryPage::handlePolygonDrag(int index, const QPointF &screenPos)
{
    if (index < 0 || index >= static_cast<int>(m_polygonPointBindings.size()))
    {
        return;
    }
    const auto [polygon, pointIndex] = m_polygonPointBindings[index];
    std::vector<geometry::Point> &points = polygon == 0 ? m_polygonA : m_polygonB;
    if (pointIndex < 0 || pointIndex >= static_cast<int>(points.size()))
    {
        return;
    }
    points[pointIndex] = geometry_qt::toGeometry(screenPos);
    ui->canvas->clearPolygonOperation();
    if (ui->onlineCheckBox->isChecked())
    {
        computePolygonOperation();
    }
}

void GeometryPage::computePolygonOperation()
{
    const std::vector<geometry::Point> first = geometry::convexHull(m_polygonA);
    const std::vector<geometry::Point> second = geometry::convexHull(m_polygonB);
    geometry::ConvexPolygonOperation operation = geometry::ConvexPolygonOperation::Intersection;
    if (m_currentTask == 8)
    {
        operation = geometry::ConvexPolygonOperation::Union;
    }
    else if (m_currentTask == 9)
    {
        operation = geometry::ConvexPolygonOperation::Difference;
    }
    const geometry::PolygonSet result = geometry::convexPolygonBoolean(first, second, operation);

    const auto toScreen = [](const std::vector<geometry::Point> &polygon)
    {
        std::vector<QPointF> converted;
        converted.reserve(polygon.size());
        for (const geometry::Point &point : polygon)
            converted.push_back(geometry_qt::toScreen(point));
        return converted;
    };
    std::vector<std::vector<QPointF>> screenResult;
    screenResult.reserve(result.size());
    for (const auto &contour : result)
        screenResult.push_back(toScreen(contour));
    QColor resultColor(220, 38, 38); // пересечение
    if (m_currentTask == 8)
    {
        resultColor = QColor(124, 58, 237); // объединение
    }
    else if (m_currentTask == 9)
    {
        resultColor = QColor(225, 29, 72); // разность
    }
    ui->canvas->setPolygonOperation(toScreen(first), toScreen(second), screenResult, resultColor);
    ui->resultLabel->setText(QString("Контуров результата: %1").arg(static_cast<qulonglong>(result.size())));
    emit logMessage(QString("Операция над выпуклыми многоугольниками: %1 контуров.")
                        .arg(static_cast<qulonglong>(result.size())));
}

void GeometryPage::handleBezierClick(const QPointF &screenPos)
{
    m_points.push_back(geometry_qt::toGeometry(screenPos));
    const int index = ui->canvas->addPoint(screenPos);
    ui->canvas->setPointLabel(index, QString("P%1").arg(index));
    if (index > 0)
        ui->canvas->addSegment(index - 1, index, QColor(148, 163, 184));
    if (ui->onlineCheckBox->isChecked())
        computeBezier();
    else
        ui->resultLabel->setText(QString("Контрольных точек: %1").arg(static_cast<qulonglong>(m_points.size())));
}

void GeometryPage::handleBezierDrag(int index, const QPointF &screenPos)
{
    if (index < 0 || index >= static_cast<int>(m_points.size()))
        return;
    m_points[index] = geometry_qt::toGeometry(screenPos);
    if (ui->onlineCheckBox->isChecked())
        computeBezier();
}

void GeometryPage::computeBezier()
{
    std::vector<std::vector<QPointF>> curves;
    const auto convert = [](const std::vector<geometry::Point> &src)
    {std::vector<QPointF> r; r.reserve(src.size());for(const auto&p:src)r.push_back(geometry_qt::toScreen(p));return r; };
    if (m_points.size() >= 3)
        curves.push_back(convert(bezier::composite(m_points, 2)));
    if (m_points.size() >= 4)
        curves.push_back(convert(bezier::composite(m_points, 3)));
    if (m_points.size() >= 2)
        curves.push_back(convert(bezier::sample(m_points)));
    const double t = ui->bezierSlider->value() / 100.0;
    std::vector<std::vector<QPointF>> levels;
    if (m_points.size() >= 2)
        levels = [&]
        {std::vector<std::vector<QPointF>> r; for(const auto& level:bezier::deCasteljauLevels(m_points,t))r.push_back(convert(level));return r; }();
    const QPointF marker = m_points.empty() ? QPointF() : geometry_qt::toScreen(bezier::deCasteljau(m_points, t));
    ui->canvas->setBezierCurves(curves, levels, marker, m_points.size() >= 2);
    ui->resultLabel->setText(QString("Контрольных точек: %1, t = %2")
                                 .arg(static_cast<qulonglong>(m_points.size()))
                                 .arg(t, 0, 'f', 2));
}
