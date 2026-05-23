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
#include "resource_util.h"

namespace{
    constexpr double kDefaultAmbientFactor = 0.2;
}

MainWidget::MainWidget(QWidget *parent, Qt::WindowFlags flags)
    :QMainWindow(parent, flags)
{
    ui_.setupUi(this);
    ui_.ambientClrBtn->setColor(ui_.meshRenderWidget->ambientLightColor());
    ui_.diffuseClrBtn->setColor(ui_.meshRenderWidget->diffuseLightColor());
    auto style_sheets = ReadStyleSheetFiles({":/qss/control.css", ":/qss/main_widget.css"});
    if(!style_sheets.isEmpty()){
        setStyleSheet(style_sheets);
    }
    ui_.meshRenderWidget->setObjectName("renderViewport");
    connect(ui_.actionImportMesh, &QAction::triggered, this, &MainWidget::onImportMeshActionTriggered);

    QStringList header_labels;
    header_labels << tr("Model File Name") << tr("Visibility");
    ui_.modelTreeWidget->setHeaderLabels(header_labels);
    ui_.modelTreeWidget->clear();
    connect(ui_.modelTreeWidget, &QTreeWidget::itemChanged, this, [this](QTreeWidgetItem* item, int column) {
        if(column == 1) { 
            bool visible = (item->checkState(1) == Qt::Checked);
            auto mesh_uid = this->treeItemToMeshUid(item);
            ui_.meshRenderWidget->setMeshVisible(mesh_uid, visible);
        }
    });
    ui_.modelTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui_.modelTreeWidget, &QTreeWidget::customContextMenuRequested, this, &MainWidget::onCustomContextMenuRequested);
    connect(ui_.modelTreeWidget, &QTreeWidget::currentItemChanged, this, &MainWidget::onCurrentItemChanged);    

    ui_.displayModeComboBox->clear();
    ui_.displayModeComboBox->addItem(tr("Facet"), static_cast<int>(MeshRenderMode::Facet));
    ui_.displayModeComboBox->addItem(tr("Wireframe"), static_cast<int>(MeshRenderMode::Wireframe));
    ui_.displayModeComboBox->addItem(tr("Points"), static_cast<int>(MeshRenderMode::Points));
    ui_.displayModeComboBox->setCurrentIndex(0);
    connect(ui_.displayModeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onDisplayModeChanged(int)));
    connect(ui_.ambientIntensitySlider, &QSlider::valueChanged, this, &MainWidget::onAmbientIntensitySliderValueChanged);
    connect(ui_.ambientClrBtn, &ColorButton::colorChanged, this, [this](const QColor& clr){
        this->ui_.meshRenderWidget->setAmbientLight(clr, this->ui_.ambientIntensitySlider->value());
    });
    connect(ui_.ambientIntensitySlider, &QSlider::valueChanged, this, &MainWidget::onAmbientIntensitySliderValueChanged);
    ui_.diffuseOnCb->setChecked(true);
    connect(ui_.diffuseOnCb, &QAbstractButton::toggled, this, [this](bool chk){
        ui_.meshRenderWidget->setDiffuseLightEnabled(chk);
        ui_.diffuseClrBtn->setEnabled(chk);
        if(!chk){
            ui_.ambientIntensitySlider->setValue(ui_.ambientIntensitySlider->maximum());
        }else{
            ui_.ambientIntensitySlider->setValue(static_cast<int>((ui_.ambientIntensitySlider->maximum() - ui_.ambientIntensitySlider->minimum()) * kDefaultAmbientFactor));        
        }       
    });
    connect(ui_.diffuseClrBtn, &ColorButton::colorChanged, this, [this](const QColor& clr){
        this->ui_.meshRenderWidget->setDiffuseLightColor(clr);
    });
    ui_.ambientIntensitySlider->setValue(static_cast<int>((ui_.ambientIntensitySlider->maximum() - ui_.ambientIntensitySlider->minimum()) * kDefaultAmbientFactor));
    ui_.diffuseOnCb->setChecked(false);
}

