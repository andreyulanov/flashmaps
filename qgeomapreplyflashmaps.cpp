#include "qgeomapreplyflashmaps.h"
#include <QDebug>

QGeoMapReplyFlashmaps::QGeoMapReplyFlashmaps(KRender*            render,
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
            &QGeoMapReplyFlashmaps::onFinishedRender);
    m_render->requestTile({spec.x(), spec.y(), spec.zoom()});
    setFinished(false);
  }
  else
  {
    setMapImageData(tile_pixmap);
    setFinished(true);
  }
}

void QGeoMapReplyFlashmaps::onFinishedRender()
{
  auto          spec        = tileSpec();
  KRender::Tile t           = {spec.x(), spec.y(), spec.zoom()};
  auto          tile_pixmap = m_render->pickTile(t);
  if (tile_pixmap.isEmpty())
  {
    connect(m_render, &KRender::finished, this,
            &QGeoMapReplyFlashmaps::onFinishedRender);
    m_render->requestTile({spec.x(), spec.y(), spec.zoom()});
  }
  else
  {
    setMapImageData(tile_pixmap);
    setFinished(true);
  }
}