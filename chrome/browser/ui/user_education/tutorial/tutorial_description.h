// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_USER_EDUCATION_TUTORIAL_TUTORIAL_DESCRIPTION_H_
#define CHROME_BROWSER_UI_USER_EDUCATION_TUTORIAL_TUTORIAL_DESCRIPTION_H_

#include <string>
#include <vector>

#include "third_party/abseil-cpp/absl/types/optional.h"
#include "ui/base/interaction/element_identifier.h"
#include "ui/base/interaction/interaction_sequence.h"

// A Struct that provides all of the data necessary to construct a Tutorial.
// A Tutorial Description is a list of Steps for a tutorial. Each step has info
// for constructing the InteractionSequence::Step from the
// TutorialDescription::Step.
struct TutorialDescription {
  TutorialDescription();
  ~TutorialDescription();
  TutorialDescription(const TutorialDescription& description);

  struct Step {
    enum Arrow {
      NONE,
      TOP,
      BOTTOM,
      TOP_HORIZONTAL,
      CENTER_HORIZONTAL,
      BOTTOM_HORIZONTAL,
    };

    Step();
    Step(absl::optional<std::u16string> title_text_,
         absl::optional<std::u16string> body_text_,
         ui::InteractionSequence::StepType step_type_,
         ui::ElementIdentifier element_id_,
         Arrow arrow_,
         absl::optional<bool> must_remain_visible_ = absl::nullopt);
    Step(const Step& step);
    Step& operator=(const Step& step) = default;
    ~Step();

    absl::optional<std::u16string> title_text;

    // The text to to populated in the bubble.
    absl::optional<std::u16string> body_text;

    // the step type for InteractionSequence::Step.
    ui::InteractionSequence::StepType step_type;

    // the element used by interaction sequence to observe and attach a bubble.
    ui::ElementIdentifier element_id;

    // the positioning of the bubble arrow
    Arrow arrow;

    // Should the element remain visible through the entire step, this should be
    // set to false for hidden steps and for shown steps that precede hidden
    // steps on the same element. if left empty the interaction sequence will
    // decide what its value should be based on the generated
    // InteractionSequence::StepBuilder
    absl::optional<bool> must_remain_visible;

    // returns true iff all of the required parameters exist to display a
    // bubble.
    bool ShouldShowBubble() const;
  };

  // the list of TutorialDescription steps
  std::vector<Step> steps;
};

#endif  // CHROME_BROWSER_UI_USER_EDUCATION_TUTORIAL_TUTORIAL_DESCRIPTION_H_
