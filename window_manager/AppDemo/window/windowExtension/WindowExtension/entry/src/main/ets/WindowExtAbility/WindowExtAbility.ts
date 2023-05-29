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

// @ts-nocheck
import Extension from '@ohos.application.WindowExtension'
import rpc from '@ohos.rpc'

export default class WindowExtAbility extends Extension {
    onWindowReady(window) {
        window.loadContent('WindowExtAbility/pages/index1').then(() => {
            window.getProperties().then((pro) => {
                console.log("WindowExtension " + JSON.stringify(pro));
            })
            window.show();
        })
    }

    onConnect(want) {
        console.info('JSWindowExtension onConnect ' + want.abilityName);
    }

    onDisconnect(want) {
        console.info('JSWindowExtension onDisconnect ' + want.abilityName);
    }
};