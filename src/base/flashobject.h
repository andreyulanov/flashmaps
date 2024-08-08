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
  void       save(const QVector<FlashClass>& class_list,
                  QByteArray&                ba) const;
  void       load(QVector<FlashClass>& class_list, int& pos,
                  const QByteArray& ba);
  QByteArray getHash64() const;
};

struct FlashFreeObject: public FlashMapObject
{
  FlashClass cl;
  void       saveToFile(QFile*) const;
  void       loadFromFile(QFile*, double _pixel_size_mm);

public:
  FlashFreeObject(FlashMapObject obj = FlashMapObject(),
                  FlashClass     cl  = FlashClass());
  void save(QString path) const;
  void load(QString path, double pixel_size_mm);
};
