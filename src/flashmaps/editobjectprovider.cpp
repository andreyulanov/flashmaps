#include "editobjectprovider.h"

EditObjectProvider::EditObjectProvider(QObject* parent):
    QObject(parent)
{
}

QVariantList EditObjectProvider::path()
{
  return m_path;
}

void EditObjectProvider::startEdit(FlashFreeObject obj)
{
  FlashGeoPolygon test_poly;
  test_poly.append(FlashGeoCoor::fromDegs(59.9768, 30.3649));
  test_poly.append(FlashGeoCoor::fromDegs(59.99, 30.37));
  obj.polygons.append(test_poly);

  for (auto p: obj.polygons.first())
  {
    m_path.append(QVariant::fromValue(
        QGeoCoordinate{p.latitude(), p.longitude()}));
  }
}
