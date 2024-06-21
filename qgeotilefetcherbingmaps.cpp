#include "qgeotilefetcherbingmaps.h"
#include "qgeomapreplybingmaps.h"
#include "qgeotiledmapbingmaps.h"
#include "qgeotiledmappingmanagerenginebingmaps.h"
#include <QtLocation/private/qgeotilespec_p.h>

#include <QDebug>
#include <QDir>
#include "krender/krender.h"

QGeoTileFetcherBingmaps::QGeoTileFetcherBingmaps(
    const QVariantMap&,
    QGeoTiledMappingManagerEngineBingmaps* engine):
    QGeoTileFetcher(engine),
    m_engineBingmaps(engine)
{
}

QGeoTileFetcherBingmaps::~QGeoTileFetcherBingmaps()
{
  delete m_render;
}

QGeoTiledMapReply*
QGeoTileFetcherBingmaps::getTileImage(const QGeoTileSpec& spec)
{
  qDebug() << "getTileImage(), spec=" << spec;
  if (!m_render)
  {
    KRender::Settings s;
    s.map_dir = "/home/user/flashmaps/data/packs";
    m_render  = new KRender(s);
  }
  QGeoTiledMapReply* mapReply =
      new QGeoMapReplyBingmaps(m_render, spec);

  return mapReply;
}
