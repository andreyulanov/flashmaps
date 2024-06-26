#include "kpanclassmanager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QMetaEnum>

KPanClassManager::KPanClassManager(QString image_dir):
    KClassManager(image_dir)
{
}

void KPanClassManager::loadClasses(QString path, QString images_dir)
{
  QFile         f(path);
  QSet<QString> id_set;
  if (f.open(QIODevice::ReadOnly))
  {
    QJsonParseError error;
    auto doc = QJsonDocument::fromJson(f.readAll(), &error);
    f.close();
    if (doc.isNull())
      return;

    if (doc.isArray())
    {
      auto ar = doc.array();
      for (auto json_val: ar)
      {
        auto obj = json_val.toObject();
        if (obj.isEmpty())
          continue;
        auto main_mip = obj.value("main_mip").toDouble();
        if (main_mip > 0)
        {
          setMainMip(main_mip);
          continue;
        }
        auto tile_mip = obj.value("tile_mip").toDouble();
        if (tile_mip > 0)
        {
          setTileMip(tile_mip);
          continue;
        }
        auto default_coor_precision_coef =
            obj.value("default_coor_precision_coef").toInt();
        if (default_coor_precision_coef > 0)
        {
          setDefaultCoorPrecisionCoef(default_coor_precision_coef);
          continue;
        }

        auto sh = new KPanClass;

        sh->id = obj.value("id").toString();
        if (id_set.contains(sh->id))
        {
          error_str = sh->id + " has been already defined!";
          return;
        }
        id_set.insert(sh->id);
        sh->pan_code = obj.value("code").toInt();
        sh->pan_key  = obj.value("key").toString();
        sh->attrname = obj.value("attrname").toString();
        sh->attrval  = obj.value("attrval").toString();

        auto type_str = obj.value("type").toString();
        if (!type_str.isEmpty())
        {
          bool ok  = false;
          sh->type = static_cast<KClass::Type>(
              QMetaEnum::fromType<KClass::Type>().keyToValue(
                  type_str.toUtf8(), &ok));
          if (!ok)
          {
            error_str = type_str + "is an unrecognized type!";
            return;
          }
        }

        sh->style      = KClass::Solid;
        auto style_str = obj.value("style").toString();
        if (!style_str.isEmpty())
        {
          bool ok   = false;
          sh->style = static_cast<KClass::Style>(
              QMetaEnum::fromType<KClass::Style>().keyToValue(
                  style_str.toUtf8(), &ok));
          if (!ok)
          {
            error_str = type_str + "is an unrecognized style!";
            return;
          }
        }

        sh->layer    = obj.value("layer").toInt();
        sh->width_mm = obj.value("width_mm").toDouble();

        auto l = obj.value("pen").toArray().toVariantList();
        if (l.count() >= 3)
          sh->pen = QColor{l.at(0).toInt(), l.at(1).toInt(),
                           l.at(2).toInt()};
        if (l.count() == 4)
          sh->pen.setAlpha(l.at(3).toInt());
        else
          sh->pen.setAlpha(255);

        l = obj.value("brush").toArray().toVariantList();
        if (l.count() >= 3)
          sh->brush = QColor{l.at(0).toInt(), l.at(1).toInt(),
                             l.at(2).toInt()};
        if (l.count() == 4)
          sh->brush.setAlpha(l.at(3).toInt());
        else
          sh->brush.setAlpha(255);

        l = obj.value("tcolor").toArray().toVariantList();
        if (l.count() >= 3)
          sh->tcolor = QColor{l.at(0).toInt(), l.at(1).toInt(),
                              l.at(2).toInt()};
        if (l.count() == 4)
          sh->tcolor.setAlpha(l.at(3).toInt());
        else
          sh->tcolor.setAlpha(255);

        sh->min_mip     = obj.value("min_mip").toDouble();
        sh->max_mip     = obj.value("max_mip").toDouble();
        sh->name_code   = obj.value("name_code").toInt();
        auto image_name = obj.value("image").toString();
        if (!image_name.isEmpty())
        {
          auto image_path = images_dir + "/" + image_name;
          auto image      = QImage(image_path);
          if (image.isNull())
            qDebug() << "error opening" << image_path;
          else
            sh->image = image.scaledToWidth(sh->getWidthPix(),
                                            Qt::SmoothTransformation);
        }

        auto attributes = obj.value("attributes");
        if (attributes.isArray())
        {
          auto attr_array = attributes.toArray();
          for (auto attr_entry: attr_array)
          {
            auto attr_obj = attr_entry.toObject();
            if (obj.isEmpty())
              continue;
            KPanAttribute attr;
            attr.code    = attr_obj.value("code").toInt();
            attr.name    = attr_obj.value("name").toString();
            attr.visible = attr_obj.value("visible").toBool();
            attr.max_mip = attr_obj.value("max_mip").toDouble();
            sh->attributes.append(attr);
          }
        }

        sh->coor_precision_coef =
            obj.value("coor_precision_coef").toInt();
        if (sh->coor_precision_coef == 0)
          sh->coor_precision_coef = default_coor_precision_coef;

        pan_classes.append(sh);
      }
    }
  }
}

QVector<KPanClass*> KPanClassManager::getClasses()
{
  return pan_classes;
}

int KPanClassManager::getClassIdx(int code, QString key,
                                  QStringList attr_names,
                                  QStringList attr_values)
{
  for (int idx = -1; auto& sh: pan_classes)
  {
    idx++;
    bool code_match = true;
    bool key_match  = true;
    bool attr_match = true;

    if (sh->pan_code == 0 && sh->pan_key.isEmpty() &&
        sh->attrname.isEmpty() && sh->attrval.isEmpty())
      continue;

    if (sh->pan_code > 0 && sh->pan_code != code)
      code_match = false;
    if (!sh->pan_key.isEmpty() && sh->pan_key != key)
      key_match = false;
    if (!sh->attrname.isEmpty() && !attr_names.contains(sh->attrname))
      attr_match = false;
    if (!sh->attrval.isEmpty() && !attr_values.contains(sh->attrval))
      attr_match = false;

    if (code_match && key_match && attr_match)
      return idx;
  }
  return -1;
}
