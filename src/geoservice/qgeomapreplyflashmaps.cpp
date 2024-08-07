#include "qgeomapreplyflashmaps.h"
#include "flashrender.h"
#include <QDebug>

QGeoMapReplyFlashmaps::QGeoMapReplyFlashmaps(const QGeoTileSpec& spec,
                                             QObject* parent):
    QGeoTiledMapReply(spec, parent)
{
  FlashRender::TileSpec t = {tileSpec().x(), tileSpec().y(),
                             tileSpec().zoom()};

  FlashRender* render      = FlashRender::instance();
  auto         tile_pixmap = render->getTile(t);
  if (tile_pixmap.isEmpty())
  {
    connect(render, &FlashRender::finished, this,
            &QGeoMapReplyFlashmaps::onFinishedRender,
            Qt::UniqueConnection);
    render->requestTile({spec.x(), spec.y(), spec.zoom()});
  }
  else
  {
    setMapImageData(tile_pixmap);
    setFinished(true);
  }
}

void QGeoMapReplyFlashmaps::onFinishedRender()
{
  auto                  spec   = tileSpec();
  FlashRender::TileSpec t      = {spec.x(), spec.y(), spec.zoom()};
  FlashRender*          render = FlashRender::instance();
  auto                  tile_pixmap = render->getTile(t);
  if (tile_pixmap.isEmpty())
  {
    render->requestTile({spec.x(), spec.y(), spec.zoom()});
    setFinished(false);
  }
  else
  {
    setMapImageData(tile_pixmap);
    setFinished(true);
  }
}
