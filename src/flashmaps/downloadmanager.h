#pragma once

#include <QThread>
#include <QPolygonF>
#include <QMap>
#include "flashmap.h"

class DownloadManager: public QThread
{
  Q_OBJECT

  struct LocalMapEntry
  {
    QRectF             rect;
    QVector<QPolygonF> polygons;
  };
  QMap<QString, LocalMapEntry> iso_metrics_map;
  QStringList                  name_list;
  QRectF                       requested_rect;
  QString                      map_dir;
  double                       max_mip;

  void run();

signals:
  void fetched(QString map_name);

public:
  DownloadManager(QString map_dir, const FlashMap* world_map,
                  double _max_mip);
  void requestRect(QRectF, double mip);
};
