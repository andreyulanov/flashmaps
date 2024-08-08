#pragma once

#include "flashmap.h"
#include <QReadWriteLock>
#include <QThread>
#include <QSet>
#include <QMutex>
#include <QGuiApplication>

class FlashRender: public QThread
{
public:
  class Map: public FlashMap
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
    bool                     need_to_wrap = false;

    void addCollectionToIndex(FlashVectorTile& collection);

  public:
    Map(const QString& path);
    void clear();
    void loadMainVectorTile(bool load_objects, double pixel_size_mm);
    void loadVectorTile(int tile_idx);
    bool intersects(const QPolygonF& polygon) const;
    bool intersectsWrapped(const QPolygonF& p1,
                           const QPolygonF& p2) const;
  };

  struct Settings
  {
    int    big_tile_multiplier          = 4;
    double max_object_size_with_name_mm = 20.0;
    double pixel_size_mm                = 0.1;
    QColor ocean_color                  = QColor(150, 210, 240);
    QColor land_color                   = QColor(250, 246, 230);
    double max_loaded_maps_count        = 3;
    double max_name_width_mm            = 20.0;
  };
  struct TileSpec
  {
    int x, y, z;
  };

  FlashRender(Settings);
  virtual ~FlashRender();
  void       addMap(Map* map, int idx = -1);
  int        getMapCount();
  void       requestTile(TileSpec);
  QByteArray getTile(TileSpec);

  Q_OBJECT

private:
  static constexpr int tile_side = 256;

  struct MapList: public QVector<Map*>
  {
    virtual ~MapList();
  };

  struct RenderResult
  {
    QString    name;
    QByteArray data;
  };

private:
  struct DrawTextEntry
  {
    QString           text;
    const FlashClass* cl;
    QRect             rect;
    QRect             actual_rect;
    Qt::Alignment     alignment;
  };

  struct NameHolder
  {
    int                   length_pix  = 0;
    int                   start_idx   = 0;
    int                   end_idx     = 0;
    int                   point_count = 0;
    double                angle_deg   = 0;
    QPoint                mid_point;
    const FlashMapObject* obj = nullptr;
    QColor                tcolor;
    void fix(const FlashMap* map, const FlashMapObject* obj,
             const QPoint& start, const QPoint& end);
  };

  struct PointNameRect
  {
    QRect             rect;
    QString           str;
    const FlashClass* cl;
  };

  Settings s;
  TileSpec tile_coor;

  QPointF               top_left_m;
  double                mip;
  QPixmap               pixmap;
  QVector<RenderResult> big_tile;
  QVector<RenderResult> big_tiles;

  MapList maps;

  QVector<PointNameRect> point_names[Map::render_count];
  QVector<DrawTextEntry> polygon_names[Map::render_count];
  QVector<NameHolder>    line_names[Map::render_count];
  QVector<QRect>         text_rect_array;
  QRectF                 render_frame_m;

  static void paintOutlinedText(QPainter* p, const QString& text,
                                const QColor& tcolor);

  void run();
  void start() = delete;
  void onFinished();

  void renderMap(QPainter* p, const Map* map, int render_idx,
                 int line_iter);
  void renderLayer(QPainter* p, QVector<Map*> render_maps,
                   int render_idx);

  bool checkMipRange(const FlashMap* map, const FlashMapObject* obj);

  void paintObject(QPainter* p, const Map* map,
                   const FlashMapObject& obj, int render_idx,
                   int line_iter);
  void paintPointNames(QPainter* p);
  void paintLineNames(QPainter* p);
  void paintPolygonNames(QPainter* p);

  bool isCluttering(const QRect&);
  void paintOutlinedText(QPainter* p, const DrawTextEntry& dte);
  void addDrawTextEntry(QVector<DrawTextEntry>& draw_text_array,
                        DrawTextEntry           new_dte);

  QPolygon poly2pix(const FlashGeoPolygon& polygon,
                    bool                   need_to_wrap);
  void     paintPointName(QPainter* p, const QString& text,
                          const QColor& tcolor);
  void     paintPointObject(QPainter* p, const Map& map,
                            const FlashMapObject& obj, int render_idx);
  void     paintPolygonObject(QPainter* p, const Map& map,
                              const FlashMapObject& obj, int render_idx);
  void     paintLineObject(QPainter* painter, const Map& map,
                           const FlashMapObject& obj, int render_idx,
                           int line_iter);
  QRectF   getDrawRectM() const;
  bool     needToLoadMap(const Map* map, const QRectF& draw_rect);
  void     checkLoad();
  void     checkUnload();
  void     render();
  QPoint   meters2pix(QPointF m) const;
  QPointF  pix2meters(QPointF pix) const;
  QPoint   deg2pix(FlashGeoCoor, bool need_to_wrap) const;
  TileSpec getBigTileCoor(TileSpec);
  QString  getTileName(TileSpec);

signals:
  void startedRender(QRectF, double);
};
