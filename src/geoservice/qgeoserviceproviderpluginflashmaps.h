#pragma once

#include <QtCore/QObject>
#include <QtLocation/QGeoServiceProviderFactory>

class QGeoServiceProviderFactoryFlashmaps:
    public QObject,
    public QGeoServiceProviderFactory
{
  Q_OBJECT
  Q_INTERFACES(QGeoServiceProviderFactory)
  Q_PLUGIN_METADATA(
      IID
      "org.qt-project.qt.geoservice.serviceproviderfactory/5.0" FILE
      "flashmaps_plugin.json")

public:
  QGeoCodingManagerEngine*
  createGeocodingManagerEngine(const QVariantMap&          parameters,
                               QGeoServiceProvider::Error* error,
                               QString* errorString) const;
  QGeoRoutingManagerEngine*
  createRoutingManagerEngine(const QVariantMap&          parameters,
                             QGeoServiceProvider::Error* error,
                             QString* errorString) const;
  QPlaceManagerEngine*
  createPlaceManagerEngine(const QVariantMap&          parameters,
                           QGeoServiceProvider::Error* error,
                           QString* errorString) const;
  QGeoMappingManagerEngine*
  createMappingManagerEngine(const QVariantMap&          parameters,
                             QGeoServiceProvider::Error* error,
                             QString* errorString) const;
};
