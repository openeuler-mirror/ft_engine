/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "screen_manager.h"

#include <map>
#include <vector>

#include <transaction/rs_interfaces.h>

#include "display_manager_adapter.h"
#include "display_manager_agent_default.h"
#include "singleton_delegator.h"
#include "window_manager_hilog.h"


namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "ScreenManager"};
    const static uint32_t MAX_SCREEN_SIZE = 32;
}
class ScreenManager::Impl : public RefBase {
public:
    Impl() = default;
    ~Impl();
    static inline SingletonDelegator<ScreenManager> delegator;
    ScreenId CreateVirtualScreen(VirtualScreenOption option);
    sptr<Screen> GetScreen(ScreenId screenId);
    sptr<ScreenGroup> GetScreenGroup(ScreenId screenId);
    std::vector<sptr<Screen>> GetAllScreens();
    bool RegisterScreenListener(sptr<IScreenListener> listener);
    bool UnregisterScreenListener(sptr<IScreenListener> listener);
    bool RegisterScreenGroupListener(sptr<IScreenGroupListener> listener);
    bool UnregisterScreenGroupListener(sptr<IScreenGroupListener> listener);
    bool RegisterVirtualScreenGroupListener(sptr<IVirtualScreenGroupListener> listener);
    bool UnregisterVirtualScreenGroupListener(sptr<IVirtualScreenGroupListener> listener);
    void OnRemoteDied();

private:
    void NotifyScreenConnect(sptr<ScreenInfo> info);
    void NotifyScreenDisconnect(ScreenId);
    void NotifyScreenChange(const sptr<ScreenInfo>& screenInfo);
    void NotifyScreenChange(const std::vector<sptr<ScreenInfo>>& screenInfos);
    bool UpdateScreenInfoLocked(sptr<ScreenInfo>);
    bool RegisterDisplayManagerAgent();
    bool UnregisterDisplayManagerAgent();
    bool isAllListenersRemoved() const;

    class ScreenManagerListener;
    sptr<ScreenManagerListener> screenManagerListener_;
    std::map<ScreenId, sptr<Screen>> screenMap_;
    std::map<ScreenId, sptr<ScreenGroup>> screenGroupMap_;
    std::recursive_mutex mutex_;
    std::set<sptr<IScreenListener>> screenListeners_;
    std::set<sptr<IScreenGroupListener>> screenGroupListeners_;
    std::set<sptr<IVirtualScreenGroupListener>> virtualScreenGroupListeners_;
    sptr<IDisplayManagerAgent> virtualScreenAgent_ = nullptr;
};

class ScreenManager::Impl::ScreenManagerListener : public DisplayManagerAgentDefault {
public:
    ScreenManagerListener(sptr<Impl> impl) : pImpl_(impl)
    {
    }

    void OnScreenConnect(sptr<ScreenInfo> screenInfo)
    {
        if (screenInfo == nullptr || screenInfo->GetScreenId() == SCREEN_ID_INVALID) {
            WLOGFE("OnScreenConnect, screenInfo is invalid.");
            return;
        }
        if (pImpl_ == nullptr) {
            WLOGFE("OnScreenConnect, impl is nullptr.");
            return;
        }
        pImpl_->NotifyScreenConnect(screenInfo);
        std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
        for (auto listener : pImpl_->screenListeners_) {
            listener->OnConnect(screenInfo->GetScreenId());
        }
    };

    void OnScreenDisconnect(ScreenId screenId)
    {
        if (screenId == SCREEN_ID_INVALID) {
            WLOGFE("OnScreenDisconnect, screenId is invalid.");
            return;
        }
        if (pImpl_ == nullptr) {
            WLOGFE("OnScreenDisconnect, impl is nullptr.");
            return;
        }
        pImpl_->NotifyScreenDisconnect(screenId);
        std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
        for (auto listener : pImpl_->screenListeners_) {
            listener->OnDisconnect(screenId);
        }
    };

    void OnScreenChange(const sptr<ScreenInfo>& screenInfo, ScreenChangeEvent event)
    {
        if (screenInfo == nullptr) {
            WLOGFE("OnScreenChange, screenInfo is null.");
            return;
        }
        if (pImpl_ == nullptr) {
            WLOGFE("OnScreenChange, impl is nullptr.");
            return;
        }
        WLOGFD("OnScreenChange. event %{public}u", event);
        pImpl_->NotifyScreenChange(screenInfo);
        std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
        for (auto listener: pImpl_->screenListeners_) {
            listener->OnChange(screenInfo->GetScreenId());
        }
    };

