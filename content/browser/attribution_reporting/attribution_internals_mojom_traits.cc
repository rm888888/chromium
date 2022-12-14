// Copyright 2021 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/attribution_reporting/attribution_internals_mojom_traits.h"

namespace mojo {

// static
bool StructTraits<content::mojom::AttributionReportIDDataView,
                  content::AttributionReport::Id>::
    Read(content::mojom::AttributionReportIDDataView data,
         content::AttributionReport::Id* out) {
  *out = content::AttributionReport::Id(data.value());
  return true;
}

}  // namespace mojo
