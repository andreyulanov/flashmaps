﻿#include "flashrender.h"
#include "flashlocker.h"
#include <QDir>
#include <QtConcurrent/QtConcurrent>
#include <QPainterPath>
#include <numeric>

using namespace flashmath;

QPoint FlashRender::meters2pix(QPointF coor_m) const
{
  auto rectm = getDrawRectM();
  int  pix_x = (coor_m.x() - rectm.left()) / mip;
  int  pix_y = (coor_m.y() - rectm.top()) / mip;
  return {pix_x, pix_y};
}
QPointF FlashRender::pix2meters(QPointF pix) const
{
  auto   rectm = getDrawRectM();
  double xm    = pix.x() * mip + rectm.left();
  double ym    = pix.y() * mip + rectm.top();
  return {xm, ym};
}

FlashRender::FlashRender(Settings v)
{
  s                 = v;
  int big_tile_side = tile_side * s.big_tile_multiplier;
  pixmap            = QPixmap{big_tile_side, big_tile_side};
  QDir dir(s.map_dir);
  dir.setNameFilters({"*.flashmap"});
  auto fi_list = dir.entryInfoList();
  for (int count = -1; auto fi: fi_list)
  {
    count++;
    int  idx      = count;
    bool load_now = false;
    if (fi.fileName() == s.world_map_name)
    {
      idx      = 0;
      load_now = true;
    }
    insertMap(idx, fi.filePath(), load_now);
  }
  connect(this, &QThread::finished, this, &FlashRender::onFinished);
}

FlashRender::~FlashRender()
{
  QThreadPool().globalInstance()->waitForDone();
  wait();
}

void FlashRender::requestTile(TileCoor t)
{
  if (t.z <= 3)
    return;
  if (isRunning())
    return;
  tile_coor = t;
  render();
}

QByteArray FlashRender::getTile(TileCoor t)
{
  auto tile_name = getTileName(t);
  for (auto big_tile: big_tiles)
  {
    if (big_tile.name == tile_name)
      return big_tile.data;
  }
  return QByteArray();
}

void FlashRender::insertMap(int idx, QString path, bool load_now)
{
  QThreadPool().globalInstance()->waitForDone();
  wait();
  auto map = new FlashRenderMap(path);
  map->loadMain(load_now, s.pixel_size_mm);
  maps.insert(idx, map);
}

QRectF FlashRender::getDrawRectM() const
{
  QSizeF size_m      = {pixmap.width() * mip, pixmap.height() * mip};
  QRectF draw_rect_m = {top_left_m.x(), top_left_m.y(),
                        size_m.width(), size_m.height()};
  return draw_rect_m;
}

void FlashRender::checkUnload()
{
  auto draw_rect_m  = getDrawRectM();
  int  loaded_count = 0;
  for (int i = -1; auto& map: maps)
  {
    i++;
    if (i == 0)
      continue;
    if (map->main.status == FlashTile::Loaded)
    {
      if (!needToLoadMap(map, draw_rect_m))
        if (loaded_count > s.max_loaded_maps_count)
          map->clear();
      loaded_count++;
    }
  }
}

bool FlashRender::needToLoadMap(const FlashRenderMap* map,
                                const QRectF&         draw_rect_m)
{
  if (map->main_mip > 0 && mip > map->main_mip)
    return false;
  auto map_rect_m       = map->frame.toMeters();
  bool frame_intersects = draw_rect_m.intersects(map_rect_m);
  if (!frame_intersects)
    return false;

  auto top_left_m = pix2meters({0, 0});
  auto bottom_right_m =
      pix2meters({(double)pixmap.width(), (double)pixmap.height()});
  auto frame_m = QRectF{top_left_m, bottom_right_m}.normalized();
  frame_m.adjust(-10000, -10000, 10000, 10000);

  QPolygonF rect;
  rect << frame_m.topLeft();
  rect << frame_m.topRight();
  rect << frame_m.bottomLeft();
  rect << frame_m.bottomRight();
  if (map->intersects(rect))
    return true;
  return false;
}

