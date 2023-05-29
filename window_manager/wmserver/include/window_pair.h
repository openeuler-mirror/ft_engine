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

#ifndef OHOS_ROSEN_WINDOW_PAIR_H
#define OHOS_ROSEN_WINDOW_PAIR_H

#include <refbase.h>
#include "class_var_definition.h"
#include "window_node.h"
#include "window_layout_policy.h"
#include "wm_common_inner.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
/**
 * @brief Enumerates the status of window pair.
 */
enum class WindowPairStatus : uint32_t {
    STATUS_EMPTY,
    STATUS_SINGLE_PRIMARY,
    STATUS_SINGLE_SECONDARY,
    STATUS_PAIRING,
    STATUS_PAIRED_DONE
};

/**
 * @brief Enumerates the message of split event.
 */
enum class SplitEventMsgType : uint32_t {
    MSG_SHOW_PRIMARY,
    MSG_SHOW_SECONDARY,
    MSG_SHOW_DIVIDER,
    MSG_DESTROY_DIVIDER,
};

class WindowPair : public RefBase {
public:
    /**
     * @brief Constructor used to create an empty WindowPair instance.
     *
     * @param displayId the display of window pair
     */
    explicit WindowPair(const DisplayId& displayId) : displayId_(displayId) {};

    /**
     * @brief Deconstructor used to deconstruct.
     *
     */
    ~WindowPair();

    /**
     * @brief Clear window pair.
     *
     */
    void Clear();

    /**
     * @brief Set split ratio.
     *
     * @param ratio split ratio
     */
    void SetSplitRatio(float ratio);

    /**
     * @brief Get split ratio.
     *
     * @return split ratio
     */
    float GetSplitRatio() const;

    /**
     * @brief Get whether the window pair is paired..
     *
     * @return the pair state of window pair
     */
    bool IsPaired() const;

    /**
     * @brief Handle changes in the state of the window pair
     *
     * @param node trigger window node
     */
    void UpdateIfSplitRelated(sptr<WindowNode>& node);

    /**
     * @brief Handle remove window from pair.
     *
     * @param node target node
     */
    void HandleRemoveWindow(sptr<WindowNode>& node);

    /**
     * @brief Find window node from window pair.
     *
     * @param node target window node
     * @return window node
     */
    sptr<WindowNode> Find(sptr<WindowNode>& node);

    /**
     * @brief Get divider window node.
     *
     * @return divider window node
     */
    sptr<WindowNode> GetDividerWindow() const;

    /**
     * @brief Get pair status.
     *
     * @return the pair status of window pair
     */
    WindowPairStatus GetPairStatus() const;

    /**
     * @brief Get all window node form pair in Z order.
     *
     * @return the list window form pair
     */
    std::vector<sptr<WindowNode>> GetOrderedPair(sptr<WindowNode>& node);

    /**
     * @brief Get all window node form pair.
     *
     * @return the list window form pair
     */
    std::vector<sptr<WindowNode>> GetPairedWindows();

    /**
     * @brief Get whether dock slice is forbidden to move.
     *
     * @return whether dock slice is forbidden to move
     */
    bool IsForbidDockSliceMove() const;

    /**
     * @brief Exit split screen mode when dock slice in exit split screen mode area.
     */
    void ExitSplitMode();

    /**
     * @brief whether dock slice in exit split screen mode area
     */
    bool IsDockSliceInExitSplitModeArea(const std::vector<int32_t>& exitSplitPoints);

    /**
     * @brief Set the initial rect of divider window.
     *
     * @param rect divider window rect
     */
    void SetDividerRect(const Rect& rect);

    /**
     * @brief Update divider window rect when display orientation changed.
     *
     * @param rect default divider rect
     */
    void RotateDividerWindow(const Rect& rect);

    /**
     * @brief Take window pair node snapshot.
     */
    bool TakePairSnapshot();

    /**
     * @brief Clear window pair node snapshot.
     */
    void ClearPairSnapshot();

private:
    /**
     * @brief Gets whether the window is related to split window.
     *
     * @param node target node
     * @return Whether target node is related to the split window
     */
    bool IsSplitRelated(sptr<WindowNode>& node) const;

    /**
     * @brief Replace paired window.
     *
     * @param node current node
     */
    void Insert(sptr<WindowNode>& node);

    /**
     * @brief Update paired window node
     *
     */
    void HandlePairedNodesChange();

    /**
     * @brief Update pair status
     *
     */
    void UpdateWindowPairStatus();

    /**
     * @brief Switch the position of two paired window.
     *
     */
    void SwitchPosition();

    /**
     * @brief Dump the info of pair.
     *
     */
    void DumpPairInfo();

    /**
     * @brief Send split screen event.
     *
     * @param msgType split event message type
     * @param missionId mission id
     */
    void SendSplitScreenCommonEvent(SplitEventMsgType msgType, int32_t missionId);

    /**
     * @brief Send split screen event to notify create recent view.
     *
     * @param node split node
     */
    void NotifyShowRecent(sptr<WindowNode> node);

    /**
     * @brief Send split screen event to notify create or destroy divider window.
     *
     * @param node split node
     * @param isDestroy destroy or create divider window flag
     */
    void NotifyCreateOrDestroyDivider(sptr<WindowNode> node, bool isDestroy);

private:
    float ratio_ = DEFAULT_SPLIT_RATIO;
    DisplayId displayId_;
    sptr<WindowNode> primary_;
    sptr<WindowNode> secondary_;
    sptr<WindowNode> divider_;
    WindowPairStatus status_ = {WindowPairStatus::STATUS_EMPTY};
    Rect dividerRect_ {0, 0, 0, 0};
    DEFINE_VAR_DEFAULT_FUNC_SET(bool, AllSplitAppWindowsRestoring, isAllSplitAppWindowsRestoring, false)
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_PAIR_H