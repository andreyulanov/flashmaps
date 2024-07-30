#pragma once

#include <QMap>
#include "flashclass.h"

struct FlashMapObject
{
  int                       class_idx = 0;
  QString                   name;
  QMap<QString, QByteArray> attributes;
  FlashGeoRect              frame;
  QVector<FlashGeoPolygon>  polygons;

public:
  void         save(const QVector<FlashClass>& class_list,
                    QByteArray&                ba) const;
  void         load(QVector<FlashClass>& class_list, int& pos,
                    const QByteArray& ba);
  FlashGeoCoor getCenter();
  QByteArray   getHash();
};

struct FlashFreeObject: public FlashMapObject
{
  FlashClass cl;
  void       saveToFile(QFile*) const;
  void       loadFromFile(QFile*, double _pixel_size_mm);

  FlashFreeObject(FlashMapObject obj = FlashMapObject());
  void  save(QString path) const;
  void  load(QString path, double pixel_size_mm);
  int   getWidthPix(double pixel_size_mm) const;
  void  setGuid(QByteArray guid_ba);
  void  setGuid(QUuid guid);
  QUuid getGuid() const;
};

struct FlashPack: QVector<FlashFreeObject>
{
  void save(QString path);
  void load(QString path, double pixel_size_mm);
  void addObject(QString path, FlashFreeObject obj);
};
