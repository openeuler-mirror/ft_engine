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

#include "pointer_draw.h"

#include "window_manager_hilog.h"
#include "display_manager_service_inner.h"
#include "ui/rs_surface_extractor.h"
#include "transaction/rs_transaction.h"

using namespace OHOS;
using namespace OHOS::Rosen;

namespace FangTian {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "PointerDraw"};
    constexpr int32_t ICON_WIDTH = 30;
    constexpr int32_t ICON_HEIGHT = 30;
}
WM_IMPLEMENT_SINGLE_INSTANCE(PointerDraw)

WMError PointerDraw::Init()
{
    if (InitDisplayInfo() != WMError::WM_OK) {
        WLOGFE("InitDisplayInfo fail");
        return WMError::WM_ERROR_INNER;
    }

    if (InitLayerNode() != WMError::WM_OK) {
        WLOGFE("InitLayerNode fail");
        return WMError::WM_ERROR_INNER;
    }

    if (InitDisplayNode() != WMError::WM_OK) {
        WLOGFE("InitDisplayNode fail");
        return WMError::WM_ERROR_INNER;
    }

    runner_ = AppExecFwk::EventRunner::Create("PointerDraw");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);

    return WMError::WM_OK;
}

void PointerDraw::AsyncMove(int32_t x, int32_t y)
{
    int32_t posx = (x >= 0) ? x : 0;
    posx = (posx <= displayWidth_) ? posx : displayWidth_;

    int32_t posy = (y >= 0) ? y : 0;
    posy = (posy <= displayHeight_) ? posy : displayHeight_;

    PostAsyncTask([this, posx, posy]() {
        MoveTo(posx, posy);
    });
}

WMError PointerDraw::InitDisplayInfo()
{
    auto displayInfo = DisplayManagerServiceInner::GetInstance().GetDefaultDisplay();
    if (displayInfo == nullptr) {
        WLOGFE("GetDefaultDisplay fail");
        return WMError::WM_ERROR_NULLPTR;
    }
    displayWidth_ = displayInfo->GetWidth();
    displayHeight_ = displayInfo->GetHeight();
    displayId_ = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();

    if (displayWidth_ <= 0 || displayHeight_ <= 0) {
        WLOGFE("Invalid display info");
        return WMError::WM_ERROR_INVALID_PARAM;
    }

    return WMError::WM_OK;
}

WMError PointerDraw::InitLayerNode()
{
    RSSurfaceNodeConfig config;
    surfaceNode_ = RSSurfaceNode::Create(config);
    if (surfaceNode_ == nullptr) {
        WLOGFE("RSSurfaceNode::Create fail");
        return WMError::WM_ERROR_NULLPTR;
    }

    surfaceNode_->SetBounds(0, 0, ICON_WIDTH, ICON_HEIGHT);
    rsSurface_ = RSSurfaceExtractor::ExtractRSSurface(surfaceNode_);
    if (rsSurface_ == nullptr) {
        WLOGFE("ExtractRSSurface fail");
        return WMError::WM_ERROR_NULLPTR;
    }

    auto framePtr = rsSurface_->RequestFrame(ICON_WIDTH, ICON_HEIGHT);
    if (framePtr == nullptr) {
        WLOGFE("RequestFrame fail");
        return WMError::WM_ERROR_NULLPTR;
    }

    auto canvas = framePtr->GetCanvas();
    canvas->clear(SK_ColorTRANSPARENT);
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kFill_Style);
    paint.setStrokeJoin(SkPaint::kRound_Join);
    paint.setColor(SK_ColorBLUE);
    canvas->drawRect(SkRect::MakeXYWH(0, 0, ICON_WIDTH, ICON_HEIGHT), paint);
    framePtr->SetDamageRegion(0, 0, ICON_WIDTH, ICON_HEIGHT);
    rsSurface_->FlushFrame(framePtr);

    return WMError::WM_OK;
}

WMError PointerDraw::InitDisplayNode()
{
    RSDisplayNodeConfig config;
    displayNode_ = RSDisplayNode::Create(config);
    if (displayNode_ == nullptr) {
        WLOGFE("RSDisplayNode::Create fail");
        return WMError::WM_ERROR_NULLPTR;
    }

    displayNode_->SetScreenId(displayId_);
    displayNode_->SetBounds(0, 0, displayWidth_, displayHeight_);
    displayNode_->AddChild(surfaceNode_, -1);
    RSTransaction::FlushImplicitTransaction();

    return WMError::WM_OK;
}

WMError PointerDraw::MoveTo(int32_t x, int32_t y)
{
    if (surfaceNode_ == nullptr) {
        WLOGFE("surfaceNode_ is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }

    surfaceNode_->SetBounds(x, y, ICON_WIDTH, ICON_HEIGHT);
    RSTransaction::FlushImplicitTransaction();

    return WMError::WM_OK;
}

void PointerDraw::PostAsyncTask(Task task)
{
    if (handler_ != nullptr) {
        bool ret = handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
        if (!ret) {
            WLOGFE("EventHandler PostTask Failed");
        }
    }
}
} // FangTian
