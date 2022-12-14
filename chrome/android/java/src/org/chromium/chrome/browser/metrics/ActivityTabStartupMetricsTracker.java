// Copyright 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.metrics;

import android.os.SystemClock;

import org.chromium.base.ObserverList;
import org.chromium.base.ThreadUtils;
import org.chromium.base.metrics.RecordHistogram;
import org.chromium.base.supplier.ObservableSupplier;
import org.chromium.chrome.browser.paint_preview.StartupPaintPreviewHelper;
import org.chromium.chrome.browser.paint_preview.StartupPaintPreviewMetrics.PaintPreviewMetricsObserver;
import org.chromium.chrome.browser.tab.Tab;
import org.chromium.chrome.browser.tabmodel.TabModelSelector;
import org.chromium.chrome.browser.tabmodel.TabModelSelectorTabObserver;
import org.chromium.components.embedder_support.util.UrlUtilities;
import org.chromium.content_public.browser.NavigationHandle;
import org.chromium.content_public.browser.WebContents;
import org.chromium.url.GURL;

/**
 * Tracks the first navigation and first contentful paint events for a tab within an activity during
 * startup.
 */
public class ActivityTabStartupMetricsTracker {
    private static final String UMA_HISTOGRAM_TABBED_SUFFIX = ".Tabbed";

    /** Observer for startup metrics. */
    public interface Observer {
        /**
         * Called when the initial navigation upon startup is started. This will be fired at most
         * once.
         */
        void onFirstNavigationStart();

        /**
         * Called when recording first visible content. This will be fired at most once.
         */
        void onFirstVisibleContent();

        /**
         * Called when recording first navigation commit. This will be fired at most once.
         */
        void onFirstNavigationCommit();

        /**
         * Called when recording first contentful paint. This will be fired at most once.
         */
        void onFirstContentfulPaint();
    }

    private static ObserverList<Observer> sObservers;

    /** Adds an observer. */
    public static boolean addObserver(Observer observer) {
        ThreadUtils.assertOnUiThread();
        if (sObservers == null) sObservers = new ObserverList<>();
        return sObservers.addObserver(observer);
    }

    /** Removes an observer. */
    public static boolean removeObserver(Observer observer) {
        ThreadUtils.assertOnUiThread();
        if (sObservers == null) return false;
        return sObservers.removeObserver(observer);
    }

    private class PageLoadMetricsObserverImpl implements PageLoadMetrics.Observer {
        private static final long NO_NAVIGATION_ID = -1;

        private long mNavigationId = NO_NAVIGATION_ID;
        private boolean mShouldRecordHistograms;
        private boolean mInvokedOnFirstNavigationStart;

        @Override
        public void onNewNavigation(WebContents webContents, long navigationId,
                boolean isFirstNavigationInWebContents) {
            if (mNavigationId != NO_NAVIGATION_ID) return;

            mNavigationId = navigationId;
            mShouldRecordHistograms = mShouldTrackStartupMetrics;

            // Only notify observers of the initial navigation in the case where we will also record
            // first contentful paint for this navigation.
            if (!mInvokedOnFirstNavigationStart && mShouldRecordHistograms) {
                for (Observer observer : sObservers) {
                    observer.onFirstNavigationStart();
                }
                mInvokedOnFirstNavigationStart = true;
            }
        }

        @Override
        public void onFirstContentfulPaint(WebContents webContents, long navigationId,
                long navigationStartTick, long firstContentfulPaintMs) {
            if (navigationId != mNavigationId || !mShouldRecordHistograms) return;

            recordFirstContentfulPaint(navigationStartTick / 1000 + firstContentfulPaintMs);
        }

        void resetMetricsRecordingStateForInitialNavigation() {
            // NOTE: |mInvokedOnFirstNavigationStart| is intentionally not reset to avoid duplicate
            // observer notifications.
            mNavigationId = NO_NAVIGATION_ID;
            mShouldRecordHistograms = false;
        }
    };

