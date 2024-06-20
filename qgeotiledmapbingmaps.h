#ifndef QGEOMAPBINGMAPS_H
#define QGEOMAPBINGMAPS_H

#include "QtLocation/private/qgeotiledmap_p.h"
#include <QtGui/QImage>
#include <QtCore/QPointer>

class QGeoTiledMappingManagerEngineBingmaps;

class QGeoTiledMapBingmaps: public QGeoTiledMap
{
  Q_OBJECT
public:
  QGeoTiledMapBingmaps(QGeoTiledMappingManagerEngineBingmaps* engine,
                       QObject* parent = 0);
  ~QGeoTiledMapBingmaps();

  QString getViewCopyright();
  void    evaluateCopyrights(const QSet<QGeoTileSpec>& visibleTiles);

private:
  // QImage m_logo;
  QImage  m_copyrightsSlab;
  QString m_lastCopyrightsString;
  QPointer<QGeoTiledMappingManagerEngineBingmaps> m_engine;

  Q_DISABLE_COPY(QGeoTiledMapBingmaps)
};

#endif  // QGEOMAPBingmaps_H
