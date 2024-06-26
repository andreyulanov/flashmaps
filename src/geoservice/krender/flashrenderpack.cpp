#include "flashrenderpack.h"
#include "flashlocker.h"
#include <QDebug>

FlashRenderPack::FlashRenderPack(const QString& _path)
{
  path = _path;
}

bool FlashRenderPack::intersects(QPolygonF polygon_m) const
{
  if (borders_m.isEmpty())
    return polygon_m.intersects(frame.toRectM());
  for (auto border_m: borders_m)
    if (border_m.intersects(polygon_m))
      return true;
  return false;
}

void FlashRenderPack::clear()
{
  FlashLocker big_locker(&main_lock, FlashLocker::Write);
  if (!big_locker.hasLocked())
    return;
  FlashLocker small_locker(&tile_lock, FlashLocker::Write);
  if (!small_locker.hasLocked())
    return;
  FlashPack::clear();
  for (int i = 0; i < max_layer_count; i++)
    render_data[i].clear();
  render_object_count = 0;
  render_start_list.clear();
}

void FlashRenderPack::loadMain(bool load_objects, double pixel_size_mm)
{
  FlashPack::loadMain(path, load_objects, pixel_size_mm);
  if (load_objects)
  {
    QWriteLocker big_locker(&main_lock);
    addCollectionToIndex(main);
    main.status = FlashTile::Loaded;
  }
}

void FlashRenderPack::loadTile(int tile_idx)
{
  FlashPack::loadTile(path, tile_idx);
  QWriteLocker small_locker(&tile_lock);
  addCollectionToIndex(tiles[tile_idx]);
  tiles[tile_idx].status = FlashTile::Loaded;
}

void FlashRenderPack::addCollectionToIndex(FlashTile& collection)
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

KRenderPackCollection::~KRenderPackCollection()
{
  qDeleteAll(*this);
}