    void OnScreenGroupChange(const std::string& trigger, const std::vector<sptr<ScreenInfo>>& screenInfos,
        ScreenGroupChangeEvent groupEvent)
    {
        if (screenInfos.empty()) {
            WLOGFE("screenInfos is empty.");
            return;
        }
        if (pImpl_ == nullptr) {
            WLOGFE("impl is nullptr.");
            return;
        }
        WLOGFD("trigger %{public}s, event %{public}u", trigger.c_str(), groupEvent);
        pImpl_->NotifyScreenChange(screenInfos);
        std::vector<ScreenId> screenIds;
        for (auto screenInfo : screenInfos) {
            if (screenInfo->GetScreenId() != SCREEN_ID_INVALID) {
                screenIds.push_back(screenInfo->GetScreenId());
            }
        }
        std::lock_guard<std::recursive_mutex> lock(pImpl_->mutex_);
        for (auto listener: pImpl_->screenGroupListeners_) {
            listener->OnChange(screenIds, groupEvent);
        }
        NotifyVirtualScreenGroupChanged(screenInfos[0], trigger, screenIds, groupEvent);
    };
private:
    void NotifyVirtualScreenGroupChanged(sptr<ScreenInfo> screenInfo,
        const std::string trigger, std::vector<ScreenId>& ids, ScreenGroupChangeEvent groupEvent)
    {
        // check for invalid scene
        if (pImpl_->virtualScreenGroupListeners_.size() <= 0) {
            WLOGFW("no virtual screnn group listeners");
            return;
        }
        if (screenInfo->GetType() != ScreenType::VIRTUAL) {
            WLOGFW("not virtual screen type: %{public}u", screenInfo->GetType());
            return;
        }

        // get the parent of screen
        ScreenId parent = groupEvent == ScreenGroupChangeEvent::ADD_TO_GROUP ?
            screenInfo->GetParentId() : screenInfo->GetLastParentId();
        WLOGFD("parentId=[%{public}llu], lastParentId=[%{public}llu]", (unsigned long long)screenInfo->GetParentId(),
            (unsigned long long)screenInfo->GetLastParentId());
        if (parent == INVALID_SCREEN_ID) {
            WLOGFE("parentId is invalid");
            return;
        }
        auto screenGroup = pImpl_->GetScreenGroup(parent);
        if (screenGroup == nullptr) {
            WLOGFE("screenGroup is null");
            return;
        }

        // notify mirror
        ScreenCombination comb = screenGroup->GetCombination();
        WLOGFD("comb %{public}u", comb);
        IVirtualScreenGroupListener::ChangeInfo changeInfo = {groupEvent, trigger, ids};
        for (auto listener: pImpl_->virtualScreenGroupListeners_) {
            if (comb == ScreenCombination::SCREEN_MIRROR) {
                listener->OnMirrorChange(changeInfo);
            }
        }
    }
    sptr<Impl> pImpl_;
};

WM_IMPLEMENT_SINGLE_INSTANCE(ScreenManager)

ScreenManager::ScreenManager()
{
    pImpl_ = new Impl();
}

ScreenManager::~ScreenManager()
{
}

ScreenManager::Impl::~Impl()
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    UnregisterDisplayManagerAgent();
}

sptr<Screen> ScreenManager::Impl::GetScreen(ScreenId screenId)
{
    auto screenInfo = SingletonContainer::Get<ScreenManagerAdapter>().GetScreenInfo(screenId);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!UpdateScreenInfoLocked(screenInfo)) {
        screenMap_.erase(screenId);
        return nullptr;
    }
    return screenMap_[screenId];
}

sptr<Screen> ScreenManager::GetScreenById(ScreenId screenId)
{
    return pImpl_->GetScreen(screenId);
}

