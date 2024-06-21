#ifndef QGEOTILEFETCHERFLASHMAPS_H
#define QGEOTILEFETCHERFLASHMAPS_H

#include "qgeoserviceproviderpluginflashmaps.h"
#include <QtLocation/private/qgeotilefetcher_p.h>
#include "krender/krender.h"

class QGeoTileSpec;
class QGeoTiledMappingManagerEngine;
class QGeoTiledMappingManagerEngineBingmaps;

class QGeoTileFetcherBingmaps: public QGeoTileFetcher
{
  Q_OBJECT

public:
  QGeoTileFetcherBingmaps(
      const QVariantMap&,
      QGeoTiledMappingManagerEngineBingmaps* engine);
  ~QGeoTileFetcherBingmaps();

  QGeoTiledMapReply* getTileImage(const QGeoTileSpec& spec);

private:
  Q_DISABLE_COPY(QGeoTileFetcherBingmaps)

  QPointer<QGeoTiledMappingManagerEngineBingmaps> m_engineBingmaps;
  KRender*                                        m_render = nullptr;
};

#endif
