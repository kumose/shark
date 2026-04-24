// Copyright (C) 2026 Kumo inc. and its affiliates. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

#include <turbo/container/flat_hash_map.h>
#include <turbo/container/span.h>
#include <turbo/base/endian.h>

namespace shark {
    /// @brief LayoutTopology manages the structural "Sharp" of nested columns.
    /// It translates hierarchical logical paths into flat physical memory offsets.

    class LayoutTopology {
    public:

        struct HelperTreeNode {
            int32_t offset{0};
            int32_t flags{0};
            std::vector<HelperTreeNode> children;
        };
        LayoutTopology()  = default;
        ~LayoutTopology() = default;

        void add_node(const std::vector<int32_t> &indexes, int32_t size) {
            auto *cur = &root;
            for (auto &index : indexes) {
                cur = &cur->children[index];
            }
            KCHECK(cur->children.empty());
            cur->children.resize(size);
        }

        void update_node(const std::vector<int32_t> &indexes, int32_t offset, int32_t flag) {
            auto *cur = &root;
            for (auto &index : indexes) {
                cur = &cur->children[index];
            }
            KCHECK(cur->children.empty());
            cur->offset = offset;
            cur->flags = flag;
        }

        std::vector<int64_t> build(std::function<void(int32_t ,int32_t)> callback) const {
            std::vector<int64_t> result;
            std::deque<const HelperTreeNode*> queue;
            queue.push_back(&root);
            while (!queue.empty()) {
                auto cur = queue.front();
                queue.pop_front();
                if (callback) {
                    callback(cur->offset, cur->flags);
                }
                int64_t node = turbo::little_endian::from_host(cur->offset);
                node <<=32;
                node |= turbo::little_endian::from_host(cur->flags);
                result.push_back(node);
                result.push_back(turbo::little_endian::from_host(cur->children.size()));
                for (auto &child : cur->children) {
                    queue.push_back(&child);
                }
            }
            return  result;
        }

        bool read_tree(turbo::span<int64_t> src) {
            if (src.size() < 2) {
                return false;
            }
            root = HelperTreeNode{};
            std::deque<HelperTreeNode*> queue;
            size_t pos = 0;
            queue.push_back(&root);
            const auto limit = src.size() - 1;
            while (!queue.empty() && pos < limit) {
                auto cur = queue.front();
                queue.pop_front();
                auto val = src[pos++];
                cur->flags  = turbo::little_endian::to_host(static_cast<int32_t>(val & 0xFFFFFFFFLL));
                cur->offset = turbo::little_endian::to_host(static_cast<int32_t>(val >> 32));
                auto size = turbo::little_endian::to_host(src[pos++]);
                cur->children.resize(size);
                for (auto &child : cur->children) {
                    queue.push_back(&child);
                }
            }
            if (pos != src.size() || !queue.empty()) {
                return false;
            }
            return true;
        }

        HelperTreeNode root;;
    };
} // namespace shark
