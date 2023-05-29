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

#ifndef FOUNDATION_DMSERVER_ABSTRACT_SCREEN_H
#define FOUNDATION_DMSERVER_ABSTRACT_SCREEN_H


#include <vector>
#include <map>
#include <refbase.h>
#include <screen_manager/screen_types.h>
#include <ui/rs_display_node.h>
#include <ui/rs_surface_node.h>

#include "noncopyable.h"
#include "screen.h"
#include "screen_group.h"
#include "screen_group_info.h"
#include "screen_info.h"

namespace OHOS::Rosen {
class AbstractScreenGroup;
class AbstractScreenController;
class AbstractScreen : public RefBase {
public:
    AbstractScreen(sptr<AbstractScreenController>, const std::string& name, ScreenId dmsId, ScreenId rsId);
    AbstractScreen() = delete;
    WM_DISALLOW_COPY_AND_MOVE(AbstractScreen);
    ~AbstractScreen();
    sptr<SupportedScreenModes> GetActiveScreenMode() const;
    std::vector<sptr<SupportedScreenModes>> GetAbstractScreenModes() const;
    sptr<AbstractScreenGroup> GetGroup() const;
    sptr<ScreenInfo> ConvertToScreenInfo() const;
    bool SetOrientation(Orientation orientation);
    Rotation CalcRotation(Orientation orientation) const;
    bool SetVirtualPixelRatio(float virtualPixelRatio);
    float GetVirtualPixelRatio() const;

    void UpdateRSTree(std::shared_ptr<RSSurfaceNode>& surfaceNode, bool isAdd);
    void UpdateDisplayGroupRSTree(std::shared_ptr<RSSurfaceNode>& surfaceNode, NodeId parentNodeId, bool isAdd);
    void InitRSDisplayNode(RSDisplayNodeConfig& config, Point& startPoint);
    ScreenId GetScreenGroupId() const;

    // colorspace, gamut
    DMError GetScreenSupportedColorGamuts(std::vector<ScreenColorGamut>& colorGamuts);
    DMError GetScreenColorGamut(ScreenColorGamut& colorGamut);
    DMError SetScreenColorGamut(int32_t colorGamutIdx);
    DMError GetScreenGamutMap(ScreenGamutMap& gamutMap);
    DMError SetScreenGamutMap(ScreenGamutMap gamutMap);
    DMError SetScreenColorTransform();

    const std::string name_;
    const ScreenId dmsId_;
    const ScreenId rsId_;
    bool isScreenGroup_ { false };
    std::shared_ptr<RSDisplayNode> rsDisplayNode_;
    RSDisplayNodeConfig rSDisplayNodeConfig_;
    ScreenId groupDmsId_ { SCREEN_ID_INVALID };
    ScreenId lastGroupDmsId_ { SCREEN_ID_INVALID };
    ScreenType type_ { ScreenType::REAL };
    int32_t activeIdx_ { 0 };
    std::vector<sptr<SupportedScreenModes>> modes_ = {};
    float virtualPixelRatio_ = { 1.0 };
    Orientation orientation_ { Orientation::UNSPECIFIED };
    Rotation rotation_ { Rotation::ROTATION_0 };
    Orientation screenRequestedOrientation_ { Orientation::UNSPECIFIED };
protected:
    void FillScreenInfo(sptr<ScreenInfo>) const;
    const sptr<AbstractScreenController> screenController_;
};

class AbstractScreenGroup : public AbstractScreen {
public:
    AbstractScreenGroup(sptr<AbstractScreenController>, ScreenId dmsId, ScreenId rsId, std::string name,
        ScreenCombination combination);
    AbstractScreenGroup() = delete;
    WM_DISALLOW_COPY_AND_MOVE(AbstractScreenGroup);
    ~AbstractScreenGroup();

    bool AddChild(sptr<AbstractScreen>& dmsScreen, Point& startPoint);
    bool AddChildren(std::vector<sptr<AbstractScreen>>& dmsScreens, std::vector<Point>& startPoints);
    bool RemoveChild(sptr<AbstractScreen>& dmsScreen);
    bool HasChild(ScreenId childScreen) const;
    std::vector<sptr<AbstractScreen>> GetChildren() const;
    std::vector<Point> GetChildrenPosition() const;
    Point GetChildPosition(ScreenId screenId) const;
    size_t GetChildCount() const;
    sptr<ScreenGroupInfo> ConvertToScreenGroupInfo() const;

    ScreenCombination combination_ { ScreenCombination::SCREEN_ALONE };
    ScreenId mirrorScreenId_ { SCREEN_ID_INVALID };

private:
    bool GetRSDisplayNodeConfig(sptr<AbstractScreen>& dmsScreen, struct RSDisplayNodeConfig& config);

    std::map<ScreenId, std::pair<sptr<AbstractScreen>, Point>> abstractScreenMap_;
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_ABSTRACT_SCREEN_H