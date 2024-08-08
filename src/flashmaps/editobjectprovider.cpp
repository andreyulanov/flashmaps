#include "editobjectprovider.h"

EditObjectProvider::EditObjectProvider(QObject* parent):
    QObject(parent)
{
}

QVariantList EditObjectProvider::path()
{
  return m_path;
}

void EditObjectProvider::startEdit(FlashObject obj, FlashClass)
{
  for (auto p: obj.polygons.first())
    m_path.append(QVariant::fromValue(
        QGeoCoordinate{p.latitude(), p.longitude()}));
}
