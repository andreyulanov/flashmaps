#include "qgeotilefetcherflashmaps.h"
#include "qgeomapreplyflashmaps.h"
#include "qgeotiledmapflashmaps.h"
#include "qgeotiledmappingmanagerengineflashmaps.h"
#include <QtLocation/private/qgeotilespec_p.h>

#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QScreen>
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
    s.map_dir              = m_engineFlashmaps->getMapDirectory();
    auto   screen          = QGuiApplication::screens().first();
    QSize  screen_size_pix = screen->availableSize();
    QSizeF screen_size_mm  = screen->physicalSize();

    s.pixel_size_mm =
        screen_size_mm.width() / screen_size_pix.width();

    QSysInfo si;
    auto is_device = si.productType().toLower().contains("android");
    if (!is_device)
      s.pixel_size_mm *= 0.5;

    m_render = new FlashRender(s);
  }
  QGeoTiledMapReply* mapReply =
      new QGeoMapReplyFlashmaps(m_render, spec);

  return mapReply;
}
