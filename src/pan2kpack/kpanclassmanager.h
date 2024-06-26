#ifndef KPANCLASSMANAGER_H
#define KPANCLASSMANAGER_H

#include "kpanclass.h"
#include "kclassmanager.h"

class KPanClassManager: public KClassManager
{
  QVector<KPanClass*> pan_classes;

public:
  KPanClassManager(QString image_dir = QString());
  void loadClasses(QString path, QString images_dir = QString());
  int  getClassIdx(int code, QString key, QStringList attr_names,
                   QStringList attr_values);
  QVector<KPanClass*> getClasses();
};

#endif  // KPANCLASSMANAGER_H
