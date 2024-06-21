#include "qgeomapreplyflashmaps.h"
#include <QDebug>

QGeoMapReplyBingmaps::QGeoMapReplyBingmaps(KRender*            render,
                                           const QGeoTileSpec& spec,
                                           QObject* parent):
    QGeoTiledMapReply(spec, parent),
    m_render(render)
{
  KRender::Tile t           = {tileSpec().x(), tileSpec().y(),
                               tileSpec().zoom()};
  auto          tile_pixmap = m_render->pickTile(t);
  if (tile_pixmap.isEmpty())
  {
    connect(m_render, &KRender::finished, this,
            &QGeoMapReplyBingmaps::onFinishedRender);
    m_render->requestTile({spec.x(), spec.y(), spec.zoom()});
    setFinished(false);
  }
  else
  {
    setMapImageData(tile_pixmap);
    setFinished(true);
  }
}

void QGeoMapReplyBingmaps::onFinishedRender()
{
  auto          spec        = tileSpec();
  KRender::Tile t           = {spec.x(), spec.y(), spec.zoom()};
  auto          tile_pixmap = m_render->pickTile(t);
  if (tile_pixmap.isEmpty())
  {
    connect(m_render, &KRender::finished, this,
            &QGeoMapReplyBingmaps::onFinishedRender);
    m_render->requestTile({spec.x(), spec.y(), spec.zoom()});
  }
  else
  {
    setMapImageData(tile_pixmap);
    setFinished(true);
  }
}
