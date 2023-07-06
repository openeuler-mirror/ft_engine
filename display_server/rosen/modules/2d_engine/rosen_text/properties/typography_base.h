/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef ROSEN_TEXT_PROPERITES_TYPOGRAPHY_BASE_H_
#define ROSEN_TEXT_PROPERITES_TYPOGRAPHY_BASE_H_

#include <vector>
#include "rosen_text/properties/text_style.h"
#include "rosen_text/properties/typography_style.h"
#include "rosen_text/properties/typography_properties.h"
#include "draw/canvas.h"

namespace rosen {
class TypographyCreateBase;

using namespace OHOS::Rosen::Drawing;
class TypographyBase {
public:
    TypographyBase() {};
    virtual ~TypographyBase() {};
    virtual void Init(std::shared_ptr<TypographyCreateBase> typographyCreateBases) = 0;
    virtual double GetMaxWidth() = 0;
    virtual double GetHeight() = 0;
    virtual double GetLongestLine() = 0;
    virtual double GetMinIntrinsicWidth() = 0;
    virtual double GetMaxIntrinsicWidth() = 0;
    virtual double GetAlphabeticBaseline() = 0;
    virtual double GetIdeographicBaseline() = 0;
    virtual bool DidExceedMaxLines() = 0;
    virtual void Layout(double width) = 0;
    virtual void Paint(Canvas* canvas, double x, double y) = 0;
    virtual std::vector<TypographyProperties::TextBox> GetRectsForRange(
        size_t start,
        size_t end,
        TypographyProperties::RectHeightStyle heightStyle,
        TypographyProperties::RectWidthStyle widthStyle) = 0;
    virtual std::vector<TypographyProperties::TextBox> GetRectsForPlaceholders() = 0;
    virtual TypographyProperties::PositionAndAffinity GetGlyphPositionAtCoordinate(double dx,
        double dy) = 0;

    virtual TypographyProperties::PositionAndAffinity GetGlyphPositionAtCoordinateWithCluster(double dx,
        double dy) = 0;
    virtual TypographyProperties::Range<size_t> GetWordBoundary(size_t offset) = 0;
};
} // namespace rosen
#endif // ROSEN_TEXT_UI_TEXT_TYPOGRAPHY_H_