sptr<ScreenGroup> ScreenManager::Impl::GetScreenGroup(ScreenId screenId)
{
    auto screenGroupInfo = SingletonContainer::Get<ScreenManagerAdapter>().GetScreenGroupInfoById(screenId);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (screenGroupInfo == nullptr) {
        WLOGFE("screenGroupInfo is null");
        screenGroupMap_.erase(screenId);
        return nullptr;
    }
    auto iter = screenGroupMap_.find(screenId);
    if (iter != screenGroupMap_.end() && iter->second != nullptr) {
        WLOGFI("get screenGroup in screenGroup map");
        iter->second->UpdateScreenGroupInfo(screenGroupInfo);
        return iter->second;
    }
    sptr<ScreenGroup> screenGroup = new ScreenGroup(screenGroupInfo);
    screenGroupMap_[screenId] = screenGroup;
    return screenGroup;
}

sptr<ScreenGroup> ScreenManager::GetScreenGroup(ScreenId screenId)
{
    return pImpl_->GetScreenGroup(screenId);
}

std::vector<sptr<Screen>> ScreenManager::Impl::GetAllScreens()
{
    auto screenInfos = SingletonContainer::Get<ScreenManagerAdapter>().GetAllScreenInfos();
    std::vector<sptr<Screen>> screens;
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    for (auto info: screenInfos) {
        if (UpdateScreenInfoLocked(info)) {
            screens.emplace_back(screenMap_[info->GetScreenId()]);
        }
    }
    screenMap_.clear();
    for (auto screen: screens) {
        screenMap_.insert(std::make_pair(screen->GetId(), screen));
    }
    return screens;
}

std::vector<sptr<Screen>> ScreenManager::GetAllScreens()
{
    return pImpl_->GetAllScreens();
}

bool ScreenManager::Impl::RegisterScreenListener(sptr<IScreenListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    bool regSucc = RegisterDisplayManagerAgent();
    if (regSucc) {
        screenListeners_.insert(listener);
    }
    return regSucc;
}

bool ScreenManager::RegisterScreenListener(sptr<IScreenListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("RegisterScreenListener listener is nullptr.");
        return false;
    }
    return pImpl_->RegisterScreenListener(listener);
}

bool ScreenManager::Impl::UnregisterScreenListener(sptr<IScreenListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = std::find(screenListeners_.begin(), screenListeners_.end(), listener);
    if (iter == screenListeners_.end()) {
        WLOGFE("could not find this listener");
        return false;
    }
    screenListeners_.erase(iter);
    return isAllListenersRemoved() ? UnregisterDisplayManagerAgent() : true;
}

bool ScreenManager::UnregisterScreenListener(sptr<IScreenListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("UnregisterScreenListener listener is nullptr.");
        return false;
    }
    return pImpl_->UnregisterScreenListener(listener);
}

bool ScreenManager::Impl::RegisterScreenGroupListener(sptr<IScreenGroupListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    bool regSucc = RegisterDisplayManagerAgent();
    if (regSucc) {
        screenGroupListeners_.insert(listener);
    }
    return regSucc;
}

bool ScreenManager::RegisterScreenGroupListener(sptr<IScreenGroupListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("RegisterScreenGroupListener listener is nullptr.");
        return false;
    }
    return pImpl_->RegisterScreenGroupListener(listener);
}

bool ScreenManager::Impl::UnregisterScreenGroupListener(sptr<IScreenGroupListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = std::find(screenGroupListeners_.begin(), screenGroupListeners_.end(), listener);
    if (iter == screenGroupListeners_.end()) {
        WLOGFE("could not find this listener");
        return false;
    }
    screenGroupListeners_.erase(iter);
    return isAllListenersRemoved() ? UnregisterDisplayManagerAgent() : true;
}

bool ScreenManager::UnregisterScreenGroupListener(sptr<IScreenGroupListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("UnregisterScreenGroupListener listener is nullptr.");
        return false;
    }
    return pImpl_->UnregisterScreenGroupListener(listener);
}

bool ScreenManager::Impl::RegisterVirtualScreenGroupListener(sptr<IVirtualScreenGroupListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    bool regSucc = RegisterDisplayManagerAgent();
    if (regSucc) {
        virtualScreenGroupListeners_.insert(listener);
    }
    return regSucc;
}

bool ScreenManager::RegisterVirtualScreenGroupListener(sptr<IVirtualScreenGroupListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("RegisterVirtualScreenGroupListener listener is nullptr.");
        return false;
    }
    return pImpl_->RegisterVirtualScreenGroupListener(listener);
}

