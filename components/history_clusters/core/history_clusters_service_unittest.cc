// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/history_clusters/core/history_clusters_service.h"

#include <memory>
#include <string>
#include <vector>

#include "base/callback_forward.h"
#include "base/containers/contains.h"
#include "base/files/scoped_temp_dir.h"
#include "base/run_loop.h"
#include "base/strings/stringprintf.h"
#include "base/task/cancelable_task_tracker.h"
#include "base/test/bind.h"
#include "base/test/gtest_util.h"
#include "base/test/metrics/histogram_tester.h"
#include "base/test/scoped_feature_list.h"
#include "base/test/task_environment.h"
#include "base/time/time.h"
#include "build/build_config.h"
#include "components/history/core/browser/history_context.h"
#include "components/history/core/browser/history_types.h"
#include "components/history/core/browser/url_row.h"
#include "components/history/core/test/history_service_test_util.h"
#include "components/history_clusters/core/clustering_backend.h"
#include "components/history_clusters/core/history_clusters_service_test_api.h"
#include "components/history_clusters/core/history_clusters_types.h"
#include "components/history_clusters/core/memories_features.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "url/gurl.h"

namespace history_clusters {

namespace {

// Trivial backend to allow us to specifically test just the service behavior.
class TestClusteringBackend : public ClusteringBackend {
 public:
  void GetClusters(
      ClustersCallback callback,
      const std::vector<history::AnnotatedVisit>& visits) override {
    callback_ = std::move(callback);
    last_clustered_visits_ = visits;

    std::move(wait_for_get_clusters_closure_).Run();
  }

  void FulfillCallback(const std::vector<history::Cluster>& clusters) {
    std::move(callback_).Run(clusters);
  }

  const std::vector<history::AnnotatedVisit>& LastClusteredVisits() const {
    return last_clustered_visits_;
  }

  // Fetches a scored visit by an ID. `visit_id` must be valid. This is a
  // convenience method used for constructing the fake response.
  history::ClusterVisit GetVisitById(int visit_id, float score = 0.5) {
    for (auto& visit : last_clustered_visits_) {
      if (visit.visit_row.visit_id == visit_id) {
        history::ClusterVisit cluster_visit;
        cluster_visit.annotated_visit = visit;
        cluster_visit.normalized_url = visit.url_row.url();
        cluster_visit.score = score;
        return cluster_visit;
      }
    }

    NOTREACHED() << "TestClusteringBackend::GetVisitById "
                 << "could not find visit_id: " << visit_id;
    return history::ClusterVisit();
  }

  // Should be invoked before `GetClusters()` is invoked.
  void WaitForGetClustersCall() {
    base::RunLoop loop;
    wait_for_get_clusters_closure_ = loop.QuitClosure();
    loop.Run();
  }

 private:
  base::OnceClosure wait_for_get_clusters_closure_;

  ClustersCallback callback_;
  std::vector<history::AnnotatedVisit> last_clustered_visits_;
};

class HistoryClustersServiceTest : public testing::Test {
 public:
  HistoryClustersServiceTest()
      : task_environment_(
            base::test::SingleThreadTaskEnvironment::TimeSource::MOCK_TIME),
        run_loop_quit_(run_loop_.QuitClosure()) {
    scoped_feature_list_ = std::make_unique<base::test::ScopedFeatureList>();
    scoped_feature_list_->InitAndEnableFeature(kJourneys);

    CHECK(history_dir_.CreateUniqueTempDir());
    history_service_ =
        history::CreateHistoryService(history_dir_.GetPath(), true);
    history_clusters_service_ = std::make_unique<HistoryClustersService>(
        history_service_.get(), /*template_url_service=*/nullptr,
        /*entity_metadata_provider=*/nullptr,
        /*url_loader_factory=*/nullptr);

    history_clusters_service_test_api_ =
        std::make_unique<HistoryClustersServiceTestApi>(
            history_clusters_service_.get(), history_service_.get());
    auto test_backend = std::make_unique<TestClusteringBackend>();
    test_clustering_backend_ = test_backend.get();
    history_clusters_service_test_api_->SetClusteringBackendForTest(
        std::move(test_backend));
  }

  HistoryClustersServiceTest(const HistoryClustersServiceTest&) = delete;
  HistoryClustersServiceTest& operator=(const HistoryClustersServiceTest&) =
      delete;

