#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include "kpack.h"

class KWorldPack: public KPack
{
public:
  void addPackToMainTile(const KPack&);
};

void KWorldPack::addPackToMainTile(const KPack& m)
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

  QString    first_pack_path = QString(argv[1]) + "/" + argv[2];
  KWorldPack united_pack;
  united_pack.loadAll(first_pack_path, 0);

  for (auto& fi: fi_list)
  {
    if (fi.suffix() != "kpack")
      continue;

    if (fi.absoluteFilePath() == first_pack_path)
      continue;

    qDebug() << "loading" << fi.absoluteFilePath();
    KPack pack;
    pack.loadAll(fi.absoluteFilePath(), 0);
    united_pack.addPackToMainTile(pack);
  }
  qDebug() << "saving united pack...";
  united_pack.save(result_path);
}
