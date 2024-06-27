#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include "flashmap.h"

class WorldMap: public FlashMap
{
public:
  void addMapToMainTile(const FlashMap&);
};

void WorldMap::addMapToMainTile(const FlashMap& m)
{
  frame = frame.united(m.frame);
  for (auto new_obj: m.main)
  {
    auto new_cl = &m.classes[new_obj.class_idx];
    bool found  = false;
    for (int class_idx = -1; auto& cl: classes)
    {
      class_idx++;
      if (new_cl->id == cl.id)
      {
        new_obj.class_idx = class_idx;
        found             = true;
        break;
      }
    }
    if (!found)
      qDebug() << "ERROR: class" << new_cl->id
               << "not found in primary classifier!";
    main.append(new_obj);
  }
}

int main(int argc, char* argv[])
{
  QCoreApplication a(argc, argv);

  QDir dir(argv[1]);
  dir.setFilter(QDir::Files | QDir::NoSymLinks |
                QDir::NoDotAndDotDot);
  auto fi_list = dir.entryInfoList();

  auto result_path = argv[3];

  QFile().remove(result_path);

  QString  first_map_path = QString(argv[1]) + "/" + argv[2];
  WorldMap united_map;
  united_map.loadAll(first_map_path, 0);

  for (auto& fi: fi_list)
  {
    if (fi.suffix() != "flashmap")
      continue;

    if (fi.absoluteFilePath() == first_map_path)
      continue;

    qDebug() << "loading" << fi.absoluteFilePath();
    FlashMap map;
    map.loadAll(fi.absoluteFilePath(), 0);
    united_map.addMapToMainTile(map);
  }
  qDebug() << "saving united map...";
  united_map.save(result_path);
}