  // Add hardcoded completed visits with context annotations to the history
  // database.
  void AddHardcodedTestDataToHistoryService() {
    history::ContextID context_id = reinterpret_cast<history::ContextID>(1);

    for (auto& visit : GetHardcodedTestVisits()) {
      history::HistoryAddPageArgs add_page_args;
      add_page_args.context_id = context_id;
      add_page_args.nav_entry_id = next_navigation_id_;
      add_page_args.url = visit.url_row.url();
      add_page_args.title = visit.url_row.title();
      add_page_args.time = visit.visit_row.visit_time;
      add_page_args.visit_source = visit.source;
      history_service_->AddPage(add_page_args);
      history_service_->UpdateWithPageEndTime(
          context_id, next_navigation_id_, visit.url_row.url(),
          visit.visit_row.visit_time + visit.visit_row.visit_duration);

      auto& incomplete_visit_context_annotations =
          history_clusters_service_
              ->GetOrCreateIncompleteVisitContextAnnotations(
                  next_navigation_id_);
      incomplete_visit_context_annotations.visit_row = visit.visit_row;
      incomplete_visit_context_annotations.url_row = visit.url_row;
      incomplete_visit_context_annotations.context_annotations =
          visit.context_annotations;
      incomplete_visit_context_annotations.status.history_rows = true;
      incomplete_visit_context_annotations.status.navigation_ended = true;
      incomplete_visit_context_annotations.status.navigation_end_signals = true;
      history_clusters_service_->CompleteVisitContextAnnotationsIfReady(
          next_navigation_id_);
      next_navigation_id_++;
    }
  }

  // Add an incomplete visit context annotations to the in memory incomplete
  // visit map. Does not touch the history database.
  void AddIncompleteVisit(
      history::URLID url_id,
      history::VisitID visit_id,
      base::Time visit_time,
      ui::PageTransition transition = ui::PageTransitionFromInt(
          ui::PAGE_TRANSITION_LINK | ui::PAGE_TRANSITION_CHAIN_START |
          ui::PAGE_TRANSITION_CHAIN_END)) {
    // It's not possible to have an incomplete visit with URL or visit set but
    // not the other. The IDs must either both be 0 or both be non-zero.
    ASSERT_FALSE(url_id ^ visit_id);
    auto& incomplete_visit_context_annotations =
        history_clusters_service_->GetOrCreateIncompleteVisitContextAnnotations(
            next_navigation_id_);
    incomplete_visit_context_annotations.url_row.set_id(url_id);
    incomplete_visit_context_annotations.visit_row.visit_id = visit_id;
    incomplete_visit_context_annotations.visit_row.visit_time = visit_time;
    incomplete_visit_context_annotations.visit_row.transition = transition;
    incomplete_visit_context_annotations.status.history_rows = url_id;
    next_navigation_id_++;
  }

  // Verifies that the hardcoded visits were passed to the clustering backend.
  void AwaitAndVerifyTestClusteringBackendRequest(bool for_keywords = false) {
    test_clustering_backend_->WaitForGetClustersCall();

    std::vector<history::AnnotatedVisit> visits =
        test_clustering_backend_->LastClusteredVisits();

    // Keyword requests should not fetch visits older than 30 days; cluster
    // requests should fetch all visits.
    ASSERT_EQ(visits.size(), for_keywords ? 2u : 3u);

    auto& visit = visits[0];
    EXPECT_EQ(visit.visit_row.visit_id, 2);
    EXPECT_EQ(visit.visit_row.visit_time,
              GetHardcodedTestVisits()[1].visit_row.visit_time);
    EXPECT_EQ(visit.visit_row.visit_duration, base::Seconds(20));
    EXPECT_EQ(visit.url_row.url(), "https://github.com/");
    EXPECT_EQ(visit.context_annotations.page_end_reason, 5);

    visit = visits[1];
    EXPECT_EQ(visit.visit_row.visit_id, 1);
    EXPECT_EQ(visit.visit_row.visit_time,
              GetHardcodedTestVisits()[0].visit_row.visit_time);
    EXPECT_EQ(visit.visit_row.visit_duration, base::Milliseconds(5600));
    EXPECT_EQ(visit.url_row.url(), "https://google.com/");
    EXPECT_EQ(visit.context_annotations.page_end_reason, 3);

    if (!for_keywords) {
      visit = visits[2];
      EXPECT_EQ(visit.visit_row.visit_id, 4);
    }

    // TODO(tommycli): Add back visit.referring_visit_id() check after updating
    //  the HistoryService test methods to support that field.
  }

 protected:
  base::test::TaskEnvironment task_environment_;

  // Used to construct a `HistoryClustersService`.
  base::ScopedTempDir history_dir_;
  std::unique_ptr<history::HistoryService> history_service_;

