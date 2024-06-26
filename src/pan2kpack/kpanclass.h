#ifndef KPANCLASS_H
#define KPANCLASS_H

#include "kclass.h"

struct KPanAttribute
{
  QString name;
  int     code;
  bool    visible;
  double  max_mip;
};

struct KPanClass: public KClass
{
  int                    pan_code = 0;
  QString                pan_key;
  int                    name_code = 0;
  QString                attrname;
  QString                attrval;
  QVector<KPanAttribute> attributes;
};

#endif  // KPANCLASS_H
