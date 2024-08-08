#include "flashrender.h"
#include "flashlocker.h"
#include <QDir>
#include <QtConcurrent/QtConcurrent>
#include <QPainterPath>
#include <numeric>

using namespace flashmath;

FlashRender::Map::Map(const QString& _path)
{
  path = _path;
}

bool FlashRender::Map::intersectsWrapped(const QPolygonF& p1,
                                         const QPolygonF& p2) const
{
  constexpr double earth_l = flashmath::earth_r * 2 * M_PI;
  if (need_to_wrap)
  {
    if (p1.intersects(p2))
      return true;
    auto wrapped_p2 = p2.translated({earth_l, 0});
    if (p1.intersects(wrapped_p2))
      return true;
    auto wrapped_p1 = p1.translated({earth_l, 0});
    if (wrapped_p1.intersects(p2))
      return true;
    return false;
  }
  else
    return p1.intersects(p2);
}

bool FlashRender::Map::intersects(const QPolygonF& polygon_m) const
{
  if (borders_m.isEmpty())
    return intersectsWrapped(polygon_m, frame.toRectM());
  for (auto border_m: borders_m)
    if (intersectsWrapped(border_m, polygon_m))
      return true;
  return false;
}

void FlashRender::Map::clear()
{
  FlashLocker big_locker(&main_lock, FlashLocker::Write);
  if (!big_locker.hasLocked())
    return;
  FlashLocker small_locker(&tile_lock, FlashLocker::Write);
  if (!small_locker.hasLocked())
    return;
  FlashMap::clear();
  for (int i = 0; i < max_layer_count; i++)
    render_data[i].clear();
  render_object_count = 0;
  render_start_list.clear();
}

void FlashRender::Map::loadMainVectorTile(bool   load_objects,
                                          double pixel_size_mm)
{
  FlashMap::loadMainVectorTile(path, load_objects, pixel_size_mm);
  auto lon1 = frame.top_left.longitude();
  auto lon2 = frame.bottom_right.longitude();
  if (lon1 < 180 && lon2 > 180 && fabs(lon1 - lon2) < 180)
    need_to_wrap = true;
  if (load_objects)
  {
    QWriteLocker big_locker(&main_lock);
    addCollectionToIndex(main);
    main.status = FlashVectorTile::Loaded;
  }
}

void FlashRender::Map::loadVectorTile(int tile_idx)
{
  FlashMap::loadVectorTile(path, tile_idx);
  QWriteLocker small_locker(&tile_lock);
  addCollectionToIndex(tiles[tile_idx]);
  tiles[tile_idx].status = FlashVectorTile::Loaded;
}

void FlashRender::Map::addCollectionToIndex(
    FlashVectorTile& collection)
{
  for (auto& obj: collection)
  {
    auto cl = classes[obj.class_idx];
    render_data[cl.layer].append(&obj);
  }

  int total_object_count = 0;
  for (auto layer: render_data)
    total_object_count += layer.count();

  render_object_count = total_object_count / render_count;

  int curr_obj_count = 0;
  render_start_list.clear();
  render_start_list += {0, 0};
  for (int layer_idx = -1; auto layer: render_data)
  {
    layer_idx++;
    for (int object_idx = -1; auto obj: layer)
    {
      object_idx++;
      Q_UNUSED(obj)
      curr_obj_count++;
      if (curr_obj_count >= render_object_count)
      {
        render_start_list += {layer_idx, object_idx};
        curr_obj_count = 0;
      }
    }
  }
}

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

FlashRender::FlashRender(Settings _s)
{
  s                 = _s;
  int big_tile_side = tile_side * s.big_tile_multiplier;
  pixmap            = QPixmap{big_tile_side, big_tile_side};
  connect(this, &QThread::finished, this, &FlashRender::onFinished);
}

FlashRender::~FlashRender()
{
  wait();
}

void FlashRender::requestTile(TileSpec t)
{
  if (isRunning())
    return;
  tile_coor = t;
  render();
}

