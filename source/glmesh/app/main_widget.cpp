#include "main_widget.h"
#include "glmesh/kernel/io/mesh_loader.h"
#include "glmesh/kernel/core/cpu_polygon_mesh.h"
#include "glmesh/kernel/cpu_to_gpu.h"

MainWidget::MainWidget(QWidget *parent, Qt::WindowFlags flags)
    :QMainWindow(parent, flags)
{
    ui_.setupUi(this);
    connect(ui_.loadPlyBtn, &QAbstractButton::clicked, this, &MainWidget::onLoadPlyBtnClicked);
}

MainWidget::~MainWidget()
{
}

void MainWidget::onLoadPlyBtnClicked()
{
    glmesh::CpuPolygonMesh polygon_mesh;
    if(!LoadPlyAsCpuPolygonMesh("E:/Code/glmesh/CFMC-46-009.ply", polygon_mesh, nullptr)){
        return;
    }
    if(polygon_mesh.polygons.empty()){
        return;        
    }
    glmesh::CpuTriangleMesh triangle_mesh;
    triangle_mesh.buildFromPolygonMesh(polygon_mesh);
    glmesh::GpuTriangleMesh gpu_triangle_mesh = glmesh::ToGpuTriangleMesh(triangle_mesh);
    ui_.meshWidget->updateMesh(gpu_triangle_mesh);
}
