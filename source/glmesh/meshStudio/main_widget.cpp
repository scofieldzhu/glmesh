/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: main_widget.cpp
 *  Copyright (c) 2024-2026 scofieldzhu
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
#include "main_widget.h"
#include <QFileDialog>
#include "glmesh/kernel/io/mesh_loader.h"
#include "glmesh/kernel/core/cpu_polygon_mesh.h"
#include "glmesh/kernel/cpu_to_gpu.h"
#include "app_log.h"

MainWidget::MainWidget(QWidget *parent, Qt::WindowFlags flags)
    :QMainWindow(parent, flags)
{
    ui_.setupUi(this);
    connect(ui_.actionOpenMesh, &QAction::triggered, this, &MainWidget::onOpenMeshActionTriggered);
}

MainWidget::~MainWidget()
{
}

void MainWidget::onOpenMeshActionTriggered()
{
    QString filepath = QFileDialog::getOpenFileName(this, tr("Open PLY File"), QString(), tr("PLY Files (*.ply)"));
    if(filepath.isEmpty()){
        return;
    }     
    glmesh::CpuPolygonMesh polygon_mesh;
    if(!glmesh::LoadPlyAsCpuPolygonMesh(filepath.toLocal8Bit().data(), polygon_mesh, nullptr)){
        APP_LOG_ERROR("Load mesh from ply file:\"{}\" failed!", filepath.toStdString());
        return;
    }
    if(polygon_mesh.polygons.empty()){
        return;        
    }
    glmesh::CpuTriangleMesh triangle_mesh;
    triangle_mesh.buildFromPolygonMesh(polygon_mesh);
    auto mesh_bound_opt = triangle_mesh.calcBounds();
    glmesh::GpuTriangleMesh gpu_triangle_mesh = glmesh::ToGpuTriangleMesh(triangle_mesh);
    ui_.meshRenderWidget->updateMesh(gpu_triangle_mesh, *mesh_bound_opt);
}
