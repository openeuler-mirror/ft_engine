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

#include <iostream>
#include <surface.h>

#include "common/rs_common_def.h"
#include "platform/common/rs_log.h"
#include "ui/rs_node.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"
#include "transaction/rs_transaction.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

class RSDemoDisplayNode : public RSDisplayNode {
public:
    RSDemoDisplayNode() : RSDisplayNode(RSDisplayNodeConfig()) {}
    
    static shared_ptr<RSDemoDisplayNode> CreateForDemo(NodeId id)
    {
        shared_ptr<RSDemoDisplayNode> node = make_shared<RSDemoDisplayNode>();
        node->SetDemoId(node->GetId());
        node->SetId(id);
        return node;
    }

    void RecoveryNode()
    {
        SetId(demoId_);
    }

private:
    void SetDemoId(NodeId id)
    {
        demoId_ = id;
    }

    NodeId demoId_ = 0;
};

class RSDemoSurfaceNode : public RSSurfaceNode {
public:
    RSDemoSurfaceNode() : RSSurfaceNode(RSSurfaceNodeConfig(), true) {}
    
    static shared_ptr<RSDemoSurfaceNode> CreateForDemo(NodeId id)
    {
        shared_ptr<RSDemoSurfaceNode> node = make_shared<RSDemoSurfaceNode>();
        node->SetDemoId(node->GetId());
        node->SetId(id);
        return node;
    }

    void RecoveryNode()
    {
        SetId(demoId_);
    }

private:
    void SetDemoId(NodeId id)
    {
        demoId_ = id;
    }

    NodeId demoId_ = 0;
};

int main()
{
    while (true) {
        int testType = -1;
        uint64_t nodeId = 0;
        int param = -1;
        cout << " ---------------------loop start------------------------" << endl;
        cout << "Input test type, 1: displayNode, 0: surfaceNode: ";
        cin >> testType;
        cout << "Input node id: ";
        cin >> nodeId;
        cout << "Input param: 1: is security, 0: not security: ";
        cin >> param;
        cout << " " << endl;

        if (testType == 0) {
            shared_ptr<RSDemoSurfaceNode> node = RSDemoSurfaceNode::CreateForDemo(nodeId);
            node->SetSecurityLayer(param == 1 ? true : false);
            node->RecoveryNode();
        } else if (testType == 1) {
            shared_ptr<RSDemoDisplayNode> node = RSDemoDisplayNode::CreateForDemo(nodeId);
            node->SetSecurityDisplay(param == 1 ? true : false);
            node->RecoveryNode();
        }
        RSTransaction::FlushImplicitTransaction();
    }
    return 0;
}
