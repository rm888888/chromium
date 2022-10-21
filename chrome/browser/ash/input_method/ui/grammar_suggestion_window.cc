// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ash/input_method/ui/grammar_suggestion_window.h"

#include "chrome/browser/ash/input_method/ui/border_factory.h"
#include "chrome/browser/ash/input_method/ui/suggestion_details.h"
#include "ui/base/metadata/metadata_impl_macros.h"
#include "ui/gfx/color_palette.h"
#include "ui/gfx/paint_vector_icon.h"
#include "ui/views/bubble/bubble_border.h"
#include "ui/views/bubble/bubble_frame_view.h"
#include "ui/views/layout/box_layout.h"
#include "ui/views/layout/layout_provider.h"
#include "ui/views/vector_icons.h"
#include "ui/wm/core/window_animations.h"

namespace ui {
namespace ime {

namespace {

constexpr SkColor kGrammarColor = gfx::kGoogleGrey700;
constexpr int kGrammarPaddingSize = 4;
constexpr float kSuggestionBorderRadius = 2;
// Large enough to make the background a circle.
constexpr float kIconBorderRadius = 100;
constexpr int kWindowOffsetY = -4;
const char16_t kSuggestionButtonAccessibleName[] = u"grammar suggestion button";
const char16_t kIgnoreButtonAccessibleName[] = u"ignore button";

bool ShouldHighlight(const views::Button& button) {
  return button.GetState() == views::Button::STATE_HOVERED ||
         button.GetState() == views::Button::STATE_PRESSED;
}

}  // namespace

GrammarSuggestionWindow::GrammarSuggestionWindow(gfx::NativeView parent,
                                                 AssistiveDelegate* delegate)
    : delegate_(delegate) {
  DialogDelegate::SetButtons(ui::DIALOG_BUTTON_NONE);
  SetCanActivate(false);
  DCHECK(parent);
  set_parent_window(parent);
  set_margins(gfx::Insets(kGrammarPaddingSize, kGrammarPaddingSize,
                          kGrammarPaddingSize, kGrammarPaddingSize));

  SetArrow(views::BubbleBorder::Arrow::BOTTOM_LEFT);
  SetLayoutManager(std::make_unique<views::BoxLayout>(
      views::BoxLayout::Orientation::kHorizontal));

  suggestion_button_ =
      AddChildView(std::make_unique<SuggestionView>(base::BindRepeating(
          &AssistiveDelegate::AssistiveWindowButtonClicked,
          base::Unretained(delegate_),
          AssistiveWindowButton{
              .id = ui::ime::ButtonId::kSuggestion,
              .window_type =
                  ui::ime::AssistiveWindowType::kGrammarSuggestion})));
  suggestion_button_->SetBackground(nullptr);
  suggestion_button_->SetAccessibleName(kSuggestionButtonAccessibleName);
  suggestion_button_->SetFocusBehavior(
      views::View::FocusBehavior::ACCESSIBLE_ONLY);
  suggestion_button_->SetVisible(true);

  ignore_button_ =
      AddChildView(std::make_unique<views::ImageButton>(base::BindRepeating(
          &AssistiveDelegate::AssistiveWindowButtonClicked,
          base::Unretained(delegate_),
          AssistiveWindowButton{
              .id = ui::ime::ButtonId::kIgnoreSuggestion,
              .window_type = ui::ime::AssistiveWindowType::kGrammarSuggestion,
          })));
  ignore_button_->SetImageHorizontalAlignment(views::ImageButton::ALIGN_CENTER);
  ignore_button_->SetImageVerticalAlignment(views::ImageButton::ALIGN_MIDDLE);
  ignore_button_->SetAccessibleName(kIgnoreButtonAccessibleName);
  ignore_button_->SetFocusBehavior(views::View::FocusBehavior::ACCESSIBLE_ONLY);
  ignore_button_->SetVisible(true);

  // Highlights buttons when they are hovered or pressed.
  const auto update_button_highlight = [](views::Button* button) {
    button->SetBackground(
        ShouldHighlight(*button)
            ? views::CreateSolidBackground(kButtonHighlightColor)
            : nullptr);
  };
  subscriptions_.insert(
      {suggestion_button_,
       suggestion_button_->AddStateChangedCallback(base::BindRepeating(
           update_button_highlight, base::Unretained(suggestion_button_)))});
  subscriptions_.insert(
      {ignore_button_,
       ignore_button_->AddStateChangedCallback(base::BindRepeating(
           update_button_highlight, base::Unretained(ignore_button_)))});
}

GrammarSuggestionWindow::~GrammarSuggestionWindow() = default;

void GrammarSuggestionWindow::OnThemeChanged() {
  ignore_button_->SetBorder(
      views::CreateEmptyBorder(views::LayoutProvider::Get()->GetInsetsMetric(
          views::INSETS_VECTOR_IMAGE_BUTTON)));

  ignore_button_->SetImage(
      views::Button::ButtonState::STATE_NORMAL,
      gfx::CreateVectorIcon(views::kCloseIcon, kGrammarColor));

  BubbleDialogDelegateView::OnThemeChanged();
}

views::Widget* GrammarSuggestionWindow::InitWidget() {
  views::Widget* widget = BubbleDialogDelegateView::CreateBubble(this);

  wm::SetWindowVisibilityAnimationTransition(widget->GetNativeView(),
                                             wm::ANIMATE_NONE);

  GetBubbleFrameView()->SetBubbleBorder(
      GetBorderForWindow(WindowBorderType::Suggestion));
  GetBubbleFrameView()->OnThemeChanged();
  return widget;
}

void GrammarSuggestionWindow::Show() {
  GetWidget()->Show();
}

void GrammarSuggestionWindow::Hide() {
  GetWidget()->Close();
}

void GrammarSuggestionWindow::SetSuggestion(const std::u16string& suggestion) {
  suggestion_button_->SetView(SuggestionDetails{
      .text = suggestion,
      .text_color = kGrammarColor,
  });
}

void GrammarSuggestionWindow::SetButtonHighlighted(
    const AssistiveWindowButton& button,
    bool highlighted) {
  if (highlighted && button.id == current_highlighted_button_id_) {
    return;
  }

  suggestion_button_->SetBackground(nullptr);
  ignore_button_->SetBackground(nullptr);

  if (highlighted) {
    switch (button.id) {
      case ButtonId::kSuggestion:
        suggestion_button_->SetBackground(views::CreateRoundedRectBackground(
            kButtonHighlightColor, kSuggestionBorderRadius));
        break;
      case ButtonId::kIgnoreSuggestion:
        ignore_button_->SetBackground(views::CreateRoundedRectBackground(
            kButtonHighlightColor, kIconBorderRadius));
        break;
      default:
        break;
    }
  }
}

void GrammarSuggestionWindow::SetBounds(gfx::Rect bounds) {
  bounds.Offset(0, kWindowOffsetY);
  SetAnchorRect(bounds);
}

SuggestionView* GrammarSuggestionWindow::GetSuggestionButtonForTesting() {
  return suggestion_button_;
}

views::Button* GrammarSuggestionWindow::GetIgnoreButtonForTesting() {
  return ignore_button_;
}

BEGIN_METADATA(GrammarSuggestionWindow, views::BubbleDialogDelegateView)
END_METADATA

}  // namespace ime
}  // namespace ui