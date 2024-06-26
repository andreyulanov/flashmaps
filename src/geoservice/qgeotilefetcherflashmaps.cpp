#include "qgeotilefetcherflashmaps.h"
#include "qgeomapreplyflashmaps.h"
#include "qgeotiledmapflashmaps.h"
#include "qgeotiledmappingmanagerengineflashmaps.h"
#include <QtLocation/private/qgeotilespec_p.h>

#include <QDebug>
#include <QDir>
#include "flashrender/flashrender.h"

QGeoTileFetcherFlashmaps::QGeoTileFetcherFlashmaps(
    const QVariantMap&,
    QGeoTiledMappingManagerEngineFlashmaps* engine):
    QGeoTileFetcher(engine),
    m_engineFlashmaps(engine)
{
}

QGeoTileFetcherFlashmaps::~QGeoTileFetcherFlashmaps()
{
  delete m_render;
}

QGeoTiledMapReply*
QGeoTileFetcherFlashmaps::getTileImage(const QGeoTileSpec& spec)
{
  if (!m_render)
  {
    FlashRender::Settings s;
    s.map_dir = m_engineFlashmaps->getMapDirectory();
    m_render  = new FlashRender(s);
  }
  QGeoTiledMapReply* mapReply =
      new QGeoMapReplyFlashmaps(m_render, spec);

  return mapReply;
}
