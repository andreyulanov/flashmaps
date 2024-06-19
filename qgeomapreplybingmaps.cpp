#include "qgeomapreplybingmaps.h"
#include <QNetworkAccessManager>
#include <QNetworkCacheMetaData>
#include <QDateTime>
#include <QBuffer>

QT_BEGIN_NAMESPACE

QGeoMapReplyBingmaps::QGeoMapReplyBingmaps(KRender*            render,
                                           const QGeoTileSpec& spec,
                                           QObject* parent):
    QGeoTiledMapReply(spec, parent),
    m_render(render)
{
  connect(m_render, &KRender::finished, this,
          &QGeoMapReplyBingmaps::findTiles, Qt::UniqueConnection);
}

void QGeoMapReplyBingmaps::findTiles()
{
  auto tile = m_render->getTile(
      {tileSpec().x(), tileSpec().y(), tileSpec().zoom()});

  if (tile.isEmpty())
    setFinished(false);
  else
  {
    setMapImageData(tile);
    setFinished(true);
  }
}

QT_END_NAMESPACE
