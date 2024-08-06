#pragma once

#include <QObject>
#include <QColor>
#include <QPixmap>
#include <QFile>
#include <QPainter>
#include "flashdatetime.h"

namespace flashmath
{
constexpr double earth_r = 6378137;
double           deg2rad(double deg);
double           rad2deg(double rad);
double           getDistance(QPoint p1, QPoint p2);
double           getAngle(QPoint p1, QPoint p2);
double           sqr(double x);
int              getPolylinePointIdxAt(QPoint p0, QPolygon polyline,
                                       int tolerance_pix);
bool isNearPolyline(const QPoint& p0, const QPolygon& polyline,
                    int tolerance_pix);
}

class FlashGeoCoor
{
  friend struct FlashGeoRect;
  friend struct FlashGeoPolygon;
  int lat = 0;
  int lon = 0;

public:
  FlashGeoCoor();
  FlashGeoCoor(int lat, int lon);
  static FlashGeoCoor fromDegs(double lat, double lon);
  static FlashGeoCoor fromMeters(QPointF m);
  QPointF             toMeters() const;
  double              longitude() const;
  double              latitude() const;
  bool                isValid();
};

struct FlashGeoRect
{
  FlashGeoCoor top_left;
  FlashGeoCoor bottom_right;
  FlashGeoRect united(const FlashGeoRect&) const;
  bool         isNull() const;
  QRectF       toMeters() const;
  QSizeF       getSizeMeters() const;
  QRectF       toRectM() const;
};

struct FlashGeoPolygon: public QVector<FlashGeoCoor>
{
  FlashGeoRect getFrame() const;
  void         save(QByteArray& ba, int coor_precision_coef) const;
  void load(const QByteArray& ba, int& pos, int coor_precision_coef);
  QPolygonF toPolygonM();
};
