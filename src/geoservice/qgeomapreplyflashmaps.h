#pragma once

#include <QtLocation/private/qgeotilespec_p.h>
#include <QtLocation/private/qgeotiledmapreply_p.h>
#include "krender/krender.h"

class QGeoMapReplyFlashmaps: public QGeoTiledMapReply
{
  Q_OBJECT

  KRender* m_render = nullptr;

public:
  QGeoMapReplyFlashmaps(KRender* render, const QGeoTileSpec& spec,
                        QObject* parent = 0);
  void onFinishedRender();
};
