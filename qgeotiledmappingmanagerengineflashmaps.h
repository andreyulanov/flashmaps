#pragma once

#include "QtLocation/private/qgeotiledmappingmanagerengine_p.h"
#include "QtLocation/private/qgeomaptype_p.h"
#include <QGeoServiceProvider>

class QGeoTiledMappingManagerEngineFlashmaps:
    public QGeoTiledMappingManagerEngine
{
  Q_OBJECT

public:
  QGeoTiledMappingManagerEngineFlashmaps(
      const QVariantMap&          parameters,
      QGeoServiceProvider::Error* error, QString* errorString);

  virtual QGeoMap* createMap();
  QString          getCacheDirectory();
  QString          getMapDirectory();

private:
  QString m_cacheDirectory;
  QString m_mapDirectory;
};
