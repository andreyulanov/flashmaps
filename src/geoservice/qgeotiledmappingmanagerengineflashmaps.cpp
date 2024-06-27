#include "QtLocation/private/qgeocameracapabilities_p.h"
#include "qgeotiledmappingmanagerengineflashmaps.h"
#include "qgeotiledmapflashmaps.h"
#include "qgeotilefetcherflashmaps.h"
#include "QtLocation/private/qgeotilespec_p.h"
#include "QtLocation/private/qgeofiletilecache_p.h"
#include <QStandardPaths>

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
}

QString QGeoTiledMappingManagerEngineFlashmaps::getCacheDirectory()
{
  return m_cacheDirectory;
}

QString QGeoTiledMappingManagerEngineFlashmaps::getMapDirectory()
{
  return m_mapDirectory;
}

QGeoMap* QGeoTiledMappingManagerEngineFlashmaps::createMap()
{
  return new QGeoTiledMapFlashmaps(this);
}