  std::unique_ptr<base::test::ScopedFeatureList> scoped_feature_list_;

  std::unique_ptr<HistoryClustersService> history_clusters_service_;
  std::unique_ptr<HistoryClustersServiceTestApi>
      history_clusters_service_test_api_;

  // Non-owning pointer. The actual owner is `history_clusters_service_`.
  TestClusteringBackend* test_clustering_backend_;

  base::CancelableTaskTracker task_tracker_;

  // Used to verify the async callback is invoked.
  base::RunLoop run_loop_;
  base::RepeatingClosure run_loop_quit_;

  // Tracks the next available navigation ID to be associated with visits.
  int64_t next_navigation_id_ = 0;
};

TEST_F(HistoryClustersServiceTest, ClusterAndVisitSorting) {
  base::HistogramTester histogram_tester;
  AddHardcodedTestDataToHistoryService();

  history_clusters_service_->QueryClusters(
      /*query=*/"", /*begin_time=*/base::Time(), /*end_time=*/base::Time(),
      /* max_count=*/0,
      // This "expect" block is not run until after the fake response is sent
      // further down in this method.
      base::BindLambdaForTesting([&](QueryClustersResult result) {
        auto& clusters = result.clusters;
        ASSERT_EQ(clusters.size(), 2u);

        auto& visits = clusters[0].visits;
        ASSERT_EQ(visits.size(), 1u);
        EXPECT_EQ(visits[0].annotated_visit.url_row.url(),
                  "https://github.com/");
        EXPECT_FLOAT_EQ(visits[0].score, 0.1);

        visits = clusters[1].visits;
        ASSERT_EQ(visits.size(), 2u);
        EXPECT_EQ(visits[0].annotated_visit.url_row.url(),
                  "https://google.com/");
        EXPECT_FLOAT_EQ(visits[0].score, 0.9);
        EXPECT_EQ(visits[1].annotated_visit.url_row.url(),
                  "https://github.com/");
        EXPECT_FLOAT_EQ(visits[1].score, 0.5);

        run_loop_quit_.Run();
      }),
      &task_tracker_);

  AwaitAndVerifyTestClusteringBackendRequest();

  std::vector<history::Cluster> clusters;
  // This first cluster is meant to validate that the higher scoring "visit 1"
  // gets sorted to the top, even though "visit 1" is older in the hardcoded
  // test data. It's to validate the within-cluster sorting.
  clusters.push_back(
      history::Cluster(0,
                       {
                           test_clustering_backend_->GetVisitById(2, 0.5),
                           test_clustering_backend_->GetVisitById(1, 0.9),
                       },
                       {}));
  clusters.push_back(
      history::Cluster(0,
                       {
                           test_clustering_backend_->GetVisitById(2, 0.1),
                       },
                       {}));
  test_clustering_backend_->FulfillCallback(clusters);

  // Verify the callback is invoked.
  run_loop_.Run();

  history::BlockUntilHistoryProcessesPendingRequests(history_service_.get());
  histogram_tester.ExpectUniqueSample(
      "History.Clusters.Backend.NumClustersReturned",
      static_cast<int>(clusters.size()), 1);
  histogram_tester.ExpectUniqueSample(
      "History.Clusters.Backend.NumVisitsToCluster", 3, 1);
  histogram_tester.ExpectUniqueSample(
      "History.Clusters.PercentClustersFilteredByQuery", 0, 1);
  histogram_tester.ExpectTotalCount(
      "History.Clusters.Backend.GetClustersLatency", 1);
}

TEST_F(HistoryClustersServiceTest, UnflattenDuplicatesIntegrationTest) {
  AddHardcodedTestDataToHistoryService();

  history_clusters_service_->QueryClusters(
      /*query=*/"", /*begin_time=*/base::Time(), /*end_time=*/base::Time(),
      /* max_count=*/0,
      // This "expect" block is not run until after the fake response is sent
      // further down in this method.
      base::BindLambdaForTesting([&](QueryClustersResult result) {
        auto& clusters = result.clusters;
        ASSERT_EQ(clusters.size(), 1u);

        auto& visits = clusters[0].visits;
        ASSERT_EQ(visits.size(), 1u);
        EXPECT_EQ(visits[0].annotated_visit.url_row.url(),
                  "https://google.com/");
        EXPECT_EQ(visits[0].normalized_url.spec(), "https://google.com/");
        EXPECT_FLOAT_EQ(visits[0].score, 0.9);

        ASSERT_EQ(visits[0].duplicate_visits.size(), 1u);
        EXPECT_EQ(visits[0].duplicate_visits[0].annotated_visit.url_row.url(),
                  "https://github.com/");
        EXPECT_EQ(visits[0].duplicate_visits[0].normalized_url.spec(),
                  "https://github.com/");
        EXPECT_FLOAT_EQ(visits[0].duplicate_visits[0].score, 0.5);

        run_loop_quit_.Run();
      }),
      &task_tracker_);

  AwaitAndVerifyTestClusteringBackendRequest();

  std::vector<history::ClusterVisit> visits = {
      test_clustering_backend_->GetVisitById(2, 0.5),
      test_clustering_backend_->GetVisitById(1, 0.9),
  };
  visits[1].duplicate_visit_ids = {2};

  std::vector<history::Cluster> clusters;
  clusters.push_back(history::Cluster(0, visits, {}));
  test_clustering_backend_->FulfillCallback(clusters);

  // Verify the callback is invoked.
  run_loop_.Run();

  history::BlockUntilHistoryProcessesPendingRequests(history_service_.get());
}

TEST_F(HistoryClustersServiceTest, UnflattenDuplicatesUnitTest) {
  // This tests the unflatten-duplicates method in more detail as a unit test.
  history::Cluster raw_cluster;
  auto& raw_visits = raw_cluster.visits;

  // Add ten visits, numbered from 1 to 8. (1-based, just like History.)
  for (size_t i = 0; i < 8; ++i) {
    raw_visits.emplace_back();
    raw_visits[i].annotated_visit.visit_row.visit_id = i + 1;
  }

  // Collapse 1, 2, 3 into visit 4. Visits 1 and 2 have related searches.
  // Visit 3 had omnibox_url_copied == true.
  ASSERT_EQ(raw_visits[3].annotated_visit.visit_row.visit_id, 4);
  raw_visits[3].duplicate_visit_ids = {1, 2, 3};

  // Collapse 7 into visit 6.
  ASSERT_EQ(raw_visits[5].annotated_visit.visit_row.visit_id, 6);
  raw_visits[5].duplicate_visit_ids = {7};

  // Canonical visits should be {4, {1,2,3}}, {5, {}}, {6, {7}}, {8, {}}.
  auto clusters =
      history_clusters_service_->CollapseDuplicateVisits({raw_cluster});
  ASSERT_EQ(clusters.size(), 1u);
  auto& visits = clusters[0].visits;
  ASSERT_EQ(visits.size(), 4u);

  // Visit 4 should have 1, 2, and 3 as duplicates.
  EXPECT_EQ(visits[0].annotated_visit.visit_row.visit_id, 4);
  ASSERT_EQ(visits[0].duplicate_visits.size(), 3u);
  EXPECT_EQ(visits[0].duplicate_visits[0].annotated_visit.visit_row.visit_id,
            1);
  EXPECT_EQ(visits[0].duplicate_visits[1].annotated_visit.visit_row.visit_id,
            2);
  EXPECT_EQ(visits[0].duplicate_visits[2].annotated_visit.visit_row.visit_id,
            3);

  EXPECT_EQ(visits[1].annotated_visit.visit_row.visit_id, 5);
  ASSERT_EQ(visits[1].duplicate_visits.size(), 0u);

  EXPECT_EQ(visits[2].annotated_visit.visit_row.visit_id, 6);
  ASSERT_EQ(visits[2].duplicate_visits.size(), 1u);
  EXPECT_EQ(visits[2].duplicate_visits[0].annotated_visit.visit_row.visit_id,
            7);

  EXPECT_EQ(visits[3].annotated_visit.visit_row.visit_id, 8);
  ASSERT_EQ(visits[3].duplicate_visits.size(), 0u);
}

TEST_F(HistoryClustersServiceTest, HardCapOnVisitsFetchedFromHistory) {
  base::test::ScopedFeatureList feature_list;
  feature_list.InitAndEnableFeatureWithParameters(
      kJourneys, {{"JourneysMaxVisitsToCluster", "20"}});

  history::ContextID context_id = reinterpret_cast<history::ContextID>(1);
  auto visit = GetHardcodedTestVisits()[0];
  for (size_t i = 0; i < 100; ++i) {
    // Visit IDs start at 1.
    visit.visit_row.visit_id = i + 1;

    history::HistoryAddPageArgs add_page_args;
    add_page_args.context_id = context_id;
    add_page_args.nav_entry_id = next_navigation_id_;
    add_page_args.url = visit.url_row.url();
    add_page_args.title = visit.url_row.title();
    add_page_args.time = visit.visit_row.visit_time;
    add_page_args.visit_source = visit.source;
    history_service_->AddPage(add_page_args);
    history_service_->UpdateWithPageEndTime(
        context_id, next_navigation_id_, visit.url_row.url(),
        visit.visit_row.visit_time + visit.visit_row.visit_duration);

    auto& incomplete_visit_context_annotations =
        history_clusters_service_->GetOrCreateIncompleteVisitContextAnnotations(
            next_navigation_id_);
    incomplete_visit_context_annotations.visit_row = visit.visit_row;
    incomplete_visit_context_annotations.url_row = visit.url_row;
    incomplete_visit_context_annotations.context_annotations =
        visit.context_annotations;
    incomplete_visit_context_annotations.status.history_rows = true;
    incomplete_visit_context_annotations.status.navigation_ended = true;
    incomplete_visit_context_annotations.status.navigation_end_signals = true;
    history_clusters_service_->CompleteVisitContextAnnotationsIfReady(
        next_navigation_id_);
    next_navigation_id_++;
  }
  history::BlockUntilHistoryProcessesPendingRequests(history_service_.get());

  history_clusters_service_->QueryClusters(
      /*query=*/"", /*begin_time=*/base::Time(), /*end_time=*/base::Time::Now(),
      /* max_count=*/0,
      base::DoNothing(),  // Only need to verify the correct request is sent.
      &task_tracker_);

  test_clustering_backend_->WaitForGetClustersCall();
  history::BlockUntilHistoryProcessesPendingRequests(history_service_.get());

  EXPECT_EQ(test_clustering_backend_->LastClusteredVisits().size(), 20U);
}

TEST_F(HistoryClustersServiceTest, QueryClustersIncompleteAndPersistedVisits) {
  // Create persisted visits 1, 2, 3.
  AddHardcodedTestDataToHistoryService();

  auto days_ago = [](int days) { return base::Time::Now() - base::Days(days); };

  // Create incomplete visits; only 3 & 4 should be returned by the query.
  AddIncompleteVisit(4, 4, days_ago(1));
  AddIncompleteVisit(0, 0, days_ago(1));  // Missing history rows.
  AddIncompleteVisit(5, 5, days_ago(90));
  AddIncompleteVisit(6, 6, days_ago(0));   // Too recent.
  AddIncompleteVisit(7, 7, days_ago(93));  // Too old.
  AddIncompleteVisit(3, 3, days_ago(90));  // Visit 3 was added to the history
                                           // database with source synched.
  AddIncompleteVisit(
      10, 10, days_ago(1),
      ui::PageTransitionFromInt(
          805306372));  // Visit 10 was added to the history database with
                        // a non-visible page transition.

  history_clusters_service_->QueryClusters(
      /*query=*/"", /*begin_time=*/base::Time(), /*end_time=*/base::Time::Now(),
      /* max_count=*/0,
      base::DoNothing(),  // Only need to verify the correct request is sent.
      &task_tracker_);

  test_clustering_backend_->WaitForGetClustersCall();
  history::BlockUntilHistoryProcessesPendingRequests(history_service_.get());

  // Persisted visits are ordered before incomplete visits. Persisted visits are
  // ordered newest first. Incomplete visits are ordered the same as they were
  // sent to the `HistoryClustersService`.
  std::vector<history::AnnotatedVisit> visits =
      test_clustering_backend_->LastClusteredVisits();
  ASSERT_EQ(visits.size(), 4u);
  EXPECT_EQ(visits[0].visit_row.visit_id, 2);
  EXPECT_EQ(visits[1].visit_row.visit_id, 1);
  EXPECT_EQ(visits[2].visit_row.visit_id, 4);
  EXPECT_EQ(visits[3].visit_row.visit_id, 5);
}

TEST_F(HistoryClustersServiceTest, QueryClustersVariousQueries) {
  base::HistogramTester histogram_tester;
  AddHardcodedTestDataToHistoryService();

  struct TestData {
    std::string query;
    const bool expect_first_cluster;
    const bool expect_second_cluster;
  } test_data[] = {
      // Empty query should get only the second, because the first is marked
      // hidden on prominent UI surfaces, including the zero query state.
      {"", false, true},
      // Non matching query should get none.
      {"non_matching_query", false, false},
      // Query matching one cluster.
      {"oran", true, false},
      // This verifies the memory doesn't flicker away as the user is typing
      // out: "red oran" one key at a time. Also tests out multi-term queries.
      {"red", true, false},
      {"red ", true, false},
      {"red o", true, false},
      {"red or", true, false},
      {"red ora", true, false},
      {"red oran", true, false},
      // Verify that we can search by URL.
      {"goog", true, false},
      // Verify we can search by page title, even mismatching case.
      {"code", true, true},
      // Verify that we match if the input query spans cluster keywords,
      // visit URLs, and visit titles.
      {"goog code apples", true, false},
  };

  for (size_t i = 0; i < base::size(test_data); ++i) {
    SCOPED_TRACE(base::StringPrintf("Testing case i=%d, query=%s", int(i),
                                    test_data[i].query.c_str()));

    base::RunLoop run_loop;
    auto run_loop_quit = run_loop.QuitClosure();

    history_clusters_service_->QueryClusters(
        test_data[i].query, /*begin_time=*/base::Time(),
        /*end_time=*/base::Time(),
        /* max_count=*/0,
        // This "expect" block is not run until after the fake response is sent
        // further down in this method.
        base::BindLambdaForTesting([&](QueryClustersResult result) {
          auto& clusters = result.clusters;
          size_t expected_size = int(test_data[i].expect_first_cluster) +
                                 int(test_data[i].expect_second_cluster);
          ASSERT_EQ(clusters.size(), expected_size);

          if (test_data[i].expect_first_cluster) {
            const auto& cluster = clusters[0];
            const auto& visits = cluster.visits;
            ASSERT_EQ(visits.size(), 2u);
            EXPECT_EQ(visits[0].annotated_visit.url_row.url(),
                      "https://github.com/");
            EXPECT_EQ(visits[0].annotated_visit.visit_row.visit_time,
                      GetHardcodedTestVisits()[1].visit_row.visit_time);
            EXPECT_EQ(visits[0].annotated_visit.url_row.title(),
                      u"Code Storage Title");
            EXPECT_FALSE(
                visits[0].annotated_visit.context_annotations.is_new_bookmark);
            EXPECT_TRUE(visits[0]
                            .annotated_visit.context_annotations
                            .is_existing_part_of_tab_group);
            EXPECT_FLOAT_EQ(visits[0].score, 0.5);

            EXPECT_EQ(visits[1].annotated_visit.url_row.url(),
                      "https://google.com/");
            EXPECT_EQ(visits[1].annotated_visit.visit_row.visit_time,
                      GetHardcodedTestVisits()[0].visit_row.visit_time);
            EXPECT_EQ(visits[1].annotated_visit.url_row.title(),
                      u"Search Engine Title");
            EXPECT_TRUE(
                visits[1].annotated_visit.context_annotations.is_new_bookmark);
            EXPECT_FALSE(visits[1]
                             .annotated_visit.context_annotations
                             .is_existing_part_of_tab_group);
            EXPECT_FLOAT_EQ(visits[1].score, 0.5);

            ASSERT_EQ(cluster.keywords.size(), 2u);
            EXPECT_EQ(cluster.keywords[0], u"apples");
            EXPECT_EQ(cluster.keywords[1], u"Red Oranges");
          }

          if (test_data[i].expect_second_cluster) {
            const auto& cluster =
                test_data[i].expect_first_cluster ? clusters[1] : clusters[0];
            const auto& visits = cluster.visits;
            ASSERT_EQ(visits.size(), 1u);
            EXPECT_EQ(visits[0].annotated_visit.url_row.url(),
                      "https://github.com/");
            EXPECT_EQ(visits[0].annotated_visit.visit_row.visit_time,
                      GetHardcodedTestVisits()[1].visit_row.visit_time);
            EXPECT_EQ(visits[0].annotated_visit.url_row.title(),
                      u"Code Storage Title");
            EXPECT_TRUE(cluster.keywords.empty());
          }

          run_loop_quit.Run();
        }),
        &task_tracker_);

    AwaitAndVerifyTestClusteringBackendRequest();

    std::vector<history::Cluster> clusters;
    // This first cluster with keywords is marked hidden on sensitive UI
    // surfaces. This test thus verifies that it's hidden in the zero-query
    // state, but the user can still get to it by searching for its keywords.
    clusters.push_back(
        history::Cluster(0,
                         {
                             test_clustering_backend_->GetVisitById(1),
                             test_clustering_backend_->GetVisitById(2),
                         },
                         {u"apples", u"Red Oranges"},
                         /*should_show_on_prominent_ui_surfaces=*/false));
    clusters.push_back(
        history::Cluster(0,
                         {
                             test_clustering_backend_->GetVisitById(2),
                         },
                         {},
                         /*should_show_on_prominent_ui_surfaces=*/true));
    test_clustering_backend_->FulfillCallback(clusters);

    // Verify the callback is invoked.
    run_loop.Run();
  }

  history::BlockUntilHistoryProcessesPendingRequests(history_service_.get());
  histogram_tester.ExpectBucketCount(
      "History.Clusters.Backend.NumClustersReturned", 2, base::size(test_data));
  histogram_tester.ExpectBucketCount(
      "History.Clusters.Backend.NumVisitsToCluster", 3, base::size(test_data));
  histogram_tester.ExpectBucketCount(
      "History.Clusters.PercentClustersFilteredByQuery", 0, 1);
  histogram_tester.ExpectBucketCount(
      "History.Clusters.PercentClustersFilteredByQuery", 100, 1);
  histogram_tester.ExpectBucketCount(
      "History.Clusters.PercentClustersFilteredByQuery", 50,
      base::size(test_data) - 2);
  histogram_tester.ExpectTotalCount(
      "History.Clusters.Backend.GetClustersLatency", base::size(test_data));
}

TEST_F(HistoryClustersServiceTest, CompleteVisitContextAnnotationsIfReady) {
  auto test = [&](RecordingStatus status, bool expected_complete) {
    auto& incomplete_visit_context_annotations =
        history_clusters_service_->GetOrCreateIncompleteVisitContextAnnotations(
            0);
    incomplete_visit_context_annotations.url_row.set_id(1);
    incomplete_visit_context_annotations.visit_row.visit_id = 1;
    incomplete_visit_context_annotations.status = status;
    history_clusters_service_->CompleteVisitContextAnnotationsIfReady(0);
    EXPECT_NE(
        history_clusters_service_->HasIncompleteVisitContextAnnotations(0),
        expected_complete);
  };

  // Complete cases:
  {
    SCOPED_TRACE("Complete without UKM");
    test({true, true, true}, true);
  }
  {
    SCOPED_TRACE("Complete with UKM");
    test({true, true, true, true, true}, true);
  }

  // Incomplete without UKM cases:
  {
    SCOPED_TRACE("Incomplete, missing history rows");
    test({false, true}, false);
  }
  {
    SCOPED_TRACE("Incomplete, navigation hasn't ended");
    test({true}, false);
  }
  {
    SCOPED_TRACE("Incomplete, navigation end metrics haven't been recorded");
    test({true, true}, false);
  }

  // Incomplete with UKM cases:
  {
    SCOPED_TRACE("Incomplete, missing history rows");
    test({false, true, false, true, true}, false);
  }
  {
    SCOPED_TRACE("Incomplete, navigation hasn't ended");
    test({true, false, false, true, true}, false);
  }
  {
    SCOPED_TRACE("Incomplete, navigation end metrics haven't been recorded");
    test({true, true, false, true, true}, false);
  }
  {
    SCOPED_TRACE("Incomplete, UKM page end missing");
    test({true, true, true, true, false}, false);
  }

  auto test_dcheck = [&](RecordingStatus status) {
    auto& incomplete_visit_context_annotations =
        history_clusters_service_->GetOrCreateIncompleteVisitContextAnnotations(
            0);
    incomplete_visit_context_annotations.url_row.set_id(1);
    incomplete_visit_context_annotations.visit_row.visit_id = 1;
    incomplete_visit_context_annotations.status = status;
    EXPECT_DCHECK_DEATH(
        history_clusters_service_->CompleteVisitContextAnnotationsIfReady(0));
    EXPECT_TRUE(
        history_clusters_service_->HasIncompleteVisitContextAnnotations(0));
  };

  // Impossible cases:
  {
    SCOPED_TRACE(
        "Impossible, navigation end signals recorded before navigation ended");
    test_dcheck({true, false, true});
  }
  {
    SCOPED_TRACE(
        "Impossible, navigation end signals recorded before history rows");
    test_dcheck({false, true, true});
  }
  {
    SCOPED_TRACE("Impossible, unexpected UKM page end recorded");
    test_dcheck({false, false, false, false, true});
  }
}

TEST_F(HistoryClustersServiceTest,
       CompleteVisitContextAnnotationsIfReadyWhenFeatureDisabled) {
  // When the feature is disabled, the `IncompleteVisitContextAnnotations`
  // should be removed but not added to visits.
  base::test::ScopedFeatureList feature_list;
  feature_list.InitWithFeatures(
      /*enabled_features=*/{}, /*disabled_features=*/{
          kJourneys,
          kPersistContextAnnotationsInHistoryDb,
      });

  auto& incomplete_visit_context_annotations =
      history_clusters_service_->GetOrCreateIncompleteVisitContextAnnotations(
          0);
  incomplete_visit_context_annotations.url_row.set_id(1);
  incomplete_visit_context_annotations.visit_row.visit_id = 1;
  incomplete_visit_context_annotations.status = {true, true, true};
  history_clusters_service_->CompleteVisitContextAnnotationsIfReady(0);
  EXPECT_FALSE(
      history_clusters_service_->HasIncompleteVisitContextAnnotations(0));
}

TEST_F(HistoryClustersServiceTest,
       CompleteVisitContextAnnotationsIfReadyWhenFeatureEnabled) {
  // When the feature is enabled, the `IncompleteVisitContextAnnotations`
  // should be removed and added to visits.
  auto& incomplete_visit_context_annotations =
      history_clusters_service_->GetOrCreateIncompleteVisitContextAnnotations(
          0);
  incomplete_visit_context_annotations.url_row.set_id(1);
  incomplete_visit_context_annotations.visit_row.visit_id = 1;
  incomplete_visit_context_annotations.status = {true, true, true};
  history_clusters_service_->CompleteVisitContextAnnotationsIfReady(0);
  EXPECT_FALSE(
      history_clusters_service_->HasIncompleteVisitContextAnnotations(0));
}

TEST_F(HistoryClustersServiceTest, DoesQueryMatchAnyCluster) {
  AddHardcodedTestDataToHistoryService();

  // Verify that initially, the test keyword doesn't match anything, but this
  // query should have kicked off a cache population request.
  EXPECT_FALSE(history_clusters_service_->DoesQueryMatchAnyCluster("apples"));

  // Providing the response and running the task loop should populate the cache.
  // This will also verify that visits older than 30 days are not included for
  // keyword requests.
  AwaitAndVerifyTestClusteringBackendRequest(true);

  std::vector<history::Cluster> clusters;
  clusters.push_back(
      history::Cluster(0,
                       {
                           test_clustering_backend_->GetVisitById(1),
                           test_clustering_backend_->GetVisitById(2),
                       },
                       {u"apples", u"oranges"},
                       /*should_show_on_prominent_ui_surfaces=*/true));
  clusters.push_back(
      history::Cluster(0,
                       {
                           test_clustering_backend_->GetVisitById(1),
                           test_clustering_backend_->GetVisitById(2),
                       },
                       {u"sensitive"},
                       /*should_show_on_prominent_ui_surfaces=*/false));
  clusters.push_back(
      history::Cluster(0,
                       {
                           test_clustering_backend_->GetVisitById(1),
                       },
                       {u"singlevisit"},
                       /*should_show_on_prominent_ui_surfaces=*/true));
  test_clustering_backend_->FulfillCallback(clusters);

  // Now the exact query should match the populated cache.
  EXPECT_TRUE(history_clusters_service_->DoesQueryMatchAnyCluster("apples"));

  // Check that clusters that shouldn't be shown on prominent UI surfaces don't
  // have their keywords inserted into the keyword bag.
  EXPECT_FALSE(
      history_clusters_service_->DoesQueryMatchAnyCluster("sensitive"));

  // Ignore clusters with fewer than two visits.
  EXPECT_FALSE(
      history_clusters_service_->DoesQueryMatchAnyCluster("singlevisit"));

  // Too-short queries rejected.
  EXPECT_FALSE(history_clusters_service_->DoesQueryMatchAnyCluster("ap"));

  // Non-exact matches are rejected too.
  EXPECT_FALSE(history_clusters_service_->DoesQueryMatchAnyCluster("appl"));

  // Adding a second non-exact query word also should make it no longer match.
  EXPECT_FALSE(
      history_clusters_service_->DoesQueryMatchAnyCluster("apples oran"));

  // Multi-word queries with all exact matches should still work.
  EXPECT_TRUE(
      history_clusters_service_->DoesQueryMatchAnyCluster("apples oranges"));

  // Deleting a history entry should clear the keyword cache.
  history_service_->DeleteURLs({GURL{"https://google.com/"}});
  history::BlockUntilHistoryProcessesPendingRequests(history_service_.get());
  EXPECT_FALSE(history_clusters_service_->DoesQueryMatchAnyCluster("apples"));

  // The keyword cache should be repopulated.
  test_clustering_backend_->WaitForGetClustersCall();
  test_clustering_backend_->FulfillCallback(clusters);
  EXPECT_TRUE(history_clusters_service_->DoesQueryMatchAnyCluster("apples"));
  history::BlockUntilHistoryProcessesPendingRequests(history_service_.get());
}

}  // namespace

}  // namespace history_clusters
