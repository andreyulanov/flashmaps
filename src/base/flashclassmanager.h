#pragma once

#include "flashclass.h"

class FlashClassManager: public QObject
{
  double          main_mip                    = 0;
  double          tile_mip                    = 0;
  int             default_coor_precision_coef = 1;
  QString         images_dir;
  QVector<FlashClass> classes;

protected:
  QString error_str;

public:
  FlashClassManager(QString image_dir = QString());

  int    getClassIdxById(QString id);
  FlashClass getClassById(QString id);
  void   loadClasses(QString path, QString images_dir = QString());
  FlashClassImageList getClassImageList();
  QVector<FlashClass> getClasses();

  void   setMainMip(double);
  double getMainMip();

  void   setTileMip(double);
  double getTileMip();

  void   setDefaultCoorPrecisionCoef(double);
  double getDefaultCoorPrecisionCoef();

  QString getErrorStr();
};
