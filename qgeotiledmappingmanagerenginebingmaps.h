#ifndef QGEOTILEDMAPPINGMANAGERENGINEBINGMAPS_H
#define QGEOTILEDMAPPINGMANAGERENGINEBINGMAPS_H

#include "QtLocation/private/qgeotiledmappingmanagerengine_p.h"
#include "QtLocation/private/qgeomaptype_p.h"
#include <QGeoServiceProvider>

class QGeoTiledMappingManagerEngineBingmaps:
    public QGeoTiledMappingManagerEngine
{
  Q_OBJECT

public:
  QGeoTiledMappingManagerEngineBingmaps(
      const QVariantMap&          parameters,
      QGeoServiceProvider::Error* error, QString* errorString);
  ~QGeoTiledMappingManagerEngineBingmaps();

  virtual QGeoMap* createMap();
  QString          getScheme(int mapId);
  QString          getCacheDirectory();

private:
  void initialize();
  void populateMapSchemes();

  QHash<int, QString> m_mapSchemes;
  QString             m_cacheDirectory;
};

#endif  // QGEOTILEDMAPPINGMANAGERENGINEBingmaps_H
