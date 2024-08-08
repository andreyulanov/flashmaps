#include "editobjectprovider.h"

EditObjectProvider::EditObjectProvider(QObject* parent):
    QObject(parent)
{
}

QVariantList EditObjectProvider::path()
{
  return m_path;
}

QVariant EditObjectProvider::color()
{
  return m_color;
}

void EditObjectProvider::startEdit(FlashObject obj, FlashClass cl)
{
  for (auto p: obj.polygons.first())
    m_path.append(QVariant::fromValue(
        QGeoCoordinate{p.latitude(), p.longitude()}));
  m_color = QVariant::fromValue(cl.pen);
}
