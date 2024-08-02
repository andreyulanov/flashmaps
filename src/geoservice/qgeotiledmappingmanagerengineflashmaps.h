#pragma once

#include "QtLocation/private/qgeotiledmappingmanagerengine_p.h"
#include "QtLocation/private/qgeomaptype_p.h"
#include "flashrender.h"
#include <QGeoServiceProvider>
#include <QPointer>

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

private:
  QString m_cacheDirectory;
};
