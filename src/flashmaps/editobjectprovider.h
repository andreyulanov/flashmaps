#pragma once

#include <QObject>
#include <qqml.h>
#include <QMetaClassInfo>
#include <QGeoPath>
#include "flashobject.h"

class EditObjectProvider: public QObject
{
  Q_OBJECT
  Q_PROPERTY(QVariantList path READ path NOTIFY pathUpdated)
  QML_ELEMENT

public:
  explicit EditObjectProvider(QObject* parent = nullptr);
  QVariantList path();
  void         startEdit(FlashFreeObject);

signals:
  void pathUpdated();
  void finishEdit(FlashFreeObject);

private:
  QVariantList m_path;
};
