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
  ~QGeoTiledMappingManagerEngineFlashmaps();

  virtual QGeoMap* createMap();
  QString          getScheme(int mapId);
  QString          getCacheDirectory();

private:
  void initialize();
  void populateMapSchemes();

  QHash<int, QString> m_mapSchemes;
  QString             m_cacheDirectory;
};
