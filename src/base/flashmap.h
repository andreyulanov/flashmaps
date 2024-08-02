#pragma once

#include <QPolygon>
#include <QColor>
#include <QPainter>
#include <QReadWriteLock>
#include <QMap>
#include <QElapsedTimer>
#include <QVariant>
#include "flashobject.h"

struct FlashVectorTile: public QVector<FlashMapObject>
{
  enum Status
  {
    Null,
    Loading,
    Loaded
  };
  Status status = Null;
};

struct FlashMap
{
  static constexpr int border_coor_precision_coef = 10000;

  double main_mip = 0;
  double tile_mip = 0;

  QVector<FlashClass>      classes;
  FlashGeoRect             frame;
  QVector<QPolygonF>       borders_m;
  QVector<FlashGeoPolygon> borders;
  FlashVectorTile                main;
  QVector<FlashVectorTile>       tiles;

  void   save(QString path) const;
  void   loadMainVectorTile(QString path, bool load_objects,
                  double pixel_size_mm);
  void   loadVectorTile(QString path, int tile_idx);
  void   loadAll(QString path, double pixel_size_mm);
  void   clear();
  qint64 count();
  void   addFreeObject(FlashFreeObject free_obj);
  QVector<FlashFreeObject> getFreeObjects();
  QVector<FlashVectorTile>       getVectorTiles();
};
