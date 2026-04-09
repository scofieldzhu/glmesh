/* 
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher). 
 *  It reduces the amount of OpenGL code required for rendering and facilitates 
 *  coherent OpenGL.
 *  
 *  File: main_window.cpp 
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

#include "main_window.h"
#include <QBoxLayout>
#include <QFileDialog>
#include <QColorDialog>
#include <spdlog/spdlog.h>
#include <glmesh/core/mesh_poly_data.h>
#include <glmesh/core/mesh_renderer.h>
#include <glmesh/core/trackball.h>
#include <glmesh/core/mesh_actor.h>
#include "ply_reader.h"
#include "mesh_process.h"

namespace {
    const QString stUserTitle = "3D mesh surface rendering control window";
}

MainWindow::MainWindow(QApplication& app, QSurfaceFormat& sf)
    :QMainWindow(nullptr, Qt::WindowFlags())
{
    setMinimumSize(1024, 480);
    ui.setupUi(this);
    setWindowTitle(stUserTitle);
    
    ren_window_ = new RenderWindow(ui.centralwidget, Qt::WindowFlags());
    ren_window_->setFormat(sf);
    auto hbl = dynamic_cast<QHBoxLayout*>(ui.centralwidget->layout());
    hbl->insertWidget(0, ren_window_);
    hbl->update();
    ui.rotate_key_cb->addItem("No Button");
    ui.rotate_key_cb->addItem("Left Button");
    ui.rotate_key_cb->addItem("Right Button");
    ui.rotate_key_cb->addItem("Middle Button");
    ui.rotate_key_cb->setCurrentIndex(1);
    connect(ui.rotate_key_cb, SIGNAL(currentIndexChanged(int)), this, SLOT(onRotationButtonChanged(int)));

    connect(ui.actionLoad_Mesh_Data, SIGNAL(triggered(bool)), this, SLOT(onMenuItemSlot_LoadMeshData(bool)));
    ui.actionDM_Points->setChecked(true);
    connect(ui.actionDM_Points, SIGNAL(triggered(bool)), this, SLOT(onMenuItemSlot_DM_Points(bool)));
    connect(ui.actionDM_Wire, SIGNAL(triggered(bool)), this, SLOT(onMenuItemSlot_DM_Wire(bool)));
    connect(ui.actionDM_Facet, SIGNAL(triggered(bool)), this, SLOT(onMenuItemSlot_DM_Facet(bool)));
    connect(ui.actionChange_Color, SIGNAL(triggered(bool)), this, SLOT(onMenuItemSlot_ChangeColor(bool)));
    connect(ui.triangulate_cb, SIGNAL(toggled(bool)), this, SLOT(onTriangulateToggled(bool)));
}

MainWindow::~MainWindow()
{
}

void MainWindow::onRotationButtonChanged(int index)
{
    ren_window_->trackball()->bindRotationToMouseButton(static_cast<glmesh::MouseButton>(index));
}

void MainWindow::onMenuItemSlot_ChangeColor(bool checked)
{
    if(!ren_window_->existMeshData())
        return;
    QColor color = QColorDialog::getColor(Qt::red, this, "Please choose a color");
    if(color.isValid()){
        ren_window_->renderer()->meshActor()->setUserColor(glm::vec4(color.redF(), color.greenF(), color.blueF(), 1.0f));
        ren_window_->update();
    }
}

void MainWindow::onTriangulateToggled(bool toggled)
{
    if(toggled){
        if(ren_window_->existMeshData()){
            auto tri_mesh = Triangulate(ren_window_->renderer()->currentMeshCloud());
            doLoadMeshData(tri_mesh);  
        }
    }
}

void MainWindow::onMenuItemSlot_DM_Points(bool checked)
{
    if(ren_window_->existMeshData()){
        ren_window_->renderer()->meshActor()->setDispalyMode(glmesh::glmDisplayMode::kPoint);
        ui.actionDM_Points->setChecked(true);
        ui.actionDM_Wire->setChecked(false);
        ui.actionDM_Facet->setChecked(false);
    }
}

void MainWindow::onMenuItemSlot_DM_Facet(bool checked)
{
    if(ren_window_->existMeshData()){
        ren_window_->renderer()->meshActor()->setDispalyMode(glmesh::glmDisplayMode::kFacet);
        ui.actionDM_Facet->setChecked(true);
        ui.actionDM_Wire->setChecked(false);
        ui.actionDM_Points->setChecked(false);
    }
}

void MainWindow::onMenuItemSlot_DM_Wire(bool checked)
{    
    if(ren_window_->existMeshData()){
        ren_window_->renderer()->meshActor()->setDispalyMode(glmesh::glmDisplayMode::kWire);
        ui.actionDM_Wire->setChecked(true);
        ui.actionDM_Facet->setChecked(false);
        ui.actionDM_Points->setChecked(false);
    }
}

void MainWindow::onMenuItemSlot_LoadMeshData(bool checked)
{
    spdlog::info("Load mesh data");
    QString mesh_dir = last_mesh_dir_.isEmpty() ? QCoreApplication::applicationDirPath() : last_mesh_dir_;
    std::string  utf8_str = "D:\\Install\\Data\\牙齿模型";
    mesh_dir = QString::fromUtf8(reinterpret_cast<const char*>(utf8_str.c_str()));
    QString file_name = QFileDialog::getOpenFileName(this, "Open file", mesh_dir, "PLY files (*.ply)");
    if(file_name.isEmpty()){
        return; // user cancel
    }
    last_mesh_dir_ = QFileInfo(file_name).absoluteDir().absolutePath();
    glmesh::glmMeshPtr mesh_cloud = std::make_shared<glmesh::MeshPolyData>();
    ply_reader::LoadFile(file_name, *mesh_cloud, false);
    doLoadMeshData(mesh_cloud);
    auto win_title = stUserTitle;
    win_title.append(QString(" - %1").arg(file_name));
    setWindowTitle(win_title);
}

void MainWindow::doLoadMeshData(glmesh::glmMeshPtr mesh_cloud)
{
    ren_window_->loadMeshCloud(mesh_cloud);
    if(mesh_cloud->triangle_facets.empty()){
        ui.triangulate_cb->setEnabled(true);
        ui.triangulate_cb->setChecked(false);
    }else{
        ui.triangulate_cb->setEnabled(false);
        ui.triangulate_cb->blockSignals(true);
        ui.triangulate_cb->setChecked(true);
        ui.triangulate_cb->blockSignals(false);
    }
    //update mesh information label
    ui.mesh_info_label->setText(QString(" Vertex count: %1 \n Color count: %2 \n Normal count: %3 \n Triangle facet count: %4 \n Polygon facet count:%5")
        .arg(mesh_cloud->vertices.size())
        .arg(mesh_cloud->colors.size())
        .arg(mesh_cloud->normals.size())
        .arg(mesh_cloud->triangle_facets.size())
        .arg(mesh_cloud->poly_facets.size()));
}
