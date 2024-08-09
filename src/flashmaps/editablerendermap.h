#pragma once

#include "flashrender.h"

class EditableRenderMap: public QObject, public FlashRender::Map
{
  Q_OBJECT

  ObjectSpec edit_obj_spec;

public:
  EditableRenderMap();
  void startEdit(ObjectSpec);
  void finishEdit(FlashObject obj, FlashClass cl);
};
