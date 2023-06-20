/*
 * Copyright (c) 2021-2023 Huawei Technologies Co., Ltd.
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
#ifndef HI_SYS_EVENT_H
#define HI_SYS_EVENT_H

namespace OHOS {
namespace HiviewDFX {
class HiSysEvent {
public:
    class Domain {
    public:
        static constexpr char AAFWK[] = "AAFWK";
        static constexpr char ACCESS_TOKEN[] = "ACCESS_TOKEN";
        static constexpr char ACCESSIBILITY[] = "ACCESSIBILITY";
        static constexpr char ACCOUNT[] = "ACCOUNT";
        static constexpr char ACE[] = "ACE";
        static constexpr char AI[] = "AI";
        static constexpr char APPEXECFWK[] = "APPEXECFWK";
        static constexpr char AUDIO[] = "AUDIO";
        static constexpr char AV_SESSION[] = "AV_SESSION";
        static constexpr char BACKGROUND_TASK[] = "BACKGROUND_TASK";
        static constexpr char BARRIER_FREE[] = "BARRIERFREE";
        static constexpr char BATTERY[] = "BATTERY";
        static constexpr char BIOMETRICS[] = "BIOMETRICS";
        static constexpr char BLUETOOTH[] = "BLUETOOTH";
        static constexpr char BUNDLE_MANAGER[] = "BUNDLE_MANAGER";
        static constexpr char CAMERA[] = "CAMERA";
        static constexpr char CAST_ENGINE[] = "CASTENGINE";
        static constexpr char CCRUNTIME[] = "CCRUNTIME";
        static constexpr char CERT_MANAGER[] = "CERT_MANAGER";
        static constexpr char COMMONEVENT[] = "COMMONEVENT";
        static constexpr char COMMUNICATION[] = "COMMUNICATION";
        static constexpr char CUSTOMIZATION_CONFIG[] = "CUST_CONFIG";
        static constexpr char CUSTOMIZATION_EDM[] = "CUST_EDM";
        static constexpr char DEVELOPTOOLS[] = "DEVELOPTOOLS";
        static constexpr char DEVICE_AUTH[] = "DEVICE_AUTH";
        static constexpr char DEVICE_PROFILE[] = "DEVICE_PROFILE";
        static constexpr char DISPLAY[] = "DISPLAY";
        static constexpr char DISTRIBUTED_AUDIO[] = "DISTAUDIO";
        static constexpr char DISTRIBUTED_CAMERA[] = "DISTCAMERA";
        static constexpr char DISTRIBUTED_DATAMGR[] = "DISTDATAMGR";
        static constexpr char DISTRIBUTED_DEVICE_MANAGER[] = "DISTDM";
        static constexpr char DISTRIBUTED_HARDWARE_FWK[] = "DISTHWFWK";
        static constexpr char DISTRIBUTED_INPUT[] = "DISTINPUT";
        static constexpr char DISTRIBUTED_SCHEDULE[] = "DISTSCHEDULE";
        static constexpr char DISTRIBUTED_SCREEN[] = "DISTSCREEN";
        static constexpr char DLP_CRE_SERVICE[] = "DLP_CRE_SERVICE";
        static constexpr char DLP[] = "DLP";
        static constexpr char DSLM[] = "DSLM";
        static constexpr char DSOFTBUS[] = "DSOFTBUS";
        static constexpr char FRAMEWORK[] = "FRAMEWORK";
        static constexpr char GLOBAL_I18N[] = "GLOBAL_I18N";
        static constexpr char GLOBAL_RESMGR[] = "GLOBAL_RESMGR";
        static constexpr char GLOBAL[] = "GLOBAL";
        static constexpr char GRAPHIC[] = "GRAPHIC";
        static constexpr char HDF_USB[] = "HDF_USB";
        static constexpr char HIVIEWDFX[] = "HIVIEWDFX";
        static constexpr char HUKS[] = "HUKS";
        static constexpr char IAWARE[] = "IAWARE";
        static constexpr char IMS_UT[] = "IMS_UT";
        static constexpr char INPUTMETHOD[] = "INPUTMETHOD";
        static constexpr char INTELLI_ACCESSORIES[] = "INTELLIACC";
        static constexpr char INTELLI_TV[] = "INTELLITV";
        static constexpr char ISHARE[] = "ISHARE";
        static constexpr char IVI_HARDWARE[] = "IVIHARDWARE";
        static constexpr char LOCATION[] = "LOCATION";
        static constexpr char MEDICAL_SENSOR[] = "MEDICAL_SENSOR";
        static constexpr char MEMMGR[] = "MEMMGR";
        static constexpr char MISCDEVICE[] = "MISCDEVICE";
        static constexpr char MSDP[] = "MSDP";
        static constexpr char MULTI_MEDIA[] = "MULTIMEDIA";
        static constexpr char MULTI_MODAL_INPUT[] = "MULTIMODALINPUT";
        static constexpr char NETMANAGER_STANDARD[] = "NETMANAGER";
        static constexpr char NOTIFICATION[] = "NOTIFICATION";
        static constexpr char PASTEBOARD[] = "PASTEBOARD";
        static constexpr char POWER[] = "POWER";
        static constexpr char POWERMGR[] = "POWERMGR";
        static constexpr char PUSH_MANAGER[] = "PUSH_MANAGER";
        static constexpr char RELIABILITY[] = "RELIABILITY";
        static constexpr char REQUEST[] = "REQUEST";
        static constexpr char ROUTER[] = "ROUTER";
        static constexpr char RSS[] = "RSS";
        static constexpr char SAMGR[] = "SAMGR";
        static constexpr char SECURITY_GUARD[] = "SECURITY_GUARD";
        static constexpr char SENSOR[] = "SENSOR";
        static constexpr char SOURCE_CODE_TRANSFORMER[] = "SRCTRANSFORMER";
        static constexpr char STARTUP[] = "STARTUP";
        static constexpr char TELEPHONY[] = "TELEPHONY";
        static constexpr char THEME[] = "THEME";
        static constexpr char THERMAL[] = "THERMAL";
        static constexpr char TIME[] = "TIME";
        static constexpr char UPDATE[] = "UPDATE";
        static constexpr char USB[] = "USB";
        static constexpr char USERIAM_FWK[] = "USERIAM_FWK";
        static constexpr char USERIAM_PIN[] = "USERIAM_PIN";
        static constexpr char WEARABLE_HARDWARE[] = "WEARABLEHW";
        static constexpr char WEARABLE[] = "WEARABLE";
        static constexpr char WEBVIEW[] = "WEBVIEW";
        static constexpr char WINDOW_MANAGER[] = "WINDOWMANAGER";
        static constexpr char WORK_SCHEDULER[] = "WORKSCHEDULER";
        static constexpr char OTHERS[] = "OTHERS";
    };

    enum EventType {
        FAULT     = 1,    // system fault event
        STATISTIC = 2,    // system statistic event
        SECURITY  = 3,    // system security event
        BEHAVIOR  = 4     // system behavior event
    };
     template<typename... Types>
    static int Write(const std::string &domain, const std::string &eventName,
        EventType type, Types... keyValues){return 0;}
};
} // namespace HiviewDFX
} // namespace OHOS

#endif // HI_SYS_EVENT_H