bool ScreenManager::Impl::UnregisterVirtualScreenGroupListener(sptr<IVirtualScreenGroupListener> listener)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = std::find(virtualScreenGroupListeners_.begin(), virtualScreenGroupListeners_.end(), listener);
    if (iter == virtualScreenGroupListeners_.end()) {
        WLOGFE("could not find this listener");
        return false;
    }
    virtualScreenGroupListeners_.erase(iter);
    return isAllListenersRemoved() ? UnregisterDisplayManagerAgent() : true;
}

bool ScreenManager::UnregisterVirtualScreenGroupListener(sptr<IVirtualScreenGroupListener> listener)
{
    if (listener == nullptr) {
        WLOGFE("UnregisterVirtualScreenGroupListener listener is nullptr.");
        return false;
    }
    return pImpl_->UnregisterVirtualScreenGroupListener(listener);
}

bool ScreenManager::Impl::RegisterDisplayManagerAgent()
{
    bool regSucc = true;
    if (screenManagerListener_ == nullptr) {
        screenManagerListener_ = new ScreenManagerListener(this);
        bool regSucc = SingletonContainer::Get<ScreenManagerAdapter>().RegisterDisplayManagerAgent(
            screenManagerListener_, DisplayManagerAgentType::SCREEN_EVENT_LISTENER);
        if (!regSucc) {
            screenManagerListener_ = nullptr;
            WLOGFW("RegisterDisplayManagerAgent failed !");
        }
    }
    return regSucc;
}

bool ScreenManager::Impl::UnregisterDisplayManagerAgent()
{
    bool unRegSucc = true;
    if (screenManagerListener_ != nullptr) {
        unRegSucc = SingletonContainer::Get<ScreenManagerAdapter>().UnregisterDisplayManagerAgent(
            screenManagerListener_, DisplayManagerAgentType::SCREEN_EVENT_LISTENER);
        screenManagerListener_ = nullptr;
        if (!unRegSucc) {
            WLOGFW("UnregisterDisplayManagerAgent failed!");
        }
    }
    return unRegSucc;
}

ScreenId ScreenManager::MakeExpand(const std::vector<ExpandOption>& options)
{
    WLOGFI("Make expand");
    if (options.empty()) {
        return SCREEN_ID_INVALID;
    }
    if (options.size() > MAX_SCREEN_SIZE) {
        WLOGFW("Make expand failed. The options size is bigger than %{public}u.", MAX_SCREEN_SIZE);
        return SCREEN_ID_INVALID;
    }
    std::vector<ScreenId> screenIds;
    std::vector<Point> startPoints;
    for (auto& option: options) {
        if (std::find(screenIds.begin(), screenIds.end(), option.screenId_) != screenIds.end()) {
            continue;
        }
        screenIds.emplace_back(option.screenId_);
        startPoints.emplace_back(Point(option.startX_, option.startY_));
    }
    ScreenId group = SingletonContainer::Get<ScreenManagerAdapter>().MakeExpand(screenIds, startPoints);
    if (group == SCREEN_ID_INVALID) {
        WLOGFI("Make expand failed");
    }
    return group;
}

ScreenId ScreenManager::MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenId)
{
    WLOGFI("Make mirror for screen: %{public}" PRIu64"", mainScreenId);
    if (mirrorScreenId.size() > MAX_SCREEN_SIZE) {
        WLOGFW("Make Mirror failed. The mirrorScreenId size is bigger than %{public}u.", MAX_SCREEN_SIZE);
        return SCREEN_ID_INVALID;
    }
    ScreenId group = SingletonContainer::Get<ScreenManagerAdapter>().MakeMirror(mainScreenId, mirrorScreenId);
    if (group == SCREEN_ID_INVALID) {
        WLOGFI("create mirror failed");
    }
    return group;
}

