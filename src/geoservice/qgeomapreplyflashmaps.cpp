#include "qgeomapreplyflashmaps.h"
#include <QDebug>

QGeoMapReplyFlashmaps::QGeoMapReplyFlashmaps(const QGeoTileSpec& spec,
                                             QObject* parent):
    QGeoTiledMapReply(spec, parent)
{
  FlashRender::TileCoor t = {tileSpec().x(), tileSpec().y(),
                             tileSpec().zoom()};
  auto tile_pixmap        = FlashRender::instance().getTile(t);
  if (tile_pixmap.isEmpty())
  {
    connect(&FlashRender::instance(), &FlashRender::finished, this,
            &QGeoMapReplyFlashmaps::onFinishedRender,
            Qt::UniqueConnection);
    FlashRender::instance().requestTile(
        {spec.x(), spec.y(), spec.zoom()});
  }
  else
  {
    setMapImageData(tile_pixmap);
    setFinished(true);
  }
}

void QGeoMapReplyFlashmaps::onFinishedRender()
{
  auto                  spec = tileSpec();
  FlashRender::TileCoor t    = {spec.x(), spec.y(), spec.zoom()};
  auto tile_pixmap           = FlashRender::instance().getTile(t);
  if (tile_pixmap.isEmpty())
  {
    FlashRender::instance().requestTile(
        {spec.x(), spec.y(), spec.zoom()});
    setFinished(false);
  }
  else
  {
    setMapImageData(tile_pixmap);
    setFinished(true);
  }
}
