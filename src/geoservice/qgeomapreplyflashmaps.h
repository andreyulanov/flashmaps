#pragma once

#include <QtLocation/private/qgeotilespec_p.h>
#include <QtLocation/private/qgeotiledmapreply_p.h>
#include "flashrender.h"

class QGeoMapReplyFlashmaps: public QGeoTiledMapReply
{
  Q_OBJECT

public:
  QGeoMapReplyFlashmaps(const QGeoTileSpec& spec,
                        QObject*            parent = 0);
  void onFinishedRender();
};
