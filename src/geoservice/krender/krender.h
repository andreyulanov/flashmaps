#pragma once

#include "krenderpack.h"
#include <QReadWriteLock>
#include <QThread>
#include <QSet>
#include <QMutex>

class KRender: public QThread
{
  friend class KRenderThread;

  static constexpr int tile_side = 256;

public:
  struct Settings
  {
    int     big_tile_multiplier          = 4;
    int     max_object_name_length_pix   = 200;
    double  max_object_size_with_name_mm = 20.0;
    double  pixel_size_mm                = 0.1;
    QColor  ocean_color                  = QColor(150, 210, 240);
    QColor  land_color                   = QColor(250, 246, 230);
    double  max_loaded_maps_count        = 3;
    QString map_dir;
    QString world_map_name = "world.kpack";
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
    QString       text;
    const KClass* cl;
    QRect         rect;
    QRect         actual_rect;
    Qt::Alignment alignment;
  };

  struct NameHolder
  {
    int            length_pix  = 0;
    int            start_idx   = 0;
    int            end_idx     = 0;
    int            point_count = 0;
    double         angle_deg   = 0;
    QPoint         mid_point;
    const KObject* obj = nullptr;
    QColor         tcolor;
    void           fix(const KPack* pack, const KObject* obj,
                       const QPoint& start, const QPoint& end);
  };

  struct PointName
  {
    QRect         rect;
    QStringList   str_list;
    const KClass* cl;
  };

  Q_OBJECT

  Settings s;
  TileCoor tile_coor;

  QPointF               top_left_m;
  double                mip;
  QPixmap               pixmap;
  QVector<RenderResult> big_tile;
  QVector<RenderResult> big_tiles;

  KRenderPackCollection packs;
  QFont                 font;

  QVector<PointName>     point_names[KRenderPack::render_count];
  QVector<DrawTextEntry> draw_text_array[KRenderPack::render_count];
  QVector<NameHolder>    name_holder_array[KRenderPack::render_count];
  QVector<QRect>         text_rect_array;
  QSizeF                 size_m;
  QRectF                 render_frame_m;

  static void paintOutlinedText(QPainter* p, const QString& text,
                                const QColor& tcolor);

  void run();
  void start() = delete;
  void onFinished();

  void insertPack(int idx, QString path, bool load_now);
  void renderPack(QPainter* p, const KRenderPack* pack,
                  int render_idx, int line_iter);
  void render(QPainter* p, QVector<KRenderPack*> render_packs,
              int render_idx);

  bool checkMipRange(const KPack* pack, const KObject* obj);

  void paintObject(QPainter* p, const KRenderPack* map,
                   const KObject& obj, int render_idx, int line_iter);
  void paintPointNames(QPainter* p);
  void paintLineNames(QPainter* p);
  void paintPolygonNames(QPainter* p);

  bool isCluttering(const QRect&);
  void paintOutlinedText(QPainter* p, const DrawTextEntry& dte);
  void addDrawTextEntry(QVector<DrawTextEntry>& draw_text_array,
                        DrawTextEntry           new_dte);

  QPolygon poly2pix(const KGeoPolygon& polygon);
  void     paintPointName(QPainter* p, const QString& text,
                          const QColor& tcolor);
  void     paintPointObject(QPainter* p, const KRenderPack& pack,
                            const KObject& obj, int render_idx);
  void     paintPolygonObject(QPainter* p, const KRenderPack& pack,
                              const KObject& obj, int render_idx);
  void     paintLineObject(QPainter* painter, const KRenderPack& pack,
                           const KObject& obj, int render_idx,
                           int line_iter);
  QRectF   getDrawRectM() const;
  bool     needToLoadPack(const KRenderPack* pack,
                          const QRectF&      draw_rect);
  void     checkLoad();
  void     checkUnload();
  void     render();
  QPoint   meters2pix(QPointF m) const;
  QPointF  pix2meters(QPointF pix) const;
  QPoint   deg2pix(KGeoCoor) const;
  TileCoor getBigTileCoor(TileCoor);
  QString  getTileName(TileCoor);

public:
  KRender(Settings);
  virtual ~KRender();
  void       requestTile(TileCoor);
  QByteArray pickTile(TileCoor);
};
