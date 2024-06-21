#include "qgeotiledmapflashmaps.h"
#include "qgeotiledmappingmanagerengineflashmaps.h"

/*!
 Constructs a new tiled map data object, which stores the map data
 required by \a geoMap and makes use of the functionality provided by
 \a engine.
 */
QGeoTiledMapFlashmaps::QGeoTiledMapFlashmaps(
    QGeoTiledMappingManagerEngineFlashmaps* engine,
    QObject*                                parent /*= 0*/):
    QGeoTiledMap(engine, parent),
    m_engine(engine)
{
}
