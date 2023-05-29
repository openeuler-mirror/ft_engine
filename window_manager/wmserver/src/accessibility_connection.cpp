/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "accessibility_connection.h"

#include "display_manager_service_inner.h"
#include "window_manager.h"
#include "window_manager_agent_controller.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "AccessibilityConnection"};
}

void AccessibilityConnection::NotifyAccessibilityWindowInfo(const sptr<WindowNode>& node, WindowUpdateType type)
{
    if (node == nullptr) {
        WLOGFE("window node is null");
        return;
    }
    auto container = windowRoot_->GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("container is null");
        return;
    }
    std::vector<sptr<WindowNode>> nodes;
    nodes.emplace_back(node);
    UpdateFocusChangeEvent(container);
    NotifyAccessibilityWindowInfo(nodes, container->GetFocusWindow(), type);
}

void AccessibilityConnection::NotifyAccessibilityWindowInfo(DisplayId displayId,
    const std::vector<sptr<WindowNode>>& nodes, WindowUpdateType type)
{
    if (nodes.empty()) {
        WLOGFE("nodes is empty");
        return;
    }
    auto container = windowRoot_->GetOrCreateWindowNodeContainer(displayId);
    if (container == nullptr) {
        WLOGFE("container is null");
        return;
    }
    UpdateFocusChangeEvent(container);
    NotifyAccessibilityWindowInfo(nodes, container->GetFocusWindow(), type);
}

void AccessibilityConnection::NotifyAccessibilityWindowInfo(DisplayId displayId, WindowUpdateType type)
{
    auto container = windowRoot_->GetOrCreateWindowNodeContainer(displayId);
    if (container == nullptr) {
        WLOGFE("container is null");
        return;
    }
    std::vector<sptr<WindowNode>> nodes;
    container->TraverseContainer(nodes);
    UpdateFocusChangeEvent(container);
    NotifyAccessibilityWindowInfo(nodes, container->GetFocusWindow(), type);
}

void AccessibilityConnection::NotifyAccessibilityWindowInfo(const std::vector<sptr<WindowNode>>& nodes,
    uint32_t focusedWindow, WindowUpdateType type) const
{
    std::vector<sptr<AccessibilityWindowInfo>> infos;
    FillAccessibilityWindowInfo(nodes, focusedWindow, infos);
    if (infos.empty()) {
        WLOGFE("infos is empty");
        return;
    }
    WindowManagerAgentController::GetInstance().NotifyAccessibilityWindowInfo(infos, type);
}

void AccessibilityConnection::FillAccessibilityWindowInfo(const std::vector<sptr<WindowNode>>& nodes,
    uint32_t focusedWindow, std::vector<sptr<AccessibilityWindowInfo>>& infos) const
{
    for (auto& node : nodes) {
        sptr<AccessibilityWindowInfo> info = new (std::nothrow) AccessibilityWindowInfo();
        if (info == nullptr) {
            WLOGFE("new accessibilityWindowInfo is null");
            return;
        }
        if (node == nullptr) {
            WLOGFW("node is null");
            break;
        }
        info->wid_ = static_cast<int32_t>(node->GetWindowId());
        info->windowRect_ = node->GetWindowRect();
        info->focused_ = node->GetWindowId() == focusedWindow;
        info->displayId_ = node->GetDisplayId();
        info->layer_ = node->zOrder_;
        info->mode_ = node->GetWindowMode();
        info->type_ = node->GetWindowType();
        auto property = node->GetWindowProperty();
        if (property != nullptr) {
            info->isDecorEnable_ = property->GetDecorEnable();
        }
        infos.emplace_back(info);
    }
}

void AccessibilityConnection::GetAccessibilityWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos) const
{
    std::map<ScreenId, sptr<WindowNodeContainer>> windowNodeContainers;
    std::vector<DisplayId> displayIds = DisplayManagerServiceInner::GetInstance().GetAllDisplayIds();
    for (DisplayId displayId : displayIds) {
        ScreenId screenGroupId = DisplayManagerServiceInner::GetInstance().GetScreenGroupIdByDisplayId(displayId);
        auto container = windowRoot_->GetOrCreateWindowNodeContainer(displayId);
        if (windowNodeContainers.count(screenGroupId) == 0 && container != nullptr) {
            windowNodeContainers.insert(std::make_pair(screenGroupId, container));
            std::vector<sptr<WindowNode>> nodes;
            container->TraverseContainer(nodes);
            FillAccessibilityWindowInfo(nodes, container->GetFocusWindow(), infos);
        }
    }
}

void AccessibilityConnection::UpdateFocusChangeEvent(const sptr<WindowNodeContainer>& container)
{
    if (container == nullptr) {
        WLOGFE("container is null");
        return;
    }
    bool focusChange = false;
    uint32_t focusWindowId = container->GetFocusWindow();
    auto iter = focusedWindowMap_.find(container);
    if (iter == focusedWindowMap_.end()) {
        focusedWindowMap_.insert(std::make_pair(container, focusWindowId));
        if (focusWindowId != INVALID_WINDOW_ID) {
            focusChange = true;
        }
    } else {
        if (focusWindowId != iter->second) {
            focusChange = true;
            iter->second = focusWindowId;
        }
    }
    if (focusChange) {
        auto focusWindowNode = windowRoot_->GetWindowNode(focusWindowId);
        if (focusWindowNode == nullptr) {
            WLOGFE("could not find window");
            return;
        }
        std::vector<sptr<WindowNode>> focusNodes;
        focusNodes.emplace_back(focusWindowNode);
        WLOGFI("notify accessibility window info: focus change, focusWindowId: %{public}u", focusWindowId);
        NotifyAccessibilityWindowInfo(focusNodes, focusWindowId, WindowUpdateType::WINDOW_UPDATE_FOCUSED);
    }
}
}
