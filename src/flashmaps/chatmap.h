#pragma once

#include "flashrender.h"

class ChatMap: public FlashRender::Map
{
  QMap<qint64, ObjectSpec> hash_table;

public:
  ChatMap();
  void addObject(FlashFreeObject obj);
};
