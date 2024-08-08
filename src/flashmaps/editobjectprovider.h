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
  Q_PROPERTY(QVariant color READ color NOTIFY colorUpdated)
  QML_ELEMENT

public:
  explicit EditObjectProvider(QObject* parent = nullptr);
  QVariantList path();
  QVariant     color();
  void         startEdit(FlashObject, FlashClass cl);

signals:
  void pathUpdated();
  void colorUpdated();
  void finishEdit(FlashObject, FlashClass);

private:
  QVariantList m_path;
  QVariant     m_color;
};