QByteArray FlashRender::getTile(TileSpec t)
{
  auto tile_name = getTileName(t);
  for (auto big_tile: big_tiles)
  {
    if (big_tile.name == tile_name)
      return big_tile.data;
  }
  return QByteArray();
}

void FlashRender::addMap(Map* map, int idx)
{
  QThreadPool().globalInstance()->waitForDone();
  wait();
  if (idx < 0)
    idx = maps.count();
  maps.insert(idx, map);
}

int FlashRender::getMapCount()
{
  return maps.count();
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
    if (map->main.status == FlashVectorTile::Loaded)
    {
      if (!needToLoadMap(map, draw_rect_m))
        if (loaded_count > s.max_loaded_maps_count)
          map->clear();
      loaded_count++;
    }
  }
}

bool FlashRender::needToLoadMap(const Map*    map,
                                const QRectF& draw_rect_m)
{
  if (map->main_mip > 0 && mip > map->main_mip)
    return false;
  auto map_rect_m = map->frame.toMeters();
  bool frame_intersects =
      map->intersectsWrapped(draw_rect_m, map_rect_m);
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
  for (int i = -1; auto& map: maps)
  {
    i++;
    if (i == 0)
      continue;
    auto map_rect_m = map->frame.toMeters();

    if (!needToLoadMap(map, draw_rect_m))
      continue;

    if (map->main.status == FlashVectorTile::Null)
      map->loadMainVectorTile(true, s.pixel_size_mm);
    if (map->main.status == FlashVectorTile::Loaded)
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
          if (tile.status == FlashVectorTile::Null &&
              map->intersectsWrapped(tile_rect_m, draw_rect_m) &&
              mip < map->tile_mip)
            map->loadVectorTile(tile_idx);
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
  int   w = s.max_name_width_mm / s.pixel_size_mm;
  rect.setSize({w, w});

  if (!isCluttering(rect))
    text_rect_array.append(rect);

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

FlashRender::TileSpec FlashRender::getBigTileCoor(TileSpec t)
{
  TileSpec bt;
  bt.x = int(t.x / s.big_tile_multiplier) * s.big_tile_multiplier;
  bt.y = int(t.y / s.big_tile_multiplier) * s.big_tile_multiplier;
  bt.z = t.z;
  return bt;
}

QString FlashRender::getTileName(TileSpec t)
{
  return "z=" + QString("%1").arg(t.z) +
         ",y=" + QString("%1").arg(t.y) +
         ",x=" + QString("%1").arg(t.x) + +".bmp";
}

QPoint FlashRender::deg2pix(FlashGeoCoor kp, bool need_to_wrap) const
{
  auto   m = kp.toMeters();
  QPoint p = {int((m.x() - top_left_m.x()) / mip),
              int((m.y() - top_left_m.y()) / mip)};
  if (need_to_wrap)
  {
    int wrap_shift =
        static_cast<int>(round(flashmath::earth_r * 2 * M_PI / mip));
    if (abs(p.x()) > wrap_shift * 0.5)
    {
      if (p.x() < 0)
        p += {wrap_shift, 0};
      else
        p -= {wrap_shift, 0};
    }
  }
  return p;
}

void FlashRender::paintPointObject(QPainter* p, const Map& map,
                                   const FlashObject& obj,
                                   int                   render_idx)
{
  auto frame = obj.frame;

  auto coor_m = frame.top_left.toMeters();

  int max_object_name_length_pix =
      s.max_name_width_mm / s.pixel_size_mm;
  auto clip_safe_rect_m =
      render_frame_m.adjusted(-max_object_name_length_pix * mip,
                              -max_object_name_length_pix * mip,
                              max_object_name_length_pix * mip,
                              max_object_name_length_pix * mip);

  if (!clip_safe_rect_m.contains(coor_m))
    return;

  auto cl = &map.classes[obj.class_idx];
  p->setPen(QPen(cl->pen, 2));
  p->setBrush(cl->brush);
  auto    kpos = obj.polygons.first().first();
  QPoint  pos  = deg2pix(kpos, map.need_to_wrap);
  QString str;
  if (!obj.name.isEmpty())
    str += obj.name;

  int w = obj.name.count() * p->font().pixelSize();
  if (obj.name.isEmpty())
    w = cl->getWidthPix();

  auto name_rect = QRect{pos.x(), pos.y(), w, cl->getWidthPix()};

  bool intersects = false;
  for (auto item: point_names[render_idx])
  {
    if (name_rect.intersects(item.rect))
    {
      intersects = true;
      break;
    }
  }
  if (intersects)
    return;

  point_names[render_idx].append({name_rect, str, cl});
}

QPolygon FlashRender::poly2pix(const FlashGeoPolygon& polygon,
                               bool                   need_to_wrap)
{
  QPoint   prev_point_pix = deg2pix(polygon.first(), need_to_wrap);
  QPolygon pl;
  pl.append(prev_point_pix);
  for (int i = 1; i < polygon.count(); i++)
  {
    auto kpoint    = polygon.at(i);
    auto point_pix = deg2pix(kpoint, need_to_wrap);
    auto d         = point_pix - prev_point_pix;
    if (d.manhattanLength() > 2 || i == polygon.count() - 1)
    {
      pl.append(point_pix);
      prev_point_pix = point_pix;
    }
  }
  return pl;
}

void FlashRender::paintPolygonObject(QPainter* p, const Map& map,
                                     const FlashObject& obj,
                                     int                   render_idx)
{
  auto  frame = obj.frame;
  QRect obj_frame_pix;

  auto   top_left_m     = frame.top_left.toMeters();
  auto   bottom_right_m = frame.bottom_right.toMeters();
  QRectF obj_frame_m    = {top_left_m, bottom_right_m};

  auto cl = &map.classes[obj.class_idx];

  int max_object_name_length_pix =
      s.max_name_width_mm / s.pixel_size_mm;
  auto clip_safe_rect_m =
      render_frame_m.adjusted(-max_object_name_length_pix * mip,
                              -max_object_name_length_pix * mip,
                              max_object_name_length_pix * mip,
                              max_object_name_length_pix * mip);

  if (!map.intersectsWrapped(obj_frame_m, clip_safe_rect_m))
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

    auto pl = poly2pix(polygon, map.need_to_wrap);

    if ((polygon_idx == 0 && !obj.name.isEmpty() &&
         obj_span_pix <
             std::min(pixmap.width(), pixmap.height() / 2) &&
         obj_span_pix >
             s.max_object_size_with_name_mm / s.pixel_size_mm) ||
        !cl->image.isNull())
    {

      QPoint top_left_pix =
          deg2pix(obj.frame.top_left, map.need_to_wrap);
      QPoint bottom_right_pix =
          deg2pix(obj.frame.bottom_right, map.need_to_wrap);
      obj_frame_pix = {top_left_pix, bottom_right_pix};

      auto  c = obj_frame_pix.center();
      QRect actual_rect;
      int   w = pixmap.width() / 32;
      actual_rect.setTopLeft(c);
      actual_rect.setSize({w, w});
      actual_rect.translate({-w / 2, -w / 2});

      addDrawTextEntry(polygon_names[render_idx],
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

void FlashRender::paintLineObject(QPainter* painter, const Map& map,
                                  const FlashObject& obj,
                                  int render_idx, int line_iter)
{
  auto frame = obj.frame;

  auto top_left_m     = frame.top_left.toMeters();
  auto bottom_right_m = frame.bottom_right.toMeters();

  QRectF obj_frame_m = {top_left_m, bottom_right_m};

  int max_object_name_length_pix =
      s.max_name_width_mm / s.pixel_size_mm;
  auto clip_safe_rect_m =
      render_frame_m.adjusted(-max_object_name_length_pix * mip,
                              -max_object_name_length_pix * mip,
                              max_object_name_length_pix * mip,
                              max_object_name_length_pix * mip);

  if (!map.intersectsWrapped(obj_frame_m, clip_safe_rect_m))
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

    auto pl           = poly2pix(polygon, map.need_to_wrap);
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
            line_names[render_idx].append(nh);
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
                                  const FlashObject* _obj,
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
                                const FlashObject* obj)
{
  auto cl = &map->classes[obj->class_idx];
  return (cl->min_mip == 0 || mip >= cl->min_mip) &&
         (cl->max_mip == 0 || mip <= cl->max_mip);
}

void FlashRender::paintObject(QPainter* p, const Map* map,
                              const FlashObject& obj,
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
  for (int render_idx = 0;
       render_idx < FlashRender::Map::render_count; render_idx++)
    for (auto item: point_names[render_idx])
    {
      if (!item.cl)
        continue;

      auto pos = item.rect.topLeft();
      auto w   = item.cl->getWidthPix();

      if (w > 0)
      {
        p->save();
        p->translate(pos);
        auto f = p->font();
        f.setPixelSize(w);
        p->setFont(f);
        if (item.cl->image.isNull())
          p->translate(QPoint(w * 0.5, 0));
        else
          p->translate(QPoint(item.cl->image.width() * 0.5, 0));
        paintPointName(p, item.str, item.cl->tcolor);
        p->restore();
      }
      if (item.cl->image.isNull())
      {
        p->setPen(item.cl->pen);
        p->setBrush(item.cl->brush);
        p->drawEllipse(pos, int(w * 0.25), int(w * 0.25));
      }
      else
      {
        auto pos2 = QPoint{pos.x() - item.cl->image.width() / 2,
                           pos.y() - item.cl->image.height() / 2};
        p->drawImage(pos2, item.cl->image);
      }
    }
}

void FlashRender::paintLineNames(QPainter* p)
{
  QRect screen_rect = {0, 0, pixmap.width(), pixmap.height()};
  text_rect_array.clear();
  auto f = p->font();
  auto w = round(1.3 / s.pixel_size_mm);
  f.setPixelSize(w);
  p->setFont(f);

  for (int render_idx = 0;
       render_idx < FlashRender::Map::render_count; render_idx++)
    for (auto nh: line_names[render_idx])
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
      if (!screen_rect.contains(mapped_rect))
      {
        p->restore();
        continue;
      }

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
  for (int render_idx = 0;
       render_idx < FlashRender::Map::render_count; render_idx++)
    for (auto& dte: polygon_names[render_idx])
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

void FlashRender::renderLayer(QPainter* p, QVector<Map*> render_maps,
                              int render_idx)
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

void FlashRender::renderMap(QPainter* p, const Map* map,
                            int render_idx, int line_iter)
{
  if (!map || render_idx > map->render_start_list.count() - 1)
    return;

  auto start        = map->render_start_list[render_idx];
  int  object_count = 0;

  p->setRenderHint(QPainter::Antialiasing);
  for (int layer_idx = start.layer_idx;
       layer_idx < FlashRender::Map::max_layer_count; layer_idx++)
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

  for (int i = 0; i < FlashRender::Map::render_count; i++)
  {
    point_names[i].clear();
    polygon_names[i].clear();
    line_names[i].clear();
  }
  QSizeF size_m  = {pixmap.width() * mip, pixmap.height() * mip};
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

  QVector<FlashRender::Map*> render_maps;
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
    if (map_idx > 0 &&
        !map->intersectsWrapped(render_frame_m, map_rect_m))
      continue;

    FlashLocker small_locker(&map->tile_lock, FlashLocker::Read);
    if (!small_locker.hasLocked())
      continue;

    if (map->render_start_list.isEmpty())
      continue;

    render_maps.append(map);
  }

  QList<RenderEntry*> render_list;
  for (int render_idx = 1;
       render_idx < FlashRender::Map::render_count; render_idx++)
  {
    auto render = new RenderEntry(render_idx, pixmap.size(), &f);
    *render->fut =
        QtConcurrent::run(this, &FlashRender::renderLayer, render->p,
                          render_maps, render_idx);
    render_list.append(render);
  }

  renderLayer(&p0, render_maps, 0);

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
      TileSpec t;
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
  startedRender(getDrawRectM(), mip);
  QThread::start();
}
