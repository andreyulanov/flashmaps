#include "qgeoserviceproviderpluginflashmaps.h"
#include "qgeotiledmappingmanagerengineflashmaps.h"

QGeoCodingManagerEngine*
QGeoServiceProviderFactoryFlashmaps::createGeocodingManagerEngine(
    const QVariantMap& parameters, QGeoServiceProvider::Error* error,
    QString* errorString) const
{
  Q_UNUSED(parameters)
  Q_UNUSED(error)
  Q_UNUSED(errorString)
  return Q_NULLPTR;  // new
                     // QGeoCodingManagerEngineBingmaps(parameters,
                     // error, errorString);
}

QGeoRoutingManagerEngine*
QGeoServiceProviderFactoryFlashmaps::createRoutingManagerEngine(
    const QVariantMap& parameters, QGeoServiceProvider::Error* error,
    QString* errorString) const
{
  Q_UNUSED(parameters)
  Q_UNUSED(error)
  Q_UNUSED(errorString)

  return Q_NULLPTR;  // new
                     // QGeoRoutingManagerEngineBingmaps(parameters,
                     // error, errorString);
}

QPlaceManagerEngine*
QGeoServiceProviderFactoryFlashmaps::createPlaceManagerEngine(
    const QVariantMap& parameters, QGeoServiceProvider::Error* error,
    QString* errorString) const
{
  Q_UNUSED(parameters)
  Q_UNUSED(error)
  Q_UNUSED(errorString)

  return Q_NULLPTR;  // new QPlaceManagerEngineBingmaps(parameters,
                     // error, errorString);
}

QGeoMappingManagerEngine*
QGeoServiceProviderFactoryFlashmaps::createMappingManagerEngine(
    const QVariantMap& parameters, QGeoServiceProvider::Error* error,
    QString* errorString) const
{
  return new QGeoTiledMappingManagerEngineFlashmaps(parameters, error,
                                                   errorString);
}
