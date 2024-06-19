#include "qgeotilefetcherbingmaps.h"
#include "qgeomapreplybingmaps.h"
#include "qgeotiledmapbingmaps.h"
#include "qgeotiledmappingmanagerenginebingmaps.h"
#include <QtLocation/private/qgeotilespec_p.h>

#include <QDebug>
#include <QSize>
#include <QDir>
#include <QTime>
#include "krender/krender.h"

#include <math.h>

#include <map>

QGeoTileFetcherBingmaps::QGeoTileFetcherBingmaps(
    const QVariantMap&,
    QGeoTiledMappingManagerEngineBingmaps* engine):
    QGeoTileFetcher(engine),
    m_engineBingmaps(engine)
{
}

QGeoTiledMapReply*
QGeoTileFetcherBingmaps::getTileImage(const QGeoTileSpec& spec)
{
  KRender::Settings s;
  if (!render)
  {
    render = new KRender(s);
    render->addPack("/home/user/flashmaps/data/packs/world.kpack",
                    true);
    render->addPack(
        "/home/user/flashmaps/data/packs/ru-spe,ru-len.kpack", false);
  }
  QGeoTiledMapReply* mapReply =
      new QGeoMapReplyBingmaps(render, spec);
  render->requestTile({spec.x(), spec.y(), spec.zoom()});

  return mapReply;
}
