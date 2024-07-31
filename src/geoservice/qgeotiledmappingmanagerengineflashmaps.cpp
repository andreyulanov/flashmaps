#include "QtLocation/private/qgeocameracapabilities_p.h"
#include "qgeotiledmappingmanagerengineflashmaps.h"
#include "qgeotiledmapflashmaps.h"
#include "qgeotilefetcherflashmaps.h"
#include "QtLocation/private/qgeotilespec_p.h"
#include "QtLocation/private/qgeofiletilecache_p.h"
#include <QStandardPaths>
#include <QGuiApplication>
#include <QScreen>

#include <QDebug>
#include <QDir>
#include <QVariant>

QGeoTiledMappingManagerEngineFlashmaps::
    QGeoTiledMappingManagerEngineFlashmaps(
        const QVariantMap& parameters, QGeoServiceProvider::Error*,
        QString*):
    QGeoTiledMappingManagerEngine()
{
  QGeoCameraCapabilities capabilities;

  capabilities.setMinimumZoomLevel(0.0);
  capabilities.setMaximumZoomLevel(21.0);

  setCameraCapabilities(capabilities);
  setTileSize(QSize(256, 256));

  QList<QGeoMapType> types;
  types << QGeoMapType(QGeoMapType::StreetMap, "Flash Street Map",
                       "Flash street map", false, false, 1,
                       "flashmaps", capabilities, parameters);

  setSupportedMapTypes(types);

  QGeoTileFetcherFlashmaps* fetcher =
      new QGeoTileFetcherFlashmaps(parameters, this);
  setTileFetcher(fetcher);

  m_cacheDirectory =
      QAbstractGeoTileCache::baseCacheDirectory() + "flashmaps";

  QStringList dir_list = QStandardPaths::standardLocations(
      QStandardPaths::AppDataLocation);
  for (auto dir: dir_list)
  {
    auto mapdir = dir + "/maps";
    if (QDir(mapdir).exists())
    {
      m_mapDirectory = mapdir;
      break;
    }
  }

  QAbstractGeoTileCache* tileCache =
      new QGeoFileTileCache(m_cacheDirectory);
  setTileCache(tileCache);
  tileCache->clearAll();

  FlashRender::Settings s;
  s.map_dir              = getMapDirectory();
  auto   screen          = QGuiApplication::screens().first();
  QSize  screen_size_pix = screen->availableSize();
  QSizeF screen_size_mm  = screen->physicalSize();

  s.pixel_size_mm = screen_size_mm.width() / screen_size_pix.width();

  QSysInfo si;
  auto     is_device = si.productType().toLower().contains("android");
  if (!is_device)
    s.pixel_size_mm *= 0.5;

  m_render.reset(new FlashRender(s));
}

QString QGeoTiledMappingManagerEngineFlashmaps::getCacheDirectory()
{
  return m_cacheDirectory;
}

QString QGeoTiledMappingManagerEngineFlashmaps::getMapDirectory()
{
  return m_mapDirectory;
}

FlashRender* QGeoTiledMappingManagerEngineFlashmaps::getRender()
{
  return m_render.data();
}

QGeoMap* QGeoTiledMappingManagerEngineFlashmaps::createMap()
{
  return new QGeoTiledMapFlashmaps(this);
}
