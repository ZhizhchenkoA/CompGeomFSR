#include "geometry_qt/conversion.h"

namespace geometry_qt {

geometry::Point toGeometry(const QPointF& screen)
{
    return geometry::Point{screen.x(), -screen.y()};
}

QPointF toScreen(const geometry::Point& geometryPoint)
{
    return QPointF(geometryPoint.x, -geometryPoint.y);
}

} // namespace geometry_qt