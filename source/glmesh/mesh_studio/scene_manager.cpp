/*
 *  glmesh is a mesh data render library base on QOpengl.
 *  glmesh provides object-oriented interfaces to the OpenGL API (3.0 and higher).
 *  It reduces the amount of OpenGL code required for rendering and facilitates
 *  coherent OpenGL.
 *
 *  File: scene_manager.cpp
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
#include "scene_manager.h"
#include "app_log.h"

// ========== 永久对象管理 ==========

QString SceneManager::addObject(RenderableObject obj)
{
    std::lock_guard<std::mutex> lock(mutex_);
    // 如果没有 uid，自动生成
    if(obj.uid.isEmpty()){
        obj.uid = QUuid::createUuid().toString();
    }
    objects_[obj.uid] = std::move(obj);
    return obj.uid;
}

bool SceneManager::removeObject(const QString& uid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = objects_.find(uid);
    if(it != objects_.end()){
        objects_.erase(it);
        return true;
    }
    return false;
}

RenderableObject* SceneManager::getObject(const QString& uid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = objects_.find(uid);
    if(it != objects_.end()){
        return &it->second;
    }
    return nullptr;
}

const RenderableObject* SceneManager::getObject(const QString& uid) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = objects_.find(uid);
    if(it != objects_.end()){
        return &it->second;
    }
    return nullptr;
}

void SceneManager::setObjectVisible(const QString& uid, bool visible)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = objects_.find(uid);
    if(it != objects_.end()){
        it->second.visible = visible;
    }
}

bool SceneManager::hasObject(const QString& uid) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return objects_.find(uid) != objects_.end();
}

// ========== 临时对象管理 ==========

void SceneManager::setTempObject(const QString& category, RenderableObject obj)
{
    std::lock_guard<std::mutex> lock(mutex_);
    // 临时对象使用 category 作为 uid
    obj.uid = category;
    temp_objects_[category] = std::move(obj);
}

void SceneManager::clearTempObject(const QString& category)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = temp_objects_.find(category);
    if(it != temp_objects_.end()){
        temp_objects_.erase(it);
    }
}

RenderableObject* SceneManager::getTempObject(const QString& category)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = temp_objects_.find(category);
    if(it != temp_objects_.end()){
        return &it->second;
    }
    return nullptr;
}

const RenderableObject* SceneManager::getTempObject(const QString& category) const
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = temp_objects_.find(category);
    if(it != temp_objects_.end()){
        return &it->second;
    }
    return nullptr;
}

QString SceneManager::finalizeTempObject(const QString& category)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = temp_objects_.find(category);
    if(it == temp_objects_.end()){
        APP_LOG_WARN("Cannot finalize temp object: category '{}' not found", category.toStdString());
        return QString();
    }
    // 移动临时对象到永久对象
    RenderableObject obj = std::move(it->second);
    temp_objects_.erase(it);
    // 生成新 uid
    obj.uid = QUuid::createUuid().toString();
    QString uid = obj.uid;
    objects_[uid] = std::move(obj);
    APP_LOG_INFO("Finalized temp object '{}' as permanent object '{}'", category.toStdString(), uid.toStdString());
    return uid;
}
