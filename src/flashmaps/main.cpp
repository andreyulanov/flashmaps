/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained
*in
** a written agreement between you and The Qt Company. For licensing
*terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD
*license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
*are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above
*copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
*USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
*ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
*USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
*DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QThreadPool>
#include <QStandardPaths>
#include <QDir>
#include <QScreen>
#include <QQmlContext>
#include "../flashrender/flashrender.h"
#include "downloadmanager.h"
#include "editablerendermap.h"
#include "editobjectprovider.h"

int main(int argc, char* argv[])
{
  QCoreApplication::addLibraryPath(
      "/home/user/flashmaps/build-main-Desktop-Debug/"
      "geoservice/plugins");

  QGuiApplication app(argc, argv);

  QStringList dir_list = QStandardPaths::standardLocations(
      QStandardPaths::AppDataLocation);
  QString map_dir;
  for (auto dir: dir_list)
  {
    auto md = dir + "/maps";
    if (QDir(md).exists())
    {
      map_dir = md;
      break;
    }
  }

  FlashRender::Settings s;
  auto                  screen = QGuiApplication::screens().first();
  QSize                 screen_size_pix = screen->availableSize();
  QSizeF                screen_size_mm  = screen->physicalSize();

  s.pixel_size_mm = screen_size_mm.width() / screen_size_pix.width();

  QSysInfo si;
  auto     is_device = si.productType().toLower().contains("android");
  if (!is_device)
    s.pixel_size_mm *= 0.5;

  FlashRender render(s);
  render.setParent(qApp);
  render.setObjectName("FlashRender");

  QDir dir(map_dir);
  dir.setNameFilters({"*.flashmap"});
  auto      fi_list   = dir.entryInfoList();
  FlashMap* world_map = nullptr;
  for (auto fi: fi_list)
  {
    int  idx      = -1;
    bool load_now = false;
    auto map      = new FlashRender::Map(fi.filePath());
    if (fi.fileName() == "world.flashmap")
    {
      idx       = 0;
      load_now  = true;
      world_map = map;
    }
    map->loadMainVectorTile(load_now, s.pixel_size_mm);
    render.addMap(map, idx);
  }

  auto chat_map = new EditableRenderMap;
  render.addMap(chat_map);

  double          download_max_mip = 200;
  DownloadManager download_man(map_dir, world_map, download_max_mip);
  QObject::connect(&download_man, &DownloadManager::fetched,
                   [&render, s](QString map_path)
                   {
                     auto map = new FlashRender::Map(map_path);
                     map->loadMainVectorTile(false, s.pixel_size_mm);
                     render.addMap(map, true);
                   });
  QObject::connect(&render, &FlashRender::startedRender,
                   &download_man, &DownloadManager::requestRect);

  EditObjectProvider edit_object_provider;
  QObject::connect(&edit_object_provider,
                   &EditObjectProvider::finishEdit, chat_map,
                   &EditableRenderMap::finishEdit);

  FlashObject edit_object;
  FlashClass  cl;
  cl.id       = "усовершенствованное шоссе";
  cl.type     = FlashClass::Line;
  cl.style    = FlashClass::Solid;
  cl.layer    = 19;
  cl.width_mm = 0.6;
  cl.pen      = QColor(252, 188, 61);
  cl.brush    = QColor(255, 255, 255);
  cl.tcolor   = QColor(0, 0, 0);
  cl.max_mip  = 200;
  FlashGeoPolygon test_poly;
  test_poly.append(FlashGeoCoor::fromDegs(59.9768, 30.3649));
  test_poly.append(FlashGeoCoor::fromDegs(59.99, 30.37));
  edit_object.polygons.append(test_poly);
  chat_map->addObject(edit_object, cl);

  edit_object_provider.startEdit(edit_object, cl);

  QQmlApplicationEngine engine;
  engine.rootContext()->setContextProperty("edit_object_provider",
                                           &edit_object_provider);
  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
  return app.exec();
}
