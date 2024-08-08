#include <math.h>
#include <QDebug>
#include <QCryptographicHash>
#include "flashobject.h"
#include "flashserialize.h"

void FlashObject::load(QVector<FlashClass>& class_list, int& pos,
                          const QByteArray& ba)
{
  using namespace FlashSerialize;

  uchar has_name;
  read(ba, pos, has_name);
  if (has_name)
    read(ba, pos, name);

  read(ba, pos, attributes);

  read(ba, pos, class_idx);
  auto cl = &class_list[class_idx];

  uchar is_multi_polygon;
  read(ba, pos, is_multi_polygon);

  uchar is_point;
  read(ba, pos, is_point);

  if (is_point)
  {
    FlashGeoCoor p;
    memcpy((char*)&p, (char*)&ba.data()[pos], sizeof(p));
    pos += sizeof(p);
    FlashGeoPolygon polygon;
    polygon.append(p);
    polygons.append(polygon);
    frame.top_left     = p;
    frame.bottom_right = p;
    return;
  }

  if (is_multi_polygon)
  {
    int polygon_count;
    read(ba, pos, polygon_count);
    polygons.resize(polygon_count);
    for (std::size_t i = 0; auto& polygon: polygons)
    {
      polygon.load(ba, pos, cl->coor_precision_coef);
      if (i++ == 0)
        frame = polygon.getFrame();
      else
        frame = frame.united(polygon.getFrame());
    }
  }
  else
  {
    polygons.resize(1);
    polygons[0].load(ba, pos, cl->coor_precision_coef);
    frame = polygons[0].getFrame();
  }
}

QByteArray FlashObject::getHash64() const
{
  QCryptographicHash hash(QCryptographicHash::Md4);
  QByteArray         ba;
  ba += name.toUtf8();
  QMapIterator<QString, QByteArray> it(attributes);
  while (it.hasNext())
  {
    it.next();
    ba += it.key().toUtf8();
    ba += it.value();
  }
  for (auto polygon: polygons)
    for (auto point: polygon)
      ba.append((char*)&point, sizeof(point));
  hash.addData(ba);
  auto res = hash.result();
  res.truncate(8);
  return res;
}

void FlashObject::save(const QVector<FlashClass>& class_list,
                          QByteArray&                ba) const
{
  using namespace FlashSerialize;
  write(ba, (uchar)(name.count() > 0));
  if (!name.isEmpty())
    write(ba, name);

  write(ba, attributes);

  auto cl = &class_list[class_idx];
  write(ba, class_idx);
  write(ba, (uchar)(polygons.count() > 1));
  write(ba, (uchar)(frame.isNull()));

  if (frame.isNull())
  {
    write(ba, polygons.first().first());
    return;
  }

  if (polygons.count() == 1)
    polygons[0].save(ba, cl->coor_precision_coef);
  else
  {
    write(ba, polygons.count());
    for (auto& polygon: polygons)
      polygon.save(ba, cl->coor_precision_coef);
  }
}
