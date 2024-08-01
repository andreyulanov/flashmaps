#pragma once

#include "flashmap.h"

class FlashRenderMap: public FlashMap
{
  struct RenderAddress
  {
    int layer_idx;
    int obj_idx;
  };

public:
  static constexpr int max_layer_count = 24;
  static constexpr int render_count    = 4;

  QVector<FlashMapObject*> render_data[max_layer_count];
  QReadWriteLock           main_lock;
  QReadWriteLock           tile_lock;
  QList<RenderAddress>     render_start_list;
  int                      render_object_count;
  QString                  path;

  void addCollectionToIndex(FlashTile& collection);

public:
  FlashRenderMap(const QString& path);
  void clear();
  void loadMain(bool load_objects, double pixel_size_mm);
  void loadTile(int tile_idx);
  bool intersects(QPolygonF polygon) const;
};

struct FlashRenderMapList: public QVector<FlashRenderMap*>
{
  virtual ~FlashRenderMapList();
};
