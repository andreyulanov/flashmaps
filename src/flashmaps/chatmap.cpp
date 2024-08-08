#include "chatmap.h"

ChatRenderMap::ChatRenderMap(): FlashRender::Map("")
{
}

void ChatRenderMap::addObject(FlashObject obj, FlashClass cl)
{
  auto   obj_spec = FlashRender::Map::addObject(obj, cl);
  qint64 hash;
  memcpy(&hash, obj.getHash64().data(), sizeof(hash));
  hash_table.insert(hash, obj_spec);
}