    private final long mActivityStartTimeMs;

    // Event duration recorded from the |mActivityStartTimeMs|.
    private long mFirstCommitTimeMs;
    private String mHistogramSuffix;
    private TabModelSelectorTabObserver mTabModelSelectorTabObserver;
    private PageLoadMetricsObserverImpl mPageLoadMetricsObserver;
    private boolean mShouldTrackStartupMetrics;
    private boolean mFirstVisibleContentRecorded;
    private boolean mVisibleContentRecorded;

    public ActivityTabStartupMetricsTracker(
            ObservableSupplier<TabModelSelector> tabModelSelectorSupplier) {
        mActivityStartTimeMs = SystemClock.uptimeMillis();
        tabModelSelectorSupplier.addObserver((selector) -> registerObservers(selector));
    }

    private void registerObservers(TabModelSelector tabModelSelector) {
        if (!mShouldTrackStartupMetrics) return;
        mTabModelSelectorTabObserver =
                new TabModelSelectorTabObserver(tabModelSelector) {
                    private boolean mIsFirstPageLoadStart = true;

                    @Override
                    public void onPageLoadStarted(Tab tab, GURL url) {
                        // Discard startup navigation measurements when the user interfered and
                        // started the 2nd navigation (in activity lifetime) in parallel.
                        if (!mIsFirstPageLoadStart) {
                            mShouldTrackStartupMetrics = false;
                        } else {
                            mIsFirstPageLoadStart = false;
                        }
                    }

                    @Override
                    public void onDidFinishNavigation(Tab tab, NavigationHandle navigation) {
                        boolean isTrackedPage = navigation.hasCommitted()
                                && navigation.isInPrimaryMainFrame() && !navigation.isErrorPage()
                                && !navigation.isSameDocument()
                                && !navigation.isFragmentNavigation()
                                && UrlUtilities.isHttpOrHttps(navigation.getUrl());
                        registerFinishNavigation(isTrackedPage);
                    }
                };
        mPageLoadMetricsObserver = new PageLoadMetricsObserverImpl();
        PageLoadMetrics.addObserver(mPageLoadMetricsObserver);
    }

    /**
     * Register an observer to be notified on the first paint of a paint preview if present.
     * @param startupPaintPreviewHelper the helper to register the observer to.
     */
    public void registerPaintPreviewObserver(StartupPaintPreviewHelper startupPaintPreviewHelper) {
        startupPaintPreviewHelper.addMetricsObserver(new PaintPreviewMetricsObserver() {
            @Override
            public void onFirstPaint(long durationMs) {
                recordFirstVisibleContent(durationMs);
                recordVisibleContent(durationMs);
            }
        });
    }

    /**
     * Invoked when a tab preloaded at startup is dropped rather than taken, meaning that a new tab
     * will need to be created to do the initial navigation. Resets state related to observation of
     * the initial navigation to ensure that loading startup metrics are properly recorded in this
     * case. Note that it is not necessary to reset the state of |mTabModelSelectorTabObserver| in
     * this case, as that observer tracks state starting only from the addition of a tab to the tab
     * model, which by definition has not yet occurred at this point.
     */
    public void onStartupTabPreloadDropped() {
        // Note that observers are not created in all contexts (e.g., CCT).
        if (mPageLoadMetricsObserver == null) return;

        mPageLoadMetricsObserver.resetMetricsRecordingStateForInitialNavigation();
    }

    /**
     * Marks that startup metrics should be tracked with the |histogramSuffix|.
     * Must only be called on the UI thread.
     */
    public void trackStartupMetrics(String histogramSuffix) {
        mHistogramSuffix = histogramSuffix;
        mShouldTrackStartupMetrics = true;
    }

    /**
     * Cancels tracking the startup metrics.
     * Must only be called on the UI thread.
     */
    public void cancelTrackingStartupMetrics() {
        if (!mShouldTrackStartupMetrics) return;

        // Ensure we haven't tried to record metrics already.
        assert mFirstCommitTimeMs == 0;

        mHistogramSuffix = null;
        mShouldTrackStartupMetrics = false;
    }

