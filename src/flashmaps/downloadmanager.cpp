#include "downloadmanager.h"
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QDebug>

DownloadManager::DownloadManager(QString         _map_dir,
                                 const FlashMap* world_map,
                                 double          _max_mip)
{
  map_dir = _map_dir;
  max_mip = _max_mip;
  QFile f(map_dir + "/maplist.txt");
  if (f.open(QIODevice::ReadOnly))
  {
    QTextStream in(&f);
    while (!in.atEnd())
      name_list.append(in.readLine());
  }
  for (auto obj: world_map->main)
  {
    auto iso_code =
        QString::fromUtf8(obj.attributes.value("iso_code")).toLower();
    if (iso_code.isEmpty())
      continue;

    QVector<QPolygonF> polygons_m;
    for (auto poly: obj.polygons)
      polygons_m.append(poly.toPolygonM());

    auto entry = LocalMapEntry{obj.frame.toRectM(), polygons_m};
    if (name_list.contains(iso_code))
      iso_metrics_map[iso_code] = entry;
  }
}

void DownloadManager::run()
{
  QDir        dir(map_dir);
  QStringList list;
  list << "*.flashmap";
  dir.setNameFilters(list);
  auto existing_maps = dir.entryList();

  QStringList                          missing_maps;
  QMapIterator<QString, LocalMapEntry> it(iso_metrics_map);
  while (it.hasNext())
  {
    it.next();
    auto lme = it.value();
    if (lme.rect.intersects(requested_rect))
    {
      for (auto poly: lme.polygons)
        if (poly.intersects(requested_rect))
        {
          auto name   = it.key();
          bool exists = false;
          for (auto map_name: existing_maps)
            if (map_name.contains(name))
            {
              exists = true;
              break;
            }
          if (!exists && !missing_maps.contains(name))
            missing_maps.append(name);
          break;
        }
    }
  }

  QDir donwload_dir(map_dir + "/1");
  donwload_dir.setNameFilters(list);
  auto available_maps = donwload_dir.entryList();

  for (auto missing_map: missing_maps)
  {
    for (auto available_map: available_maps)
    {
      if (available_map.contains(missing_map))
      {
        auto path     = map_dir + "/1/" + available_map;
        auto new_path = map_dir + "/" + available_map;
        QFile(path).copy(new_path);
        fetched(new_path);
      }
    }
  }
}

void DownloadManager::requestRect(QRectF rect, double mip)
{
  if (mip > max_mip)
    return;
  requested_rect = rect;
  start();
}
