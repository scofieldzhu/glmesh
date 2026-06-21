/*
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher).
 *  It reduces the amount of OpenGL code required for rendering and facilitates
 *  coherent OpenGL.
 *
 *  File: scene_manager.h
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
#ifndef __scene_manager_h__
#define __scene_manager_h__

#include "renderable_object.h"
#include <QString>
#include <QUuid>
#include <unordered_map>
#include <mutex>

/**
 * @brief SceneManager 管理场景中的所有可渲染对象
 *
 * 职责：
 * - 管理永久对象（已完成的 mesh、polyline、annotation 等）
 * - 管理临时对象（绘制预览、临时标注等）
 * - 提供工厂方法创建常见对象类型
 * - 线程安全的对象访问
 *
 * 设计理念：
 * - 将场景对象管理从 MeshWidget 中分离
 * - 支持多种对象类型扩展（polyline、rectangle、annotation 等）
 * - 临时对象用于实时预览，可随时替换或完成
 */
class SceneManager
{
public:
    SceneManager() = default;
    ~SceneManager() = default;

    // ========== 永久对象管理 ==========

    /**
     * @brief 添加对象到场景
     * @param obj 要添加的对象（会自动生成 uid 如果为空）
     * @return 对象的 uid
     */
    QString addObject(RenderableObject obj);

    /**
     * @brief 移除对象
     * @param uid 对象 uid
     * @return 是否成功移除
     */
    bool removeObject(const QString& uid);

    /**
     * @brief 获取对象
     * @param uid 对象 uid
     * @return 对象指针，不存在则返回 nullptr
     */
    RenderableObject* getObject(const QString& uid);
    const RenderableObject* getObject(const QString& uid) const;

    /**
     * @brief 获取所有对象
     */
    const std::unordered_map<QString, RenderableObject>& objects() const { return objects_; }

    /**
     * @brief 设置对象可见性
     */
    void setObjectVisible(const QString& uid, bool visible);

    /**
     * @brief 检查对象是否存在
     */
    bool hasObject(const QString& uid) const;

    // ========== 临时对象管理（用于预览）==========

    /**
     * @brief 设置临时对象（用于绘制预览）
     * @param category 类别标识（如 "polyline_preview"）
     * @param obj 临时对象
     *
     * 同一 category 只能有一个临时对象，新设置会覆盖旧的
     */
    void setTempObject(const QString& category, RenderableObject obj);

    /**
     * @brief 清除临时对象
     * @param category 类别标识
     */
    void clearTempObject(const QString& category);

    /**
     * @brief 获取临时对象
     * @param category 类别标识
     * @return 对象指针，不存在则返回 nullptr
     */
    RenderableObject* getTempObject(const QString& category);
    const RenderableObject* getTempObject(const QString& category) const;

    /**
     * @brief 完成临时对象（转为永久对象）
     * @param category 类别标识
     * @return 永久对象的 uid，失败返回空字符串
     */
    QString finalizeTempObject(const QString& category);

    /**
     * @brief 获取所有临时对象
     */
    const std::unordered_map<QString, RenderableObject>& tempObjects() const { return temp_objects_; }

private:
    std::unordered_map<QString, RenderableObject> objects_;       // 永久对象
    std::unordered_map<QString, RenderableObject> temp_objects_;  // 临时对象
    mutable std::mutex mutex_;                                    // 线程安全
};

#endif
