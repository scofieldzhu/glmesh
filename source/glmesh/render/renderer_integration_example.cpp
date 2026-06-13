/*
 *  示例：如何在 MeshWidget 中集成 Renderer 类
 *
 *  这个文件展示了如何将 Renderer 类集成到现有的 QOpenGLWidget 中。
 *  注意：这是一个示例文件，不会被编译到项目中。
 */

#include "glmesh/render/renderer.h"
#include "glmesh/render/camera.h"
#include <QOpenGLWidget>
#include <QMouseEvent>
#include <glad/glad.h>

using namespace glmesh;

class ExampleGLWidget : public QOpenGLWidget
{
public:
    ExampleGLWidget(QWidget* parent = nullptr)
        : QOpenGLWidget(parent)
    {
    }

protected:
    void initializeGL() override
    {
        // 1. 初始化 OpenGL 函数加载器
        gladLoadGLLoader((GLADloadproc)QOpenGLContext::currentContext()->getProcAddress);

        // 2. 设置 Renderer 的渲染窗口尺寸
        renderer_.setRenderWindowSize(width(), height());

        // 3. 设置背景（可选：单色或渐变）
        renderer_.setBackground(glm::vec3(0.2f, 0.2f, 0.2f));
        // 或者使用渐变：
        // renderer_.setBackgroundGradient(
        //     glm::vec3(0.3f, 0.3f, 0.4f),  // 顶部
        //     glm::vec3(0.1f, 0.1f, 0.15f)  // 底部
        // );

        // 4. 设置 Viewport（默认全屏）
        renderer_.setViewport(0.0f, 0.0f, 1.0f, 1.0f);

        // 5. 初始化并绑定 Camera
        camera_.setViewport(width(), height());
        camera_.setPerspective(45.0f, 0.1f, 1000.0f);
        camera_.setDistance(100.0f);
        renderer_.setCamera(&camera_);

        // 6. 其他 OpenGL 初始化...
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);
    }

    void resizeGL(int w, int h) override
    {
        // 更新 Renderer 和 Camera 的尺寸
        renderer_.setRenderWindowSize(w, h);
        camera_.setViewport(w, h);
    }

    void paintGL() override
    {
        // 1. 应用 Viewport
        int vp_x, vp_y, vp_w, vp_h;
        renderer_.getViewportInPixels(vp_x, vp_y, vp_w, vp_h);
        glViewport(vp_x, vp_y, vp_w, vp_h);

        // 2. 应用背景颜色并清屏
        const auto& bg = renderer_.background();
        glClearColor(bg.r, bg.g, bg.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 3. 如果使用渐变背景，需要渲染背景四边形
        if (renderer_.isGradientBackground()) {
            // 使用 GLBkg 渲染渐变背景
            // bg_drawable_->draw();
        }

        // 4. 设置深度测试
        if (renderer_.isDepthTestEnabled()) {
            glEnable(GL_DEPTH_TEST);
        } else {
            glDisable(GL_DEPTH_TEST);
        }

        // 5. 渲染场景对象...
        // for (auto& obj : renderable_objects_) {
        //     obj.material.bind(shader_program);
        //     obj.drawable->draw();
        // }
    }

    void mousePressEvent(QMouseEvent* event) override
    {
        last_mouse_pos_ = event->pos();

        // 示例：使用 Renderer 进行射线投射拾取
        if (event->button() == Qt::LeftButton) {
            Ray pick_ray = renderer_.computePickRay(
                static_cast<float>(event->x()),
                static_cast<float>(event->y())
            );

            // 使用射线进行拾取
            // bool hit = pickObject(pick_ray, hit_point);
            // if (hit) {
            //     qDebug() << "Picked point:" << hit_point.x << hit_point.y << hit_point.z;
            // }
        }
    }

    void mouseMoveEvent(QMouseEvent* event) override
    {
        QPoint delta = event->pos() - last_mouse_pos_;
        last_mouse_pos_ = event->pos();

        if (event->buttons() & Qt::LeftButton) {
            // 轨道旋转
            camera_.orbit(delta.x(), delta.y());
            update();
        } else if (event->buttons() & Qt::MiddleButton) {
            // 平移
            camera_.pan(delta.x(), delta.y());
            update();
        }
    }

    void wheelEvent(QWheelEvent* event) override
    {
        camera_.zoomByWheelDelta(event->angleDelta().y());
        update();
    }

private:
    // 坐标转换示例
    void demonstrateCoordinateConversions()
    {
        // 示例 1: World → Display（3D 世界坐标转屏幕像素）
        glm::vec3 world_pos(10.0f, 5.0f, 0.0f);
        glm::vec3 screen_pos = renderer_.worldToDisplay(world_pos);
        // screen_pos.xy 是屏幕像素坐标，screen_pos.z 是深度 [0,1]

        // 示例 2: Display → World（屏幕像素转 3D 世界坐标）
        glm::vec3 display_pos(400.0f, 300.0f, 0.5f);
        glm::vec3 world_pos_back = renderer_.displayToWorld(display_pos);

        // 示例 3: 通用转换接口
        using CS = Renderer::CoordinateSystem;
        glm::vec3 view_pos = renderer_.convertCoordinate(
            world_pos,
            CS::World,
            CS::View
        );

        // 示例 4: 判断点是否在 Viewport 内
        float mouse_x = 100.0f;
        float mouse_y = 200.0f;
        if (renderer_.isInViewport(mouse_x, mouse_y)) {
            // 处理鼠标事件
        }
    }

    // 多层渲染示例
    void demonstrateMultiLayerRendering()
    {
        // 创建多个 Renderer 用于不同的层
        Renderer bg_renderer;     // 背景层
        Renderer main_renderer;   // 主场景层
        Renderer overlay_renderer; // 叠加层（UI、文字等）

        // 设置层级
        bg_renderer.setLayer(0);
        main_renderer.setLayer(1);
        overlay_renderer.setLayer(2);

        // 叠加层设置为透明（不清除背景）
        overlay_renderer.setTransparent(true);

        // 渲染循环
        std::vector<Renderer*> renderers = {
            &bg_renderer,
            &main_renderer,
            &overlay_renderer
        };

        for (auto* renderer : renderers) {
            // 应用该层的 Viewport
            int x, y, w, h;
            renderer->getViewportInPixels(x, y, w, h);
            glViewport(x, y, w, h);

            // 非透明层需要清屏
            if (!renderer->isTransparent()) {
                const auto& bg = renderer->background();
                glClearColor(bg.r, bg.g, bg.b, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            }

            // 渲染该层的内容...
        }
    }

private:
    Renderer renderer_;
    Camera camera_;
    QPoint last_mouse_pos_;
};

/*
 * 多 Viewport 布局示例（类似 3ds Max / Blender 四视图）
 */
class MultiViewportWidget : public QOpenGLWidget
{
public:
    MultiViewportWidget(QWidget* parent = nullptr)
        : QOpenGLWidget(parent)
    {
    }

protected:
    void initializeGL() override
    {
        gladLoadGLLoader((GLADloadproc)QOpenGLContext::currentContext()->getProcAddress);

        // 设置四个 Viewport：左上(透视)、右上(顶视)、左下(前视)、右下(右视)
        int w = width();
        int h = height();

        // 透视视图（左上）
        perspective_renderer_.setRenderWindowSize(w, h);
        perspective_renderer_.setViewportInPixels(0, h / 2, w / 2, h / 2);
        perspective_renderer_.setBackground(glm::vec3(0.2f, 0.2f, 0.25f));

        // 顶视图（右上）
        top_renderer_.setRenderWindowSize(w, h);
        top_renderer_.setViewportInPixels(w / 2, h / 2, w / 2, h / 2);
        top_renderer_.setBackground(glm::vec3(0.25f, 0.2f, 0.2f));

        // 前视图（左下）
        front_renderer_.setRenderWindowSize(w, h);
        front_renderer_.setViewportInPixels(0, 0, w / 2, h / 2);
        front_renderer_.setBackground(glm::vec3(0.2f, 0.25f, 0.2f));

        // 右视图（右下）
        right_renderer_.setRenderWindowSize(w, h);
        right_renderer_.setViewportInPixels(w / 2, 0, w / 2, h / 2);
        right_renderer_.setBackground(glm::vec3(0.2f, 0.2f, 0.2f));

        // 配置 Camera...
        perspective_camera_.setProjectionType(Camera::ProjectionType::Perspective);
        top_camera_.setProjectionType(Camera::ProjectionType::Orthographic);
        front_camera_.setProjectionType(Camera::ProjectionType::Orthographic);
        right_camera_.setProjectionType(Camera::ProjectionType::Orthographic);

        perspective_renderer_.setCamera(&perspective_camera_);
        top_renderer_.setCamera(&top_camera_);
        front_renderer_.setCamera(&front_camera_);
        right_renderer_.setCamera(&right_camera_);
    }

    void paintGL() override
    {
        // 渲染四个视图
        std::vector<Renderer*> renderers = {
            &perspective_renderer_,
            &top_renderer_,
            &front_renderer_,
            &right_renderer_
        };

        for (auto* renderer : renderers) {
            // 应用该视图的 Viewport
            int x, y, w, h;
            renderer->getViewportInPixels(x, y, w, h);
            glViewport(x, y, w, h);

            // 清屏
            const auto& bg = renderer->background();
            glClearColor(bg.r, bg.g, bg.b, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // 渲染场景...
        }
    }

private:
    // 四个 Renderer
    Renderer perspective_renderer_;
    Renderer top_renderer_;
    Renderer front_renderer_;
    Renderer right_renderer_;

    // 四个 Camera
    Camera perspective_camera_;
    Camera top_camera_;
    Camera front_camera_;
    Camera right_camera_;
};