void FlashRender::checkLoad()
{
  auto draw_rect_m = getDrawRectM();
  for (auto& map: maps)
  {
    auto map_rect_m = map->frame.toMeters();

    if (!needToLoadMap(map, draw_rect_m))
      continue;

    if (map->main.status == FlashTile::Null)
      map->loadMain(true, s.pixel_size_mm);
    if (map->main.status == FlashTile::Loaded)
    {
      if (needToLoadMap(map, draw_rect_m))
      {
        int    tile_side_count = sqrt(map->tiles.count());
        QSizeF tile_size_m = {map_rect_m.width() / tile_side_count,
                              map_rect_m.height() / tile_side_count};
        for (int tile_idx = 0; auto& tile: map->tiles)
        {
          int    tile_idx_y = tile_idx / tile_side_count;
          int    tile_idx_x = tile_idx - tile_idx_y * tile_side_count;
          double tile_left =
              map_rect_m.x() + tile_idx_x * tile_size_m.width();
          double tile_top =
              map_rect_m.y() + tile_idx_y * tile_size_m.height();
          QRectF tile_rect_m = {{tile_left, tile_top}, tile_size_m};
          if (tile.status == FlashTile::Null &&
              tile_rect_m.intersects(draw_rect_m) &&
              mip < map->tile_mip)
            map->loadTile(tile_idx);
          tile_idx++;
        }
      }
    }
  }
}

void FlashRender::paintPointName(QPainter* p, const QString& text,
                                 const QColor& tcolor)
{
  QRect rect;
  int   w = 20.0 / s.pixel_size_mm;
  rect.setSize({w, w});

  p->setPen(Qt::white);
  auto shifts = {-2, 0, 2};
  int  flags  = Qt::AlignLeft | Qt::AlignTop | Qt::TextWordWrap |
              Qt::TextDontClip;
  for (auto x: shifts)
    for (auto y: shifts)
    {
      p->save();
      p->translate(x, y);
      p->drawText(rect, flags, text);
      p->restore();
    }

  p->setPen(tcolor);
  p->drawText(rect, flags, text);
}

void FlashRender::paintOutlinedText(QPainter* p, const QString& text,
                                    const QColor& tcolor)
{
  p->setPen(Qt::white);
  auto shifts = {-2, 0, 2};
  for (auto x: shifts)
    for (auto y: shifts)
      p->drawText(x, y, text);

  p->setPen(tcolor);
  p->drawText(0, 0, text);
}

void FlashRender::paintOutlinedText(QPainter*            p,
                                    const DrawTextEntry& dte)
{
  p->setPen(Qt::white);
  auto shifts = {-2, 0, 2};

  for (auto x: shifts)
    for (auto y: shifts)
    {
      p->save();
      p->translate(x, y);
      p->drawText(dte.rect,
                  dte.alignment | Qt::TextWordWrap | Qt::TextDontClip,
                  dte.text);
      p->restore();
    }

  p->setPen(dte.cl->tcolor);
  p->drawText(dte.rect,
              dte.alignment | Qt::TextWordWrap | Qt::TextDontClip,
              dte.text);
}

void FlashRender::addDrawTextEntry(
    QVector<DrawTextEntry>& draw_text_array, DrawTextEntry new_dte)
{
  bool can_fit = true;
  for (auto dte: draw_text_array)
  {
    if (dte.actual_rect.intersects(new_dte.actual_rect))
    {
      can_fit = false;
      break;
    }
  }
  if (can_fit)
    draw_text_array.append(new_dte);
};

FlashRender::TileCoor FlashRender::getBigTileCoor(TileCoor t)
{
  TileCoor bt;
  bt.x = int(t.x / s.big_tile_multiplier) * s.big_tile_multiplier;
  bt.y = int(t.y / s.big_tile_multiplier) * s.big_tile_multiplier;
  bt.z = t.z;
  return bt;
}

