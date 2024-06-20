#ifndef QGEOMAPREPLYBINGMAPS_H
#define QGEOMAPREPLYBINGMAPS_H

#include <QtLocation/private/qgeotilespec_p.h>
#include <QtLocation/private/qgeotiledmapreply_p.h>
#include "krender/krender.h"

class QGeoMapReplyBingmaps: public QGeoTiledMapReply
{
  Q_OBJECT

  KRender* m_render = nullptr;

public:
  QGeoMapReplyBingmaps(KRender* render, const QGeoTileSpec& spec,
                       QObject* parent = 0);
  void onFinishedRender();
};

#endif
