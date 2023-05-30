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
#include "common_test_utils.h"

#include <access_token.h>
#include <accesstoken_kit.h>
#include <nativetoken_kit.h>
#include <token_setproc.h>
#include <securec.h>

namespace OHOS::Rosen {
void CommonTestUtils::InjectTokenInfoByHapName(int userID, const std::string& bundleName, int instIndex)
{
    Security::AccessToken::AccessTokenID tokenId =
        Security::AccessToken::AccessTokenKit::GetHapTokenID(userID, bundleName, instIndex);
    SetSelfTokenID(tokenId);
}

std::shared_ptr<Media::PixelMap> CommonTestUtils::CreatePixelMap()
{
    // pixel_map testing code
    Media::InitializationOptions opt;
    opt.size.width = TEST_IMAGE_WIDTH;
    opt.size.height = TEST_IMAGE_HEIGHT;
    opt.pixelFormat = Media::PixelFormat::RGBA_8888;
    opt.alphaType = Media::AlphaType::IMAGE_ALPHA_TYPE_OPAQUE;
    opt.scaleMode = Media::ScaleMode::FIT_TARGET_SIZE;
    opt.editable = false;
    opt.useSourceIfMatch = false;

    const int bitmapDepth = 8; // color depth
    const int bpp = 4; // bytes per pixel
    const int pixelValue = 125;

    const int voulumeSize = opt.size.width * opt.size.height * bpp;
    auto data = (uint32_t *)malloc(voulumeSize);
    if (data == nullptr) {
        return nullptr;
    }

    uint8_t *pic = (uint8_t *)data;
    if (memset_s(pic, voulumeSize, pixelValue, voulumeSize) != EOK) {
        free(data);
        return nullptr;
    }

    uint32_t colorLen = voulumeSize * bitmapDepth;
    auto pixelMap = Media::PixelMap::Create(data, colorLen, opt);
    free(data);
    if (pixelMap == nullptr) {
        return nullptr;
    }
    std::shared_ptr<Media::PixelMap> pixelMap_(pixelMap.release());
    return pixelMap_;
}

void CommonTestUtils::SetAceessTokenPermission(const std::string processName)
{
    uint64_t tokenId;
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 0,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = nullptr,
        .acls = nullptr,
        .processName = processName.c_str(),
        .aplStr = "system_basic",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}

void CommonTestUtils::SetAceessTokenPermission(const std::string processName,
    const char** perms, const int permCount)
{
    if (perms == nullptr || permCount == 0) {
        return;
    }
    uint64_t tokenId;
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = permCount,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = processName.c_str(),
        .aplStr = "system_basic",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}
} // namespace OHOS::Rosen