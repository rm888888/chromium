// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_PERFORMANCE_MANAGER_POLICIES_PAGE_DISCARDING_HELPER_H_
#define CHROME_BROWSER_PERFORMANCE_MANAGER_POLICIES_PAGE_DISCARDING_HELPER_H_

#include "base/callback_forward.h"
#include "base/containers/flat_map.h"
#include "base/memory/weak_ptr.h"
#include "base/time/time.h"
#include "components/performance_manager/public/decorators/page_live_state_decorator.h"
#include "components/performance_manager/public/features.h"
#include "components/performance_manager/public/graph/graph.h"
#include "components/performance_manager/public/graph/graph_registered.h"
#include "components/performance_manager/public/graph/node_data_describer.h"
#include "components/performance_manager/public/graph/page_node.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace performance_manager {

namespace mechanism {
class PageDiscarder;
}  // namespace mechanism

namespace policies {

// Helper class to be used by the policies that want to discard tabs.
//
// This is a GraphRegistered object and should be accessed via
// PageDiscardingHelper::GetFromGraph(graph()).
class PageDiscardingHelper : public GraphOwned,
                             public PageNode::ObserverDefaultImpl,
                             public GraphRegisteredImpl<PageDiscardingHelper>,
                             public NodeDataDescriberDefaultImpl {
 public:
  PageDiscardingHelper();
  ~PageDiscardingHelper() override;
  PageDiscardingHelper(const PageDiscardingHelper& other) = delete;
  PageDiscardingHelper& operator=(const PageDiscardingHelper&) = delete;

  // Selects a tab to discard based on |strategy| and posts to the UI thread to
  // discard it. This will try to discard a tab until there's been a successful
  // discard or until there's no more discard candidate.
  void UrgentlyDiscardAPage(features::DiscardStrategy discard_strategy,
                            base::OnceCallback<void(bool)> post_discard_cb);

  // Discards multiple tabs to meet the reclaim target based on |strategy| and
  // posts to the UI thread to discard these tabs. Retries discarding if all
  // discardings in the UI thread fail. If |reclaim_target_kb| is nullopt, only
  // discard one tab. If |discard_protected_tabs| is true, protected tab
  // (CanUrgentlyDiscard() returns kProtected) can also be discarded.
  void UrgentlyDiscardMultiplePages(
      absl::optional<uint64_t> reclaim_target_kb,
      features::DiscardStrategy discard_strategy,
      bool discard_protected_tabs,
      base::OnceCallback<void(bool)> post_discard_cb);

  // PageNodeObserver:
  void OnBeforePageNodeRemoved(const PageNode* page_node) override;
  void OnIsAudibleChanged(const PageNode* page_node) override;

  void SetMockDiscarderForTesting(
      std::unique_ptr<mechanism::PageDiscarder> discarder);
  bool CanUrgentlyDiscardForTesting(const PageNode* page_node) const {
    return CanUrgentlyDiscard(page_node) == CanUrgentlyDiscardResult::kEligible;
  }
  void SetGraphForTesting(Graph* graph) { graph_ = graph; }
  static void AddDiscardAttemptMarkerForTesting(PageNode* page_node);
  static void RemovesDiscardAttemptMarkerForTesting(PageNode* page_node);

 protected:
  void OnPassedToGraph(Graph* graph) override;
  void OnTakenFromGraph(Graph* graph) override;

  // Returns the PageLiveStateDecorator::Data associated with a PageNode.
  // Exposed and made virtual to allowed injecting some fake data in tests.
  virtual const PageLiveStateDecorator::Data* GetPageNodeLiveStateData(
      const PageNode* page_node) const;

 private:
  enum class CanUrgentlyDiscardResult {
    // Discarding eligible nodes is hard to notice for user.
    kEligible,
    // Discarding protected nodes is noticeable to user.
    kProtected,
    // Marked nodes can never be discarded.
    kMarked,
  };

  // Indicates if a PageNode can be urgently discarded.
  CanUrgentlyDiscardResult CanUrgentlyDiscard(const PageNode* page_node) const;

  // NodeDataDescriber implementation:
  base::Value DescribePageNodeData(const PageNode* node) const override;

  // Called after each discard attempt. |success| will indicate whether or not
  // the attempt has been successful. |post_discard_cb| will be called once
  // there's been at least one successful discard or if there's no more discard
  // candidates.
  void PostDiscardAttemptCallback(
      absl::optional<uint64_t> reclaim_target_kb,
      features::DiscardStrategy discard_strategy,
      bool discard_protected_tabs,
      base::OnceCallback<void(bool)> post_discard_cb,
      bool success);

  // Map that associates a PageNode with the last time it became non audible.
  // PageNodes that have never been audible are not present in this map.
  base::flat_map<const PageNode*, base::TimeTicks>
      last_change_to_non_audible_time_;

  // The mechanism used to do the actual discarding.
  std::unique_ptr<performance_manager::mechanism::PageDiscarder>
      page_discarder_;

  Graph* graph_ = nullptr;

  SEQUENCE_CHECKER(sequence_checker_);

  base::WeakPtrFactory<PageDiscardingHelper> weak_factory_{this};
};

}  // namespace policies

}  // namespace performance_manager

#endif  // CHROME_BROWSER_PERFORMANCE_MANAGER_POLICIES_PAGE_DISCARDING_HELPER_H_