QString FlashRender::getTileName(TileCoor t)
{
  return "z=" + QString("%1").arg(t.z) +
         ",y=" + QString("%1").arg(t.y) +
         ",x=" + QString("%1").arg(t.x) + +".bmp";
}

QPoint FlashRender::deg2pix(FlashGeoCoor kp) const
{
  auto m = kp.toMeters();
  return {int((m.x() - top_left_m.x()) / mip),
          int((m.y() - top_left_m.y()) / mip)};
}

void FlashRender::paintPointObject(QPainter*             p,
                                   const FlashRenderMap& map,
                                   const FlashMapObject& obj,
                                   int                   render_idx)
{
  auto frame = obj.frame;

  auto coor_m = frame.top_left.toMeters();

  auto clip_safe_rect_m = render_frame_m.adjusted(
      -s.max_object_name_length_pix * mip, -50 * mip,
      s.max_object_name_length_pix * mip, 50 * mip);

  if (!clip_safe_rect_m.contains(coor_m))
    return;

  auto cl = &map.classes[obj.class_idx];
  p->setPen(QPen(cl->pen, 2));
  p->setBrush(cl->brush);
  auto        kpos       = obj.polygons.first().first();
  QPoint      pos        = deg2pix(kpos);
  int         max_length = 0;
  QStringList str_list;
  if (!obj.name.isEmpty())
  {
    str_list += obj.name;
    max_length = obj.name.count();
  }

  auto rect = QRect{pos.x(), pos.y(), max_length * cl->getWidthPix(),
                    str_list.count() * cl->getWidthPix()};

  bool intersects = false;
  for (auto item: point_names[render_idx])
  {
    if (rect.intersects(item.rect))
    {
      intersects = true;
      break;
    }
  }
  if (intersects)
    return;

  point_names[render_idx].append({rect, str_list, cl});
}

QPolygon FlashRender::poly2pix(const FlashGeoPolygon& polygon)
{
  QPoint   prev_point_pix = deg2pix(polygon.first());
  QPolygon pl;
  pl.append(prev_point_pix);
  for (int i = 1; i < polygon.count(); i++)
  {
    auto kpoint    = polygon.at(i);
    auto point_pix = deg2pix(kpoint);
    auto d         = point_pix - prev_point_pix;
    if (d.manhattanLength() > 2 || i == polygon.count() - 1)
    {
      pl.append(point_pix);
      prev_point_pix = point_pix;
    }
  }
  return pl;
}

