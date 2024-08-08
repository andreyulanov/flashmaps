#include "qgeomapreplyflashmaps.h"
#include "flashrender.h"
#include <QDebug>

static FlashRender* getRender()
{
  auto ret = static_cast<FlashRender*>(
      qApp->findChild<QObject*>("FlashRender"));
  if (!ret)
    qDebug() << "Failed to get \"FlashRender\" object! Use "
                "setObjectName() for initialization";
  return ret;
}

QGeoMapReplyFlashmaps::QGeoMapReplyFlashmaps(const QGeoTileSpec& spec,
                                             QObject* parent):
    QGeoTiledMapReply(spec, parent)
{
  FlashRender::TileSpec t = {tileSpec().x(), tileSpec().y(),
                             tileSpec().zoom()};

  FlashRender* render = getRender();
  if (!render)
    return;
  auto tile_pixmap = render->getTile(t);
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
  FlashRender*          render = getRender();
  if (!render)
    return;
  auto tile_pixmap = render->getTile(t);
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
