#include "editablerendermap.h"

EditableRenderMap::EditableRenderMap(): FlashRender::Map("")
{
}

void EditableRenderMap::finishEdit(FlashObject obj, FlashClass cl)
{
  if (edit_obj_spec.tile_idx < 0)
    addObject(obj, cl);
  else
    modifyObject(edit_obj_spec, obj, cl);
}
