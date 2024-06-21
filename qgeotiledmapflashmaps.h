#pragma once

#include "QtLocation/private/qgeotiledmap_p.h"
#include <QtGui/QImage>
#include <QtCore/QPointer>

class QGeoTiledMappingManagerEngineFlashmaps;

class QGeoTiledMapFlashmaps: public QGeoTiledMap
{
  Q_OBJECT
public:
  QGeoTiledMapFlashmaps(
      QGeoTiledMappingManagerEngineFlashmaps* engine,
      QObject*                                parent = 0);
  ~QGeoTiledMapFlashmaps();

  QString getViewCopyright();
  void    evaluateCopyrights(const QSet<QGeoTileSpec>& visibleTiles);

private:
  // QImage m_logo;
  QImage  m_copyrightsSlab;
  QString m_lastCopyrightsString;
  QPointer<QGeoTiledMappingManagerEngineFlashmaps> m_engine;

  Q_DISABLE_COPY(QGeoTiledMapFlashmaps)
};
