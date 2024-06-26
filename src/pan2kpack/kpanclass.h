#ifndef KPANCLASS_H
#define KPANCLASS_H

#include "flashclass.h"

struct FlashPanAttribute
{
  QString name;
  int     code;
  bool    visible;
  double  max_mip;
};

struct FlashPanClass: public FlashClass
{
  int                    pan_code = 0;
  QString                pan_key;
  int                    name_code = 0;
  QString                attrname;
  QString                attrval;
  QVector<FlashPanAttribute> attributes;
};

#endif  // KPANCLASS_H
