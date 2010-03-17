// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_COMMON_VIEW_TYPES_H_
#define CHROME_COMMON_VIEW_TYPES_H_

#include "base/basictypes.h"

// Indicates different types of views
class ViewType {
 public:
  enum Type {
    INVALID,
    TAB_CONTENTS,
    EXTENSION_TOOLSTRIP,
    EXTENSION_MOLE,
    EXTENSION_BACKGROUND_PAGE,
    EXTENSION_POPUP,
    EXTENSION_INFOBAR,
    DEV_TOOLS_UI,
    INTERSTITIAL_PAGE,
  };

  // Constant strings corresponding to the Type enumeration values.  Used
  // when converting JS arguments.
  static const char* kTabContents;
  static const char* kToolstrip;
  static const char* kMole;
  static const char* kBackgroundPage;
  static const char* kPopup;
  static const char* kInfobar;
  static const char* kAll;

  // Return true if a view type should automatically resize to fit its contents.
  static bool ShouldAutoResize(Type);

 private:
  // This class is for scoping only, so you shouldn't create an instance of it.
  ViewType() {}

  DISALLOW_COPY_AND_ASSIGN(ViewType);
};

#endif  // CHROME_COMMON_VIEW_TYPES_H_