DMError ScreenManager::RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens)
{
    WLOGFI("screens.size=%{public}llu", (unsigned long long)screens.size());
    if (screens.empty()) {
        WLOGFW("RemoveVirtualScreenFromGroup failed. screens is empty.");
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    if (screens.size() > MAX_SCREEN_SIZE) {
        WLOGFW("RemoveVirtualScreenFromGroup failed. The screens size is bigger than %{public}u.", MAX_SCREEN_SIZE);
        return DMError::DM_ERROR_INVALID_PARAM;
    }
    SingletonContainer::Get<ScreenManagerAdapter>().RemoveVirtualScreenFromGroup(screens);
    return DMError::DM_OK;
}

ScreenId ScreenManager::CreateVirtualScreen(VirtualScreenOption option)
{
    return pImpl_->CreateVirtualScreen(option);
}

ScreenId ScreenManager::Impl::CreateVirtualScreen(VirtualScreenOption option)
{
    //  After the process creating the virtual screen is killed, DMS needs to delete the virtual screen
    if (virtualScreenAgent_ == nullptr) {
        virtualScreenAgent_ = new DisplayManagerAgentDefault();
    }
    return SingletonContainer::Get<ScreenManagerAdapter>().CreateVirtualScreen(option, virtualScreenAgent_);
}

DMError ScreenManager::DestroyVirtualScreen(ScreenId screenId)
{
    return SingletonContainer::Get<ScreenManagerAdapter>().DestroyVirtualScreen(screenId);
}

DMError ScreenManager::SetVirtualScreenSurface(ScreenId screenId, sptr<Surface> surface)
{
    return SingletonContainer::Get<ScreenManagerAdapter>().SetVirtualScreenSurface(screenId, surface);
}

bool ScreenManager::SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason)
{
    WLOGFI("state:%{public}u, reason:%{public}u", state, reason);
    return SingletonContainer::Get<ScreenManagerAdapter>().SetScreenPowerForAll(state, reason);
}

ScreenPowerState ScreenManager::GetScreenPower(ScreenId dmsScreenId)
{
    return SingletonContainer::Get<ScreenManagerAdapter>().GetScreenPower(dmsScreenId);
}

DMError ScreenManager::SetScreenRotationLocked(bool isLocked)
{
    SingletonContainer::Get<ScreenManagerAdapter>().SetScreenRotationLocked(isLocked);
    return DMError::DM_OK;
}

bool ScreenManager::IsScreenRotationLocked()
{
    return SingletonContainer::Get<ScreenManagerAdapter>().IsScreenRotationLocked();
}

void ScreenManager::Impl::NotifyScreenConnect(sptr<ScreenInfo> info)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    UpdateScreenInfoLocked(info);
}

void ScreenManager::Impl::NotifyScreenDisconnect(ScreenId screenId)
{
    WLOGFI("screenId:%{public}" PRIu64".", screenId);
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    screenMap_.erase(screenId);
}

void ScreenManager::Impl::NotifyScreenChange(const sptr<ScreenInfo>& screenInfo)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    UpdateScreenInfoLocked(screenInfo);
}

void ScreenManager::Impl::NotifyScreenChange(const std::vector<sptr<ScreenInfo>>& screenInfos)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    for (auto screenInfo : screenInfos) {
        UpdateScreenInfoLocked(screenInfo);
    }
}

bool ScreenManager::Impl::UpdateScreenInfoLocked(sptr<ScreenInfo> screenInfo)
{
    if (screenInfo == nullptr) {
        WLOGFE("displayInfo is null");
        return false;
    }
    ScreenId screenId = screenInfo->GetScreenId();
    WLOGFI("screenId:%{public}" PRIu64".", screenId);
    if (screenId == SCREEN_ID_INVALID) {
        WLOGFE("displayId is invalid.");
        return false;
    }
    auto iter = screenMap_.find(screenId);
    if (iter != screenMap_.end() && iter->second != nullptr) {
        WLOGFI("get screen in screen map");
        iter->second->UpdateScreenInfo(screenInfo);
        return true;
    }
    sptr<Screen> screen = new Screen(screenInfo);
    screenMap_[screenId] = screen;
    return true;
}

bool ScreenManager::Impl::isAllListenersRemoved() const
{
    return screenListeners_.empty() && screenGroupListeners_.empty() && virtualScreenGroupListeners_.empty();
}

void ScreenManager::Impl::OnRemoteDied()
{
    WLOGFI("dms is died");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    screenManagerListener_ = nullptr;
    virtualScreenAgent_ = nullptr;
}

void ScreenManager::OnRemoteDied()
{
    pImpl_->OnRemoteDied();
}
} // namespace OHOS::Rosen