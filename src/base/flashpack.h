#pragma once

#include <QPolygon>
#include <QColor>
#include <QPainter>
#include <QReadWriteLock>
#include <QMap>
#include <QElapsedTimer>
#include <QVariant>
#include "flashobject.h"

struct FlashTile: public QVector<FlashMapObject>
{
  enum Status
  {
    Null,
    Loading,
    Loaded
  };
  Status status = Null;
};

struct FlashPack
{
  static constexpr int border_coor_precision_coef = 10000;

  double main_mip = 0;
  double tile_mip = 0;

  QVector<FlashClass>      classes;
  FlashGeoRect             frame;
  QVector<QPolygonF>   borders_m;
  QVector<FlashGeoPolygon> borders;
  FlashTile                main;
  QVector<FlashTile>       tiles;

  void                 save(QString path) const;
  void                 loadMain(QString path, bool load_objects,
                                double pixel_size_mm);
  void                 loadTile(QString path, int tile_idx);
  void                 loadAll(QString path, double pixel_size_mm);
  void                 clear();
  qint64               count();
  void                 addObject(FlashFreeObject free_obj);
  QVector<FlashFreeObject> getObjects();
};