void FlashRender::paintPolygonObject(QPainter*             p,
                                     const FlashRenderMap& map,
                                     const FlashMapObject& obj,
                                     int                   render_idx)
{
  auto  frame = obj.frame;
  QRect obj_frame_pix;

  auto   top_left_m     = frame.top_left.toMeters();
  auto   bottom_right_m = frame.bottom_right.toMeters();
  QRectF obj_frame_m    = {top_left_m, bottom_right_m};

  auto cl = &map.classes[obj.class_idx];

  auto clip_safe_rect_m = render_frame_m.adjusted(
      -s.max_object_name_length_pix * mip, -50 * mip,
      s.max_object_name_length_pix * mip, 50 * mip);

  if (!obj_frame_m.intersects(clip_safe_rect_m))
    return;

  double obj_span_m   = sqrt(pow(obj_frame_m.width(), 2) +
                             pow(obj_frame_m.height(), 2));
  int    obj_span_pix = obj_span_m / mip;

  if (cl->pen == Qt::black)
    p->setPen(Qt::NoPen);
  else
    p->setPen(cl->pen);
  if (cl->style == FlashClass::Hatch)
    p->setBrush(QBrush(cl->brush, Qt::HorPattern));
  else if (cl->style == FlashClass::BDiag)
    p->setBrush(QBrush(cl->brush, Qt::BDiagPattern));
  else if (cl->style == FlashClass::FDiag)
    p->setBrush(QBrush(cl->brush, Qt::FDiagPattern));
  else if (cl->style == FlashClass::Horiz)
    p->setBrush(QBrush(cl->brush, Qt::HorPattern));
  else if (cl->style == FlashClass::Vert)
    p->setBrush(QBrush(cl->brush, Qt::VerPattern));
  else
    p->setBrush(cl->brush);

  QPainterPath path;
  for (int polygon_idx = -1; auto polygon: obj.polygons)
  {
    polygon_idx++;

    auto pl = poly2pix(polygon);

    if ((polygon_idx == 0 && !obj.name.isEmpty() &&
         obj_span_pix <
             std::min(pixmap.width(), pixmap.height() / 2) &&
         obj_span_pix >
             s.max_object_size_with_name_mm / s.pixel_size_mm) ||
        !cl->image.isNull())
    {

      QPoint top_left_pix     = deg2pix(obj.frame.top_left);
      QPoint bottom_right_pix = deg2pix(obj.frame.bottom_right);
      obj_frame_pix           = {top_left_pix, bottom_right_pix};

      auto  c = obj_frame_pix.center();
      QRect actual_rect;
      int   w = pixmap.width() / 32;
      actual_rect.setTopLeft(c);
      actual_rect.setSize({w, w});
      actual_rect.translate({-w / 2, -w / 2});

      addDrawTextEntry(draw_text_array[render_idx],
                       {obj.name, cl, obj_frame_pix, actual_rect,
                        Qt::AlignCenter});
    }

    if (obj.polygons.count() == 1)
    {
      p->drawPolygon(pl);
      continue;
    }

    if (polygon_idx == 0)
      p->drawPolygon(pl);
    else
    {
      p->setBrush(s.land_color);
      p->drawPolygon(pl);
    }
  }
}