    public void destroy() {
        mShouldTrackStartupMetrics = false;
        if (mTabModelSelectorTabObserver != null) {
            mTabModelSelectorTabObserver.destroy();
            mTabModelSelectorTabObserver = null;
        }

        if (mPageLoadMetricsObserver != null) {
            PageLoadMetrics.removeObserver(mPageLoadMetricsObserver);
            mPageLoadMetricsObserver = null;
        }
    }

    /**
     * Registers the fact that a navigation has finished. Based on this fact, may discard recording
     * histograms later.
     */
    private void registerFinishNavigation(boolean isTrackedPage) {
        if (!mShouldTrackStartupMetrics) return;

        if (isTrackedPage && UmaUtils.hasComeToForeground() && !UmaUtils.hasComeToBackground()) {
            mFirstCommitTimeMs = SystemClock.uptimeMillis() - mActivityStartTimeMs;
            RecordHistogram.recordMediumTimesHistogram(
                    "Startup.Android.Cold.TimeToFirstNavigationCommit" + mHistogramSuffix,
                    mFirstCommitTimeMs);
            if (mHistogramSuffix.equals(UMA_HISTOGRAM_TABBED_SUFFIX)) {
                recordFirstVisibleContent(mFirstCommitTimeMs);
            }

            for (Observer observer : sObservers) {
                observer.onFirstNavigationCommit();
            }
        }
        mShouldTrackStartupMetrics = false;
    }

    /**
     * Record the First Contentful Paint time.
     *
     * @param firstContentfulPaintMs timestamp in uptime millis.
     */
    private void recordFirstContentfulPaint(long firstContentfulPaintMs) {
        // First commit time histogram should be recorded before this one. We should discard a
        // record if the first commit time wasn't recorded.
        if (mFirstCommitTimeMs == 0) return;

        if (UmaUtils.hasComeToForeground() && !UmaUtils.hasComeToBackground()) {
            long durationMs = firstContentfulPaintMs - mActivityStartTimeMs;
            RecordHistogram.recordMediumTimesHistogram(
                    "Startup.Android.Cold.TimeToFirstContentfulPaint" + mHistogramSuffix,
                    durationMs);
            if (mHistogramSuffix.equals(UMA_HISTOGRAM_TABBED_SUFFIX)) {
                recordVisibleContent(durationMs);
            }

            for (Observer observer : sObservers) {
                observer.onFirstContentfulPaint();
            }
        }
        // This is the last event we track, so destroy this tracker and remove observers.
        destroy();
    }

    /**
     * Record the time to first visible content. This metric acts as the Clank cold start guardian
     * metric. Reports the minimum value of
     * Startup.Android.Cold.TimeToFirstNavigationCommit.Tabbed and
     * Browser.PaintPreview.TabbedPlayer.TimeToFirstBitmap.
     *
     * @param durationMs duration in millis.
     */
    private void recordFirstVisibleContent(long durationMs) {
        if (mFirstVisibleContentRecorded) return;

        mFirstVisibleContentRecorded = true;
        RecordHistogram.recordMediumTimesHistogram(
                "Startup.Android.Cold.TimeToFirstVisibleContent", durationMs);

        for (Observer observer : sObservers) {
            observer.onFirstVisibleContent();
        }
    }

    /**
     * Record the first Visible Content time.
     * This metric reports the minimum value of
     * Startup.Android.Cold.TimeToFirstContentfulPaint.Tabbed and
     * Browser.PaintPreview.TabbedPlayer.TimeToFirstBitmap.
     *
     * @param durationMs duration in millis.
     */
    private void recordVisibleContent(long durationMs) {
        if (mVisibleContentRecorded) return;

        mVisibleContentRecorded = true;
        RecordHistogram.recordMediumTimesHistogram(
                "Startup.Android.Cold.TimeToVisibleContent", durationMs);
    }
}
