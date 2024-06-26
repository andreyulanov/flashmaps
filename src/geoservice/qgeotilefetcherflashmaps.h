#pragma once

#include "qgeoserviceproviderpluginflashmaps.h"
#include <QtLocation/private/qgeotilefetcher_p.h>
#include "flashrender/flashrender.h"

class QGeoTileSpec;
class QGeoTiledMappingManagerEngine;
class QGeoTiledMappingManagerEngineFlashmaps;

class QGeoTileFetcherFlashmaps: public QGeoTileFetcher
{
  Q_OBJECT

public:
  QGeoTileFetcherFlashmaps(
      const QVariantMap&,
      QGeoTiledMappingManagerEngineFlashmaps* engine);
  ~QGeoTileFetcherFlashmaps();

  QGeoTiledMapReply* getTileImage(const QGeoTileSpec& spec);

private:
  Q_DISABLE_COPY(QGeoTileFetcherFlashmaps)

  QPointer<QGeoTiledMappingManagerEngineFlashmaps> m_engineFlashmaps;
  FlashRender*                                     m_render = nullptr;
};
