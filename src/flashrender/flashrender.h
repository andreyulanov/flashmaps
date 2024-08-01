#pragma once

#include "flashrendermap.h"
#include <QReadWriteLock>
#include <QThread>
#include <QSet>
#include <QMutex>
#include <QGuiApplication>

class FlashRender: public QThread
{
  Q_OBJECT

  static constexpr int  tile_side    = 256;
  static constexpr char class_name[] = "FlashRender";

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

    void addCollectionToIndex(FlashTile& collection);

  public:
    Map(const QString& path);
    void clear();
    void loadMain(bool load_objects, double pixel_size_mm);
    void loadTile(int tile_idx);
    bool intersects(QPolygonF polygon) const;
  };

  struct MapList: public QVector<Map*>
  {
    virtual ~MapList();
  };

  struct Settings
  {
    int     big_tile_multiplier          = 4;
    double  max_object_size_with_name_mm = 20.0;
    double  pixel_size_mm                = 0.1;
    QColor  ocean_color                  = QColor(150, 210, 240);
    QColor  land_color                   = QColor(250, 246, 230);
    double  max_loaded_maps_count        = 3;
    double  max_name_width_mm            = 20.0;
    QString map_dir;
    QString world_map_name = "world.flashmap";
  };
  struct TileCoor
  {
    int x, y, z;
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
  TileCoor tile_coor;

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

  void insertMap(int idx, QString path, bool load_now);
  void renderMap(QPainter* p, const Map* map, int render_idx,
                 int line_iter);
  void render(QPainter* p, QVector<Map*> render_maps, int render_idx);

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

  QPolygon poly2pix(const FlashGeoPolygon& polygon);
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
  QPoint   deg2pix(FlashGeoCoor) const;
  TileCoor getBigTileCoor(TileCoor);
  QString  getTileName(TileCoor);

public:
  FlashRender();
  virtual ~FlashRender();
  void                setSettings(Settings);
  void                requestTile(TileCoor);
  QByteArray          getTile(TileCoor);
  static FlashRender* instance();
};
