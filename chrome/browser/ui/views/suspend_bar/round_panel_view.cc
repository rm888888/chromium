// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/views/suspend_bar/round_panel_view.h"

#include <memory>

#include "base/feature_list.h"
#include "base/memory/raw_ptr.h"
#include "chrome/browser/themes/theme_properties.h"
#include "chrome/browser/ui/layout_constants.h"
#include "chrome/browser/ui/ui_features.h"
#include "chrome/browser/ui/views/frame/top_container_background.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/base/theme_provider.h"
#include "ui/compositor/layer.h"
#include "ui/gfx/color_palette.h"
#include "ui/gfx/color_utils.h"
#include "ui/gfx/geometry/dip_util.h"
#include "ui/gfx/geometry/insets_conversions.h"
#include "ui/gfx/scoped_canvas.h"
#include "ui/views/border.h"
#include "ui/views/controls/separator.h"
#include "ui/views/layout/fill_layout.h"
#include "ui/views/layout/layout_provider.h"
#include "ui/views/view_observer.h"

namespace {

// This thickness includes the solid-color background and the inner round-rect
// border-color stroke. It does not include the outer-color separator.
//    constexpr int kBorderThickness = 16 + views::Separator::kThickness;
//update on 20220714
    constexpr int kBorderThickness = views::Separator::kThickness;
//
// This is how many units of the toolbar are essentially expected to be
// background.
//    constexpr int kOverlapFromToolbar = 4;

// We want the border to visually look like kBorderThickness units on all sides.
// On the top side, background is drawn on top of the top-content separator and
// some units of background inside the toolbar (or bookmarks bar) itself.
// Subtract both of those to not get visually-excessive padding.
//    constexpr auto kBorderInsets = gfx::Insets(
//            kBorderThickness - views::Separator::kThickness - kOverlapFromToolbar,
//            kBorderThickness,
//            kBorderThickness,
//            kBorderThickness);
//update on 20220714
    constexpr auto kBorderInsets = gfx::Insets(
            kBorderThickness,
            kBorderThickness,
            kBorderThickness,
            kBorderThickness);
//
// This border paints the toolbar color around the side panel content and draws
// a roundrect viewport around the side panel content.
    class SidePanelBorder : public views::Border {
    public:
        explicit SidePanelBorder(BrowserView* browser_view,int radius)
                : Border(gfx::kPlaceholderColor), browser_view_(browser_view),radius_(radius) {}

        SidePanelBorder(const SidePanelBorder&) = delete;
        SidePanelBorder& operator=(const SidePanelBorder&) = delete;

        // views::Border:
        void Paint(const views::View& view, gfx::Canvas* canvas) override {
            // Undo DSF so that we can be sure to draw an integral number of pixels for
            // the border. Integral scale factors should be unaffected by this, but for
            // fractional scale factors this ensures sharp lines.
            gfx::ScopedCanvas scoped(canvas);
            float dsf = canvas->UndoDeviceScaleFactor();

            gfx::RectF scaled_bounds = gfx::ConvertRectToPixels(
                    view.GetLocalBounds(), view.layer()->device_scale_factor());

            const float corner_radius =
                    view.GetLayoutProvider()->GetCornerRadiusMetric(
                            views::Emphasis::kMedium, view.GetContentsBounds().size()) *
                    dsf+radius_;
            gfx::InsetsF insets_in_pixels(
                    gfx::ToFlooredInsets(gfx::ConvertInsetsToPixels(GetInsets(), dsf)));
            scaled_bounds.Inset(insets_in_pixels);
            SkRRect rect = SkRRect::MakeRectXY(gfx::RectFToSkRect(scaled_bounds),
                                               corner_radius, corner_radius);

            // Clip out the content area from the background about to be painted.
            canvas->sk_canvas()->clipRRect(rect, SkClipOp::kDifference, true);

            // Draw the top-container background.
            {
                // Redo device-scale factor, the theme background is drawn in DIPs. Note
                // that the clip area above is in pixels, hence the
                // UndoDeviceScaleFactor() call before this.
                gfx::ScopedCanvas scoped(canvas);
                canvas->Scale(dsf, dsf);

//                TopContainerBackground::PaintBackground(
//                        canvas, &view, browser_view_,
//                        /*translate_view_coordinates=*/true);
                //update on 20220523
                TopContainerBackground::PaintBackground(
                        canvas, &view, browser_view_,
                        /*translate_view_coordinates=*/true,1);
            }

            // Paint the inner border around RoundPanelView content.
            const float stroke_thickness = views::Separator::kThickness * dsf;

            cc::PaintFlags flags;
            flags.setStrokeWidth(stroke_thickness);
//            flags.setColor(view.GetThemeProvider()->GetColor(
//                    ThemeProperties::COLOR_TOOLBAR));
            //update on 20220524
//flags.setColor(SkColorSetARGB(255,235,226,238));
            flags.setColor(view.GetThemeProvider()->GetColor(
                    ThemeProperties::COLOR_FRAME_ACTIVE));
//
            flags.setStyle(cc::PaintFlags::kStroke_Style);
            flags.setAntiAlias(true);

            // Outset half of the stroke thickness so that it's painted fully on the
            // outside of the clipping region.
            scaled_bounds.Inset(gfx::InsetsF(-stroke_thickness / 2));
            canvas->DrawRoundRect(scaled_bounds, corner_radius, flags);
        }

