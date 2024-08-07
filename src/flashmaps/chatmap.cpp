#include "chatmap.h"

ChatMap::ChatMap(): FlashRender::Map("")
{
}

void ChatMap::addObject(FlashFreeObject obj)
{
  auto   obj_spec = FlashRender::Map::addObject(obj);
  qint64 hash;
  memcpy(&hash, obj.getHash64().data(), sizeof(hash));
  hash_table.insert(hash, obj_spec);
}