MainWidget::~MainWidget()
{
}

void MainWidget::onCustomContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem* target_item = ui_.modelTreeWidget->itemAt(pos);
    if(target_item == nullptr){
        return;
    }
    QString mesh_uid = treeItemToMeshUid(target_item);
    QMenu menu(ui_.modelTreeWidget);
    QAction* delete_action = menu.addAction(tr("Delete"));
    QAction* selected_action = menu.exec(ui_.modelTreeWidget->viewport()->mapToGlobal(pos));
    if(selected_action == delete_action){
        delete target_item;
        ui_.meshRenderWidget->removeMesh(mesh_uid);
        if(ui_.modelTreeWidget->topLevelItemCount()){
            ui_.modelTreeWidget->setCurrentItem(ui_.modelTreeWidget->topLevelItem(0));
        }
    } 
}

void MainWidget::onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if(current){
        auto mesh_uid = treeItemToMeshUid(current);
        ui_.meshRenderWidget->setActiveMesh(mesh_uid);
    }
}

void MainWidget::onAmbientIntensitySliderValueChanged(int value)
{
    float ambient_intensity_value = (float)value / (ui_.ambientIntensitySlider->maximum() - ui_.ambientIntensitySlider->minimum());
    ui_.meshRenderWidget->setAmbientLight(ui_.ambientClrBtn->color(), ambient_intensity_value);
}

void MainWidget::onDisplayModeChanged(int mode_index)
{
    auto render_mode = static_cast<MeshRenderMode>(ui_.displayModeComboBox->itemData(mode_index, Qt::UserRole).toInt());
    ui_.meshRenderWidget->setMeshRenderMode(ui_.meshRenderWidget->activeMesh(), render_mode);
}

QString MainWidget::treeItemToMeshUid(QTreeWidgetItem *item) const
{
    return item->data(0, Qt::UserRole).toString();
}

void MainWidget::onImportMeshActionTriggered()
{
    QString filepath = QFileDialog::getOpenFileName(this, tr("Open PLY File"), QString(), tr("PLY Files (*.ply)"));
    if(filepath.isEmpty()){
        return;
    }     
    glmesh::CpuPolygonMesh polygon_mesh;
    if(!glmesh::LoadPlyAsCpuPolygonMesh(filepath.toLocal8Bit().data(), polygon_mesh, nullptr)){
        APP_LOG_ERROR("Load mesh from ply file:\"{}\" failed!", QStrToLogStr(filepath));
        return;
    }
    if(polygon_mesh.polygons.empty()){
        return;        
    }
    glmesh::CpuTriangleMesh<> triangle_mesh;
    triangle_mesh.buildFromPolygonMesh(polygon_mesh);
    auto mesh_bound_opt = triangle_mesh.calcBounds();
    APP_LOG_TRACE("Center:{} radius:{}", GlmVec3ToStr(mesh_bound_opt->center), mesh_bound_opt->radius);
    glmesh::GpuTriangleMesh<> gpu_triangle_mesh = glmesh::ToGpuTriangleMesh(triangle_mesh);
    MeshWidget::UpdateError err;
    QString mesh_uid = ui_.meshRenderWidget->addMesh(gpu_triangle_mesh, *mesh_bound_opt, &err);
    if(mesh_uid.isEmpty()){
        APP_LOG_ERROR("updateMesh failed! err code:{}", (int)err);
        return;
    }
    QFileInfo fi(filepath);
    QString mesh_filename = fi.fileName();
    QTreeWidgetItem *new_mesh_item = new QTreeWidgetItem(ui_.modelTreeWidget);
    new_mesh_item->setData(0, Qt::UserRole, mesh_uid);
    new_mesh_item->setText(0, mesh_filename);
    new_mesh_item->setToolTip(0, filepath);
    new_mesh_item->setCheckState(1, Qt::Checked);    
    ui_.modelTreeWidget->setCurrentItem(new_mesh_item);
}
