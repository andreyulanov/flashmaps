﻿#include "QtLocation/private/qgeocameracapabilities_p.h"
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
  types << QGeoMapType(QGeoMapType::StreetMap, "Flash Street Map",
                       "Flash street map", false, false, 1,
                       "flashmaps", capabilities, parameters);

  setSupportedMapTypes(types);

  QGeoTileFetcherFlashmaps* fetcher =
      new QGeoTileFetcherFlashmaps(parameters, this);
  setTileFetcher(fetcher);

  if (parameters.contains(QStringLiteral("flashmaps.cachefolder")))
    m_cacheDirectory =
        parameters.value(QStringLiteral("flashmaps.cachefolder"))
            .toString()
            .toLatin1();
  else
    m_cacheDirectory = QAbstractGeoTileCache::baseCacheDirectory() +
                       QLatin1String("flashmaps");

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
