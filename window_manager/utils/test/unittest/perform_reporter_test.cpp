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

#include <gtest/gtest.h>
#include <thread>

#include "perform_reporter.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "PerformReporterTest"};
}
class PerformReporterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    void SimuReportProcess(PerformReporter& pr, const std::vector<uint32_t>& durations);
    bool PerformDataCmp(const PerformReporter& pr, const uint32_t totalCount, const std::vector<uint32_t>& splitCount);
};

void PerformReporterTest::SetUpTestCase()
{
}

void PerformReporterTest::TearDownTestCase()
{
}

void PerformReporterTest::SetUp()
{
}

void PerformReporterTest::TearDown()
{
}

void PerformReporterTest::SimuReportProcess(PerformReporter& pr, const std::vector<uint32_t>& durations)
{
    for (auto duration : durations) {
        pr.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(duration));
        pr.end();
    }
}

bool PerformReporterTest::PerformDataCmp(const PerformReporter& pr,
    const uint32_t totalCount, const std::vector<uint32_t>& splitCount)
{
    if (pr.totalCount_ != totalCount) {
        WLOGFE("pr.totalCount_=%{public}u, expect=%{public}u", pr.totalCount_.load(), totalCount);
        return false;
    }

    size_t i = 0;
    for (auto& iter: pr.timeSplitCount_) {
        if (iter.second != splitCount[i]) {
            std::ostringstream oss;
            oss << "pr.timeSplitCount_[" << iter.first << "]=" << iter.second << ", but expect=" << splitCount[i];
            WLOGFI("%{public}s", oss.str().c_str());
            return false;
        }
        i++;
    }

    return true;
}

namespace {
/**
 * @tc.name: StartEnd
 * @tc.desc: StartEnd test
 * @tc.type: FUNC
 */
HWTEST_F(PerformReporterTest, StartEnd, Function | SmallTest | Level2)
{
    PerformReporter pr = PerformReporter("TestTag", {100, 200, 300}, 10);
    SimuReportProcess(pr, {50, 150, 250, 350, 450});
    ASSERT_EQ(true, PerformDataCmp(pr, 5, {1, 1, 1, 2}));
}

/**
 * @tc.name: StartEndClear
 * @tc.desc: StartEndClear test
 * @tc.type: FUNC
 */
HWTEST_F(PerformReporterTest, StartEndClear, Function | SmallTest | Level2)
{
    PerformReporter pr = PerformReporter("TestTag", {100, 200, 300}, 3);
    SimuReportProcess(pr, {50, 150, 250});
}

/**
 * @tc.name: StartEndInvSeq
 * @tc.desc: StartEndInvSeq test
 * @tc.type: FUNC
 */
HWTEST_F(PerformReporterTest, StartEndInvSeq, Function | SmallTest | Level2)
{
    PerformReporter pr = PerformReporter("TestTag", {100, 200, 300}, 4);
    SimuReportProcess(pr, {250, 150, 50});
}

/**
 * @tc.name: PrivateClear
 * @tc.desc: PrivateClear test
 * @tc.type: FUNC
 */
HWTEST_F(PerformReporterTest, PrivateClear, Function | SmallTest | Level2)
{
    PerformReporter pr = PerformReporter("TestTag", {100, 200, 300}, 10);
    SimuReportProcess(pr, {50, 150, 250, 350, 450});

    pr.clear();
}
}
} // namespace Rosen
} // namespace OHOS