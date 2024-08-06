#include "QtLocation/private/qgeocameracapabilities_p.h"
#include "qgeotiledmappingmanagerengineflashmaps.h"
#include "qgeotiledmapflashmaps.h"
#include "qgeotilefetcherflashmaps.h"
#include "QtLocation/private/qgeotilespec_p.h"
#include "QtLocation/private/qgeofiletilecache_p.h"
#include <QGuiApplication>
#include <QScreen>
#include <QQmlEngine>
#include <QQmlContext>

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

  capabilities.setMinimumZoomLevel(2);
  capabilities.setMaximumZoomLevel(22);

  setCameraCapabilities(capabilities);
  setTileSize(QSize(256, 256));

  m_prefetchStyle = QGeoTiledMap::PrefetchTwoNeighbourLayers;

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

  QAbstractGeoTileCache* tileCache =
      new QGeoFileTileCache(m_cacheDirectory);
  setTileCache(tileCache);
  tileCache->clearAll();
}

QString QGeoTiledMappingManagerEngineFlashmaps::getCacheDirectory()
{
  return m_cacheDirectory;
}

QGeoMap* QGeoTiledMappingManagerEngineFlashmaps::createMap()
{
  return new QGeoTiledMapFlashmaps(this);
}
