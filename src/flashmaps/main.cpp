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
#include "../flashrender/flashrender.h"

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

  QDir dir(map_dir);
  dir.setNameFilters({"*.flashmap"});
  auto fi_list = dir.entryInfoList();
  int  count   = -1;
  for (auto fi: fi_list)
  {
    count++;
    int  idx      = count;
    bool load_now = false;
    if (fi.fileName() == "world.flashmap")
    {
      idx      = 0;
      load_now = true;
    }
    render.loadBackgroundMap(idx, fi.filePath(), load_now);
  }

  //render.loadEditableMap(count, "user1-user2.flashmap");
  QQmlApplicationEngine engine;
  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
  return app.exec();
}
