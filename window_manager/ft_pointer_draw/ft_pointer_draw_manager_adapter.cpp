/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <map>
#include <cstdint>
#include "i_pointer_drawing_manager.h"

#if defined(__cplusplus)
extern "C" {
#endif // __cplusplus

using PointerDrawPtr = std::shared_ptr<IPointerDrawingManager>;
std::map<uintptr_t, PointerDrawPtr> g_pointerDrawMap;
constexpr uint32_t MAX_PTRDRAW_NUMS = 64;

static PointerDrawPtr FindPointerDraw(const uintptr_t instanceHdl)
{
    auto it = g_pointerDrawMap.find(instanceHdl);
    if (it == g_pointerDrawMap.end()) {
        return nullptr;
    }
    return it->second;
}

uintptr_t FTPtrDrawMgrGetInstance()
{
    if (g_pointerDrawMap.size() > MAX_PTRDRAW_NUMS) {
        return 0;
    }

    PointerDrawPtr instance = IPointerDrawingManager::GetInstance();
    if (instance == nullptr) {
        return 0;
    }

    uintptr_t instanceHdl = reinterpret_cast<uintptr_t>(instance.get());
    g_pointerDrawMap[instanceHdl] = instance;
    return instanceHdl;
}

void FTPtrDrawMgrFreeInstance(const uintptr_t instanceHdl)
{
    if (g_pointerDrawMap.size() == 0) {
        return;
    }
    g_pointerDrawMap.erase(instanceHdl);
}

bool FTPtrDrawMgrInit(const uintptr_t instanceHdl)
{
    PointerDrawPtr instance = FindPointerDraw(instanceHdl);
    if (instance == nullptr) {
        return false;
    }

    instance->Init();
    return true;
}

bool FTPtrDrawMgrUpdataDispInfo(const uintptr_t instanceHdl, int32_t displayId, int32_t width, int32_t height)
{
    PointerDrawPtr instance = FindPointerDraw(instanceHdl);
    if (instance == nullptr) {
        return false;
    }

    ScreenDisplayInfo displayInfo;
    displayInfo.width = width;
    displayInfo.height = height;
    displayInfo.id = displayId;

    instance->UpdateDisplayInfo(displayInfo);
    return true;
}

bool FTPtrDrawMgrDrawPointer(const uintptr_t instanceHdl, int32_t displayId, int32_t physicalX, int32_t physicalY)
{
    int32_t defaultStyle = 0;
    PointerDrawPtr instance = FindPointerDraw(instanceHdl);
    if (instance == nullptr) {
        return false;
    }

    instance->DrawPointer(displayId, physicalX, physicalY, defaultStyle);
    return true;
}

bool FTPtrDrawMgrGetScreenSize(const uintptr_t instanceHdl, int32_t *width, int32_t *height)
{
    if (width == nullptr || height == nullptr) {
        return false;
    }
    PointerDrawPtr instance = FindPointerDraw(instanceHdl);
    if (instance == nullptr) {
        return false;
    }

    int32_t w = -1;
    int32_t h = -1;
    instance->GetScreenSize(w, h);
    *width = w;
    *height= h;
    return true;
}

#if defined(__cplusplus)
}
#endif // __cplusplus