void FlashRender::paintLineObject(QPainter*             painter,
                                  const FlashRenderMap& map,
                                  const FlashMapObject& obj,
                                  int render_idx, int line_iter)
{
  auto frame = obj.frame;

  auto top_left_m     = frame.top_left.toMeters();
  auto bottom_right_m = frame.bottom_right.toMeters();

  QRectF obj_frame_m = {top_left_m, bottom_right_m};

  auto clip_safe_rect_m = render_frame_m.adjusted(
      -s.max_object_name_length_pix * mip, -50 * mip,
      s.max_object_name_length_pix * mip, 50 * mip);

  if (!obj_frame_m.intersects(clip_safe_rect_m))
    return;

  auto cl = &map.classes[obj.class_idx];

  Qt::PenStyle style = Qt::SolidLine;
  if (cl->style == FlashClass::Dash)
    style = Qt::DashLine;
  if (cl->style == FlashClass::Dots)
    style = Qt::DotLine;
  int obj_name_width = 0;
  if (!obj.name.isEmpty())
    obj_name_width =
        painter->font().pixelSize() * obj.name.count() * 0.3;

  auto         fixed_w    = cl->getWidthPix();
  int          sizeable_w = 0;
  int          w          = fixed_w;
  bool         one_way    = false;
  QMapIterator it(obj.attributes);
  while (it.hasNext())
  {
    it.next();
    if (it.key() == "oneway")
      one_way = true;

    if (it.key() == "lanes")
    {
      sizeable_w = 4 * it.value().toInt() / mip;
      w          = std::max((int)fixed_w, sizeable_w);
    }
  }

  painter->setPen(QPen(cl->pen, w, style));
  painter->setBrush(Qt::NoBrush);

  for (int poly_idx = -1; auto polygon: obj.polygons)
  {
    poly_idx++;
    NameHolder nh;
    QPoint     p0;
    double     a0 = 0;

    auto pl = poly2pix(polygon);

    auto size_m       = polygon.getFrame().getSizeMeters();
    auto size_pix     = (size_m.width() + size_m.height()) / mip;
    auto hatch_length = size_pix * 0.05;
    if (hatch_length > 5)
      if (cl->style == FlashClass::Hatch)
      {
        if (hatch_length > 5)
          hatch_length = 5;
        auto p0 = pl.first();
        for (int c = -1; auto p: pl)
        {
          c++;
          double a = 0;
          if (c == 0)
            a = getAngle(p0, pl.at(1));
          else
            a = getAngle(p0, p);
          painter->save();
          painter->translate(p);
          painter->rotate(rad2deg(a));
          int length = getDistance(p0, p);
          int step   = 10;
          for (int l = 0; l < length; l += step)
          {
            painter->drawLine(0, 0, 0, hatch_length);
            painter->translate(-step, 0);
          }
          painter->restore();
          p0 = p;
        }
      }

    if (!obj.name.isEmpty() && poly_idx == 0)
      for (int point_idx = -1; auto p: pl)
      {
        point_idx++;
        if (nh.point_count == 0)
        {
          p0 = p;
          nh.point_count++;
          continue;
        }
        auto a = getAngle(p0, p);

        if (nh.point_count == 1)
          a0 = a;
        auto da = a0 - a;
        if (da > M_PI)
          da -= 2 * M_PI;
        else if (da < -M_PI)
          da += 2 * M_PI;
        da = fabs(da);
        if (da > deg2rad(5) || point_idx == polygon.count() - 1)
        {
          if (nh.length_pix > obj_name_width)
          {
            nh.fix(&map, &obj, pl.at(nh.start_idx),
                   pl.at(nh.end_idx));
            name_holder_array[render_idx].append(nh);
          }
          nh           = NameHolder();
          nh.start_idx = point_idx;
          p0           = p;
          continue;
        }
        auto length_pix = getDistance(p0, p);
        nh.length_pix += length_pix;
        p0 = p;
        nh.point_count++;
        nh.end_idx = point_idx;
      }

    QPen pen;
    if (line_iter == 0)
    {
      double coef = 2;
      if (w > 20)
        coef = 1.5;
      if (w > 50)
        coef = 1.2;
      int w2 = w * coef;
      pen    = QPen(cl->brush, w2, Qt::SolidLine, Qt::FlatCap,
                    Qt::RoundJoin);
    }
    else
      pen = QPen(cl->pen, w, style, Qt::FlatCap, Qt::RoundJoin);
    painter->setPen(pen);
    painter->drawPolyline(pl);
    if (sizeable_w > 7 && w > 0)
    {
      painter->setPen(Qt::black);
      auto orig_f = painter->font();
      auto f      = orig_f;
      f.setPixelSize(w);
      painter->setFont(f);
      if (pl.count() > 1 && sizeable_w > 7 && one_way)
      {
        auto p0 = pl.first();
        for (int i = 1; i < pl.count(); i++)
        {
          auto   p1  = pl.at(i);
          auto   a   = getAngle(p0, p1);
          QPoint mid = {(p0.x() + p1.x()) / 2, (p0.y() + p1.y()) / 2};
          painter->save();
          painter->translate(mid);
          painter->rotate(rad2deg(a));
          painter->drawText(0, w * 0.3, "→");
          painter->restore();
          p0 = p1;
        }
      }
      painter->setFont(orig_f);
    }
  }
}

void FlashRender::NameHolder::fix(const FlashMap*       map,
                                  const FlashMapObject* _obj,
                                  const QPoint&         start,
                                  const QPoint&         end)
{
  obj       = _obj;
  mid_point = {(start.x() + end.x()) / 2, (start.y() + end.y()) / 2};
  angle_deg = rad2deg(getAngle(start, end));
  if (angle_deg > 90)
    angle_deg -= 180;
  if (angle_deg < -90)
    angle_deg += 180;
  tcolor = map->classes[_obj->class_idx].tcolor;
}

