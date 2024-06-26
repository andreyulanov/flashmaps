#include "qgeomapreplyflashmaps.h"
#include <QDebug>

QGeoMapReplyFlashmaps::QGeoMapReplyFlashmaps(FlashRender* render,
                                             const QGeoTileSpec& spec,
                                             QObject* parent):
    QGeoTiledMapReply(spec, parent),
    m_render(render)
{
  FlashRender::TileCoor t           = {tileSpec().x(), tileSpec().y(),
                                   tileSpec().zoom()};
  auto              tile_pixmap = m_render->getTile(t);
  if (tile_pixmap.isEmpty())
  {
    connect(m_render, &FlashRender::finished, this,
            &QGeoMapReplyFlashmaps::onFinishedRender);
    m_render->requestTile({spec.x(), spec.y(), spec.zoom()});
  }
  else
  {
    setMapImageData(tile_pixmap);
    setFinished(true);
  }
}

void QGeoMapReplyFlashmaps::onFinishedRender()
{
  auto              spec        = tileSpec();
  FlashRender::TileCoor t           = {spec.x(), spec.y(), spec.zoom()};
  auto              tile_pixmap = m_render->getTile(t);
  if (tile_pixmap.isEmpty())
  {
    m_render->requestTile({spec.x(), spec.y(), spec.zoom()});
    setFinished(false);
  }
  else
  {
    setMapImageData(tile_pixmap);
    setFinished(true);
  }
}
