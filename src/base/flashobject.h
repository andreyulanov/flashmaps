#pragma once

#include <QMap>
#include "flashclass.h"

struct FlashMapObject
{
  int                       class_idx = 0;
  QString                   name;
  QMap<QString, QByteArray> attributes;
  FlashGeoRect                  frame;
  QVector<FlashGeoPolygon>      polygons;

public:
  void save(const QVector<FlashClass>& class_list, QByteArray& ba) const;
  void load(QVector<FlashClass>& class_list, int& pos,
            const QByteArray& ba);
  FlashGeoCoor getCenter();
};

struct FlashFreeObject: public FlashMapObject
{
  FlashClass cl;

  FlashFreeObject(FlashMapObject obj = FlashMapObject());
  void  save(QString path);
  void  load(QString path, double pixel_size_mm);
  int   getWidthPix(double pixel_size_mm);
  void  setGuid(QByteArray guid_ba);
  void  setGuid(QUuid guid);
  QUuid getGuid() const;
};
