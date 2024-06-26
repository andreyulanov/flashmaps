#pragma once

#include "flashpanclass.h"
#include "flashclassmanager.h"

class FlashPanClassManager: public FlashClassManager
{
  QVector<FlashPanClass*> pan_classes;

public:
  FlashPanClassManager(QString image_dir = QString());
  void loadClasses(QString path, QString images_dir = QString());
  int  getClassIdx(int code, QString key, QStringList attr_names,
                   QStringList attr_values);
  QVector<FlashPanClass*> getClasses();
};
