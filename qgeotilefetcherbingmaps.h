#ifndef QGEOTILEFETCHERBINGMAPS_H
#define QGEOTILEFETCHERBINGMAPS_H

#include "qgeoserviceproviderpluginbingmaps.h"
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

  QGeoTiledMapReply* getTileImage(const QGeoTileSpec& spec);

private:
  Q_DISABLE_COPY(QGeoTileFetcherBingmaps)

  QPointer<QGeoTiledMappingManagerEngineBingmaps> m_engineBingmaps;
  QPointer<KRender>                               m_render;
};

#endif