bool FlashRender::isCluttering(const QRect& rect)
{
  bool clutter_flag = false;
  for (auto ex_rect: text_rect_array)
    if (ex_rect.intersects(rect))
    {
      clutter_flag = true;
      break;
    }
  return clutter_flag;
}

bool FlashRender::checkMipRange(const FlashMap*       map,
                                const FlashMapObject* obj)
{
  auto cl = &map->classes[obj->class_idx];
  return (cl->min_mip == 0 || mip >= cl->min_mip) &&
         (cl->max_mip == 0 || mip <= cl->max_mip);
}

void FlashRender::paintObject(QPainter* p, const FlashRenderMap* map,
                              const FlashMapObject& obj,
                              int render_idx, int line_iter)
{
  auto cl = &map->classes[obj.class_idx];
  switch (cl->type)
  {
  case FlashClass::Point:
    paintPointObject(p, *map, obj, render_idx);
    break;
  case FlashClass::Line:
    paintLineObject(p, *map, obj, render_idx, line_iter);
    break;
  case FlashClass::Polygon:
    paintPolygonObject(p, *map, obj, render_idx);
    break;
  default:
    break;
  }
}

void FlashRender::paintPointNames(QPainter* p)
{
  for (int render_idx = 0; render_idx < FlashRenderMap::render_count;
       render_idx++)
    for (auto item: point_names[render_idx])
    {
      auto pos = item.rect.topLeft();
      auto w   = item.cl->getWidthPix();
      if (w > 0)
      {
        p->save();
        p->translate(pos);
        auto f = p->font();
        f.setPixelSize(w);
        p->setFont(f);
        for (auto str: item.str_list)
        {
          p->translate(
              QPoint(item.cl->image.width() * 0.8, -w * 0.3));
          paintPointName(p, str, item.cl->tcolor);
        }
        p->restore();
      }
      if (item.cl)
      {
        auto pos2 = QPoint{pos.x() - item.cl->image.width() / 2,
                           pos.y() - item.cl->image.height() / 2};
        p->drawImage(pos2, item.cl->image);
      }
      else
        p->drawEllipse(pos, int(1.0 / s.pixel_size_mm),
                       int(1.0 / s.pixel_size_mm));
    }
}

void FlashRender::paintLineNames(QPainter* p)
{
  text_rect_array.clear();
  auto f = p->font();
  auto w = round(1.5 / s.pixel_size_mm);
  f.setPixelSize(w);
  p->setFont(f);

  for (int render_idx = 0; render_idx < FlashRenderMap::render_count;
       render_idx++)
    for (auto nh: name_holder_array[render_idx])
    {
      p->save();
      QRect text_rect;
      text_rect.setSize(
          {int(p->font().pixelSize() * nh.obj->name.count() * 0.6),
           p->font().pixelSize()});

      QTransform tr;
      tr.translate(nh.mid_point.x(), nh.mid_point.y());
      tr.rotate(nh.angle_deg);
      tr.translate(-text_rect.width() / 2, 0);

      QRect mapped_rect = tr.mapRect(text_rect);
      if (isCluttering(mapped_rect))
      {
        p->restore();
        continue;
      }

      p->setTransform(tr);
      text_rect_array.append(mapped_rect);
      paintOutlinedText(p, nh.obj->name, nh.tcolor);
      p->restore();
    }
}

