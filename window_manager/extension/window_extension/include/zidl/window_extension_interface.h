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

#ifndef WINDOW_EXTENSION_INTERFACE_H
#define WINDOW_EXTENSION_INTERFACE_H

#include <iremote_broker.h>
#include <refbase.h>

#include "window.h"
#include "window_extension_client_interface.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class IWindowExtension : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IWindowExtension");

    enum {
        TRANS_ID_SETBOUNDS = 0,
        TRANS_ID_HIDE_WINDOW,
        TRANS_ID_SHOW_WINDOW,
        TRANS_ID_REQUESTFOCUS,
        TRANS_ID_CONNECT_TO_EXTENSION,
    };

    virtual void SetBounds(const Rect& rect) = 0;
    virtual void Hide() = 0;
    virtual void Show() = 0;
    virtual void RequestFocus() = 0;
    virtual void GetExtensionWindow(sptr<IWindowExtensionClient>& token) = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // WINDOW_EXTENSION_INTERFACE_H