#include "qgeotilefetcherflashmaps.h"
#include "qgeomapreplyflashmaps.h"
#include "qgeotiledmapflashmaps.h"
#include "qgeotiledmappingmanagerengineflashmaps.h"
#include <QtLocation/private/qgeotilespec_p.h>
#include <QDebug>

QGeoTileFetcherFlashmaps::QGeoTileFetcherFlashmaps(
    const QVariantMap&,
    QGeoTiledMappingManagerEngineFlashmaps* engine):
    QGeoTileFetcher(engine),
    m_engineFlashmaps(engine)
{
}

QGeoTiledMapReply*
QGeoTileFetcherFlashmaps::getTileImage(const QGeoTileSpec& spec)
{
  return new QGeoMapReplyFlashmaps(m_engineFlashmaps->getRender(),
                                   spec);
}