        gfx::Insets GetInsets() const override {
            // This additional inset matches the growth inside BorderView::Layout()
            // below to let us paint on top of the toolbar separator. This additional
            // inset is outside the RoundPanelView itself, but not outside the BorderView.
            return kBorderInsets +
                   gfx::Insets(views::Separator::kThickness, 0, 0, 0);
        }
        gfx::Size GetMinimumSize() const override {
            return gfx::Size(GetInsets().width(), GetInsets().height());
        }

    private:
        const raw_ptr<BrowserView> browser_view_;
        int radius_;
    };

    class BorderView : public views::View {
    public:
        explicit BorderView(BrowserView* browser_view,int radius) {
            SetVisible(false);
            SetBorder(std::make_unique<SidePanelBorder>(browser_view,radius));
            // Don't allow the view to process events.
            //SetCanProcessEventsWithinSubtree(false);
        }

        void Layout() override {
            // Let BorderView grow slightly taller so that it overlaps the divider into
            // the toolbar or bookmarks bar above it.
            gfx::Rect bounds = parent()->GetLocalBounds();
            bounds.Inset(gfx::Insets(-views::Separator::kThickness, 0, 0, 0));

            SetBoundsRect(bounds);
        }

        void OnThemeChanged() override {
            SchedulePaint();
            View::OnThemeChanged();
        }
    };

}  // namespace

RoundPanelView::RoundPanelView(BrowserView* browser_view,int type,int radius)
        : type_(type),border_view_(AddChildView(std::make_unique<BorderView>(browser_view,radius))) {
    SetVisible(false);
    SetLayoutManager(std::make_unique<views::FillLayout>());

    // TODO(pbos): Reconsider if SetPanelWidth() should add borders, if so move
    // accounting for the border into SetPanelWidth(), otherwise remove this TODO.
    //constexpr int kDefaultWidth = 320;
    //SetPanelWidth(kDefaultWidth + kBorderInsets.width());

    SetBorder(views::CreateEmptyBorder(kBorderInsets));
    AddObserver(this);
}

RoundPanelView::~RoundPanelView() {
    RemoveObserver(this);
}

void RoundPanelView::SetPanelWidth(int width) {
    // Only the width is used by BrowserViewLayout.
    SetPreferredSize(gfx::Size(width, 1));
}

void RoundPanelView::ChildVisibilityChanged(View* child) {
    UpdateVisibility();
}

void RoundPanelView::OnChildViewAdded(View* observed_view, View* child) {
    UpdateVisibility();
    // Reorder `border_view_` to be last so that it gets painted on top, even if
    // an added child also paints to a layer.
    if(type_ == 1) {
        ReorderChildView(border_view_, -1);
        //ReorderChildView(child, -1);
    }
    else
    //update on 20220608
    ReorderChildView(child, -1);
    //
}

void RoundPanelView::OnChildViewRemoved(View* observed_view, View* child) {
    UpdateVisibility();
}

void RoundPanelView::UpdateVisibility() {
    bool any_child_visible = false;
    // TODO(pbos): Iterate content instead. Requires moving the owned pointer out
    // of owned contents before resetting it.
    for (const auto* view : children()) {
        if (view == border_view_)
            continue;

        if (view->GetVisible()) {
            any_child_visible = true;
            break;
        }
    }
    // Make sure the border visibility matches the side panel. Also dynamically
    // create and destroy the layer to reclaim memory and avoid painting and
    // compositing this border when it's not showing. See
    // https://crbug.com/1269090.
    // TODO(pbos): Should layer visibility/painting be automatically tied to
    // parent visibility? I.e. the difference between GetVisible() and IsDrawn().
    if (any_child_visible != border_view_->GetVisible()) {
        border_view_->SetVisible(any_child_visible);
        if (any_child_visible) {
            border_view_->SetPaintToLayer();
            border_view_->layer()->SetFillsBoundsOpaquely(false);
        } else {
            border_view_->DestroyLayer();
        }
    }
    SetVisible(any_child_visible);
}

BEGIN_METADATA(RoundPanelView, views::View)
END_METADATA