#include "QtLocation/private/qgeocameracapabilities_p.h"
#include "qgeotiledmappingmanagerengineflashmaps.h"
#include "qgeotiledmapflashmaps.h"
#include "qgeotilefetcherflashmaps.h"
#include "QtLocation/private/qgeotilespec_p.h"
#include "QtLocation/private/qgeofiletilecache_p.h"

#include <QDebug>
#include <QDir>
#include <QVariant>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/qmath.h>
#include <QtCore/qstandardpaths.h>

QGeoTiledMappingManagerEngineFlashmaps::
    QGeoTiledMappingManagerEngineFlashmaps(
        const QVariantMap& parameters, QGeoServiceProvider::Error*,
        QString*):
    QGeoTiledMappingManagerEngine()
{
  QGeoCameraCapabilities capabilities;

  capabilities.setMinimumZoomLevel(0.0);
  capabilities.setMaximumZoomLevel(24.0);

  setCameraCapabilities(capabilities);
  setTileSize(QSize(256, 256));
  setCacheHint(QAbstractGeoTileCache::MemoryCache);

  QList<QGeoMapType> types;
#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
  types << QGeoMapType(QGeoMapType::StreetMap, "Bing Street Map",
                       "Bing street map", false, false, 1);
  types << QGeoMapType(QGeoMapType::SatelliteMapDay,
                       "Bing Satellite Map", "Bing satellite map",
                       false, false, 2);
  types << QGeoMapType(QGeoMapType::HybridMap, "Bing Hybrid Map",
                       "Bing hybrid map", false, false, 3);
#elif QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
  types << QGeoMapType(QGeoMapType::StreetMap, "Bing Street Map",
                       "Bing street map", false, false, 1,
                       "bingmaps");
  types << QGeoMapType(QGeoMapType::SatelliteMapDay,
                       "Bing Satellite Map", "Bing satellite map",
                       false, false, 2, "bingmaps");
  types << QGeoMapType(QGeoMapType::HybridMap, "Bing Hybrid Map",
                       "Bing hybrid map", false, false, 3,
                       "bingmaps");
#else
  types << QGeoMapType(QGeoMapType::StreetMap, "Bing Street Map",
                       "Bing street map", false, false, 1, "bingmaps",
                       capabilities, parameters);
  types << QGeoMapType(QGeoMapType::SatelliteMapDay,
                       "Bing Satellite Map", "Bing satellite map",
                       false, false, 2, "bingmaps", capabilities,
                       parameters);
  types << QGeoMapType(QGeoMapType::HybridMap, "Bing Hybrid Map",
                       "Bing hybrid map", false, false, 3, "bingmaps",
                       capabilities, parameters);
#endif

  setSupportedMapTypes(types);

  QGeoTileFetcherFlashmaps* fetcher =
      new QGeoTileFetcherFlashmaps(parameters, this);
  setTileFetcher(fetcher);

  if (parameters.contains(QStringLiteral("bingmaps.cachefolder")))
    m_cacheDirectory =
        parameters.value(QStringLiteral("bingmaps.cachefolder"))
            .toString()
            .toLatin1();
  else
    m_cacheDirectory = QAbstractGeoTileCache::baseCacheDirectory() +
                       QLatin1String("bingmaps");

  QAbstractGeoTileCache* tileCache =
      new QGeoFileTileCache(m_cacheDirectory);
  setTileCache(tileCache);
  tileCache->clearAll();

  populateMapSchemes();
}

QGeoTiledMappingManagerEngineFlashmaps::
    ~QGeoTiledMappingManagerEngineFlashmaps()
{
}

void QGeoTiledMappingManagerEngineFlashmaps::populateMapSchemes()
{
  m_mapSchemes[0] = QStringLiteral("roadmap");
  m_mapSchemes[1] = QStringLiteral("roadmap");
  m_mapSchemes[2] = QStringLiteral("satellite");
  m_mapSchemes[3] = QStringLiteral("hybrid");
}

QString QGeoTiledMappingManagerEngineFlashmaps::getCacheDirectory()
{
  return m_cacheDirectory;
}

QString QGeoTiledMappingManagerEngineFlashmaps::getScheme(int mapId)
{
  return m_mapSchemes[mapId];
}

QGeoMap* QGeoTiledMappingManagerEngineFlashmaps::createMap()
{
  return new QGeoTiledMapFlashmaps(this);
}