void FlashRender::paintPolygonNames(QPainter* p)
{
  for (int render_idx = 0; render_idx < FlashRenderMap::render_count;
       render_idx++)
    for (auto& dte: draw_text_array[render_idx])
    {
      QFontMetrics fm(p->font());
      auto         actual_rect = fm.boundingRect(
                  dte.actual_rect,
                  Qt::AlignLeft | Qt::TextWordWrap | Qt::TextDontClip,
                  dte.text);
      if (actual_rect.width() > dte.actual_rect.width() * 1.5 &&
          actual_rect.height() > dte.actual_rect.height() * 1.5)
        continue;

      if (isCluttering(actual_rect))
        continue;
      paintOutlinedText(p, dte);

      if (!dte.cl->image.isNull() &&
          dte.rect.width() > dte.cl->image.width() * 2 &&
          dte.rect.height() > dte.cl->image.height() * 2)
      {
        auto obj_center = dte.rect.center();
        auto pos =
            QPoint{obj_center.x() - dte.cl->image.width() / 2,
                   obj_center.y() - dte.cl->image.height() / 2};
        if (!dte.text.isEmpty())
          pos -= QPoint(0, dte.cl->getWidthPix() + 5);
        p->drawImage(pos, dte.cl->image);
      }

      text_rect_array.append(dte.rect);
    }
}

void FlashRender::render(QPainter*                p,
                         QVector<FlashRenderMap*> render_maps,
                         int                      render_idx)
{
  for (auto map: render_maps)
  {
    FlashLocker main_locker(&map->main_lock, FlashLocker::Read);
    if (!main_locker.hasLocked())
      continue;
    FlashLocker tile_locker(&map->tile_lock, FlashLocker::Read);
    if (!tile_locker.hasLocked())
      continue;

    for (int line_iter = 0; line_iter < 2; line_iter++)
      renderMap(p, map, render_idx, line_iter);
  }
}

void FlashRender::renderMap(QPainter* p, const FlashRenderMap* map,
                            int render_idx, int line_iter)
{
  if (!map || render_idx > map->render_start_list.count() - 1)
    return;

  auto start        = map->render_start_list[render_idx];
  int  object_count = 0;

  p->setRenderHint(QPainter::Antialiasing);
  for (int layer_idx = start.layer_idx;
       layer_idx < FlashRenderMap::max_layer_count; layer_idx++)
  {
    int start_obj_idx = 0;
    if (layer_idx == start.layer_idx)
      start_obj_idx = start.obj_idx;
    auto& layer     = map->render_data[layer_idx];
    auto  obj_count = layer.count();

    for (int obj_idx = start_obj_idx; obj_idx < obj_count; obj_idx++)
    {
      auto obj = layer[obj_idx];
      object_count++;
      if (object_count == map->render_object_count)
        return;
      if (!obj)
        continue;

      auto cl = &map->classes[obj->class_idx];

      if (cl->type != FlashClass::Line && line_iter == 1)
        continue;

      if (cl->type == FlashClass::Line && line_iter == 0 &&
          cl->brush == Qt::black)
        continue;

      if (!checkMipRange(map, obj))
        continue;

      paintObject(p, map, *obj, render_idx, line_iter);
    }
  }
}

struct RenderEntry
{
  int            idx;
  QPixmap*       pm;
  QPainter*      p;
  QElapsedTimer* t;
  QFuture<void>* fut;
  RenderEntry(int idx, QSize s, QFont* f);
  ~RenderEntry();
};

RenderEntry::RenderEntry(int _idx, QSize size, QFont* f)
{
  idx = _idx;
  pm  = new QPixmap(size);
  pm->fill(Qt::transparent);
  p = new QPainter(pm);
  p->setFont(*f);
  t = new QElapsedTimer;
  t->start();
  fut = new QFuture<void>;
}

RenderEntry::~RenderEntry()
{
  delete p;
  delete pm;
  delete t;
  delete fut;
}

void FlashRender::onFinished()
{
  wait();
  big_tiles.append(big_tile);
  while (big_tiles.count() > 4 * sqr(s.big_tile_multiplier))
    big_tiles.removeFirst();
}

