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

#include "minimize_app.h"

#include <ability_manager_client.h>
#include "window_manager_hilog.h"
#include "window_inner_manager.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "MinimizeApp"};
}

std::map<MinimizeReason, std::vector<wptr<WindowNode>>> MinimizeApp::needMinimizeAppNodes_;
bool MinimizeApp::isMinimizedByOtherWindow_ = true;
std::recursive_mutex MinimizeApp::mutex_;
void MinimizeApp::AddNeedMinimizeApp(const sptr<WindowNode>& node, MinimizeReason reason)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!EnableMinimize(reason)) {
        return;
    }
    if (!node) {
        WLOGFE("AddNeedMinimizeApp failed since node is nullptr");
        return;
    }
    wptr<WindowNode> weakNode(node);
    for (auto& appNodes: needMinimizeAppNodes_) {
        auto windowId = node->GetWindowId();
        auto iter = std::find_if(appNodes.second.begin(), appNodes.second.end(),
                                [windowId](wptr<WindowNode> srcNode) {
                                    auto weakSrcNode = srcNode.promote();
                                    if (weakSrcNode == nullptr) {
                                        return false;
                                    }
                                    return weakSrcNode->GetWindowId() == windowId;
                                });
        if (iter != appNodes.second.end()) {
            WLOGFI("[Minimize] Window %{public}u is already in minimize list", node->GetWindowId());
            return;
        }
    }
    WLOGFI("[Minimize] Add Window %{public}u to minimize list, reason %{public}u", node->GetWindowId(), reason);
    needMinimizeAppNodes_[reason].emplace_back(weakNode);
}

std::vector<wptr<WindowNode>> MinimizeApp::GetNeedMinimizeAppNodesWithReason(MinimizeReason reason)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    std::vector<wptr<WindowNode>> needMinimizeAppNodes;
    if (needMinimizeAppNodes_.find(reason) != needMinimizeAppNodes_.end()) {
        for (auto& node : needMinimizeAppNodes_[reason]) {
            needMinimizeAppNodes.emplace_back(node);
        }
    }
    return needMinimizeAppNodes;
}

void MinimizeApp::ExecuteMinimizeAll()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    for (auto& appNodes: needMinimizeAppNodes_) {
        bool isFromUser = IsFromUser(appNodes.first);
        WLOGFI("[Minimize] ExecuteMinimizeAll with size: %{public}zu, reason: %{public}u",
            appNodes.second.size(), appNodes.first);
        for (auto& node : appNodes.second) {
            WindowInnerManager::GetInstance().MinimizeAbility(node, isFromUser);
        }
        appNodes.second.clear();
    }
    needMinimizeAppNodes_.clear();
}

void MinimizeApp::ClearNodesWithReason(MinimizeReason reason)
{
    WLOGFI("[Minimize] ClearNodesWithReason reason %{public}u", reason);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (needMinimizeAppNodes_.find(reason) != needMinimizeAppNodes_.end()) {
        needMinimizeAppNodes_.at(reason).clear();
    }
}

sptr<WindowNode> MinimizeApp::GetRecoverdNodeFromMinimizeList()
{
    WLOGFI("[Minimize] RevertMinimizedNodeForTile");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (needMinimizeAppNodes_.find(MinimizeReason::LAYOUT_TILE) != needMinimizeAppNodes_.end()) {
        auto& tileNodesForMinimize = needMinimizeAppNodes_.at(MinimizeReason::LAYOUT_TILE);
        if (!tileNodesForMinimize.empty()) {
            auto recoverNode = tileNodesForMinimize.back().promote();
            tileNodesForMinimize.pop_back();
            return recoverNode;
        }
    }
    return nullptr;
}

bool MinimizeApp::IsNodeNeedMinimize(const sptr<WindowNode>& node)
{
    if (node == nullptr) {
        WLOGFE("[Minimize] node is nullptr");
        return false;
    }
    for (auto iter : needMinimizeAppNodes_) {
        auto nodes = iter.second;
        if (std::find(nodes.begin(), nodes.end(), node) != nodes.end()) {
            return true;
        }
    }
    return false;
}

bool MinimizeApp::IsNodeNeedMinimizeWithReason(const sptr<WindowNode>& node, MinimizeReason reason)
{
    if (node == nullptr) {
        WLOGFE("[Minimize] node is nullptr");
        return false;
    }
    if (needMinimizeAppNodes_.find(reason) == needMinimizeAppNodes_.end()) {
        WLOGFD("[Minimize] no need to minimize with id:%{public}u reason:%{public}u",
            node->GetWindowId(), reason);
        return false;
    }
    auto nodes = needMinimizeAppNodes_.at(reason);
    if (std::find(nodes.begin(), nodes.end(), node) != nodes.end()) {
        WLOGFI("[Minimize] id:%{public}u need to minimize with reason:%{public}u",
            node->GetWindowId(), reason);
        return true;
    }
    return false;
}

bool MinimizeApp::EnableMinimize(MinimizeReason reason)
{
    bool isFromUser = IsFromUser(reason);
    if (!isMinimizedByOtherWindow_ && !isFromUser) {
        return false;
    }
    return true;
}

void MinimizeApp::ExecuteMinimizeTargetReasons(uint32_t reasons)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    while (reasons) {
        MinimizeReason reason = static_cast<MinimizeReason>(reasons & (~reasons + 1));
        if (needMinimizeAppNodes_.find(reason) != needMinimizeAppNodes_.end()) {
            WLOGFI("[Minimize] ExecuteMinimizeTargetReason with size: %{public}zu, reason: %{public}u",
                needMinimizeAppNodes_.at(reason).size(), reason);
            bool isFromUser = IsFromUser(reason);
            for (auto& node : needMinimizeAppNodes_.at(reason)) {
                WindowInnerManager::GetInstance().MinimizeAbility(node, isFromUser);
            }
            needMinimizeAppNodes_.at(reason).clear();
        }
        reasons -= reason;
    }
}

void MinimizeApp::SetMinimizedByOtherConfig(bool isMinimizedByOther)
{
    isMinimizedByOtherWindow_ = isMinimizedByOther;
}
} // Rosen
} // OHOS
