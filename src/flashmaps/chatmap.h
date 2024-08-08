#pragma once

#include "flashrender.h"

class ChatRenderMap: public QObject, public FlashRender::Map
{
  Q_OBJECT

  QMap<qint64, ObjectSpec> hash_table;

public:
  ChatRenderMap();
  void addObject(FlashObject, FlashClass cl);
};
