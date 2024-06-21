#include "qgeotiledmapflashmaps.h"
#include "qgeotiledmappingmanagerengineflashmaps.h"
#if QT_VERSION <= QT_VERSION_CHECK(5, 6, 0)
  #include "QtLocation/private/qgeomapcontroller_p.h"
#endif

/*!
 Constructs a new tiled map data object, which stores the map data
 required by \a geoMap and makes use of the functionality provided by
 \a engine.
 */
QGeoTiledMapBingmaps::QGeoTiledMapBingmaps(
    QGeoTiledMappingManagerEngineBingmaps* engine,
    QObject*                               parent /*= 0*/):
    QGeoTiledMap(engine, parent),
    m_engine(engine)
{
}

QGeoTiledMapBingmaps::~QGeoTiledMapBingmaps()
{
}

void QGeoTiledMapBingmaps::evaluateCopyrights(
    const QSet<QGeoTileSpec>& visibleTiles)
{
  Q_UNUSED(visibleTiles);
}
