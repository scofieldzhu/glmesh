/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: main.cpp 
 *  Copyright (c) 2024-2024 scofieldzhu
 *  
 *  MIT License
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#include <QApplication>
#include <QThread>
#include <memory>
#include <QSurfaceFormat>
#include <spdlog/spdlog.h>
#include "main_window.h"
using namespace glmesh;

// MainWindow* gMainWindow = nullptr;

// void LoadMeshData()
// {
//     QString data_file = QCoreApplication::applicationDirPath() + "/Axle shaft.ply";
//     glmMeshPtr mesh_cloud = std::make_shared<MeshPolyData>();
//     ply_reader::LoadFile(data_file, *mesh_cloud, false);
//     gMainWindow->ren_window->loadMeshCloud(mesh_cloud);
// }

int main(int argc, char * argv[])
{
    spdlog::set_level(spdlog::level::trace);

    spdlog::info("Main thread id:{}", QThread::currentThreadId());

    QApplication app(argc, argv);

    QSurfaceFormat format;
#ifdef __APPLE__
    format.setVersion(4, 6);
    format.setProfile(QSurfaceFormat::CoreProfile);
#else
    format.setVersion(4, 6);
    format.setProfile(QSurfaceFormat::CoreProfile);
#endif
    format.setDepthBufferSize(24);

    std::unique_ptr<MainWindow> main_win = std::make_unique<MainWindow>(app, format);

    //QTimer::singleShot(2000, LoadMeshData);

    main_win->show();

    return app.exec();
}

