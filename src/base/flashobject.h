#pragma once

#include <QMap>
#include "flashclass.h"

struct FlashObject
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