void FlashRender::run()
{
  int tile_count      = pow(2, tile_coor.z);
  int world_width_pix = tile_side * tile_count;
  mip = 2 * M_PI * flashmath::earth_r / world_width_pix;
  auto big_tile_coor = getBigTileCoor(tile_coor);
  top_left_m = {(big_tile_coor.x - tile_count / 2) * tile_side * mip,
                (big_tile_coor.y - tile_count / 2) * tile_side * mip};

  for (int i = 0; i < FlashRenderMap::render_count; i++)
  {
    point_names[i].clear();
    draw_text_array[i].clear();
    name_holder_array[i].clear();
  }
  size_m         = {pixmap.width() * mip, pixmap.height() * mip};
  render_frame_m = {top_left_m, size_m};

  checkLoad();

  QElapsedTimer total_render_time;
  total_render_time.start();

  pixmap.fill(s.ocean_color);
  QPainter p0(&pixmap);
  QFont    f = p0.font();

  double font_size =
      std::min((int)std::round(1.5 / s.pixel_size_mm / mip), 1);
  font_size = std::clamp(font_size, 1.5 / s.pixel_size_mm,
                         3.0 / s.pixel_size_mm);
  f.setPixelSize(font_size);
  f.setBold(true);
  p0.setFont(f);

  QVector<int> intersecting_maps;
  auto         draw_rect = getDrawRectM();
  for (int map_idx = -1; auto& map: maps)
  {
    if (map->main_mip > 0 && mip > map->main_mip)
      continue;
    map_idx++;
    if (map_idx == 0)
      continue;

    if (needToLoadMap(map, draw_rect))
      intersecting_maps.append(map_idx);
  }

  QVector<FlashRenderMap*> render_maps;
  for (int map_idx = -1; auto& map: maps)
  {
    map_idx++;

    if (map_idx > 0)
      if (!intersecting_maps.contains(map_idx))
        continue;

    FlashLocker big_locker(&map->main_lock, FlashLocker::Read);
    if (!big_locker.hasLocked())
      continue;

    if (map_idx > 0 && !needToLoadMap(map, render_frame_m))
      continue;

    auto map_rect_m = map->frame.toMeters();
    if (map_idx > 0 && !render_frame_m.intersects(map_rect_m))
      continue;

    FlashLocker small_locker(&map->tile_lock, FlashLocker::Read);
    if (!small_locker.hasLocked())
      continue;

    if (map->render_start_list.isEmpty())
      continue;

    render_maps.append(map);
  }

  QList<RenderEntry*> render_list;
  for (int render_idx = 1; render_idx < FlashRenderMap::render_count;
       render_idx++)
  {
    auto render = new RenderEntry(render_idx, pixmap.size(), &f);
    *render->fut =
        QtConcurrent::run(this, &FlashRender::render, render->p,
                          render_maps, render_idx);
    render_list.append(render);
  }

  render(&p0, render_maps, 0);

  for (auto render: render_list)
    render->fut->waitForFinished();

  for (auto render: render_list)
    p0.drawPixmap(0, 0, *render->pm);

  qDeleteAll(render_list);

  paintLineNames(&p0);
  paintPolygonNames(&p0);
  paintPointNames(&p0);

  big_tile.clear();
  for (int yi = 0; yi < s.big_tile_multiplier; yi++)
    for (int xi = 0; xi < s.big_tile_multiplier; xi++)
    {
      TileCoor t;
      t.x     = big_tile_coor.x + xi;
      t.y     = big_tile_coor.y + yi;
      t.z     = big_tile_coor.z;
      auto pm = pixmap.copy(xi * tile_side, yi * tile_side, tile_side,
                            tile_side);
      QByteArray ba;
      QBuffer    buf(&ba);
      pm.save(&buf, "bmp");

      big_tile.append({getTileName(t), ba});
    }
  qDebug() << "render time=" << total_render_time.elapsed();
}

void FlashRender::render()
{
  if (isRunning())
    return;
  if (QThreadPool::globalInstance()->activeThreadCount() == 0)
    checkUnload();
  QThread::start();
}
