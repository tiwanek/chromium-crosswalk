// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_EXTENSIONS_EXTENSION_ICON_MANAGER_H_
#define CHROME_BROWSER_EXTENSIONS_EXTENSION_ICON_MANAGER_H_

#include <map>
#include <set>
#include <string>

#include "base/basictypes.h"
#include "chrome/browser/extensions/image_loading_tracker.h"
#include "third_party/skia/include/core/SkBitmap.h"

class Extension;

class ExtensionIconManager : public ImageLoadingTracker::Observer {
 public:
  ExtensionIconManager();

  // Start loading the icon for the given extension.
  void LoadIcon(Extension* extension);

  // This returns a bitmap of width/height kFavIconSize, loaded either from an
  // entry specified in the extension's 'icon' section of the manifest, or a
  // default extension icon.
  const SkBitmap& GetIcon(const std::string& extension_id);

  // Removes the extension's icon from memory.
  void RemoveIcon(const std::string& extension_id);

  // Implements the ImageLoadingTracker::Observer interface.
  virtual void OnImageLoaded(SkBitmap* image, ExtensionResource resource,
                             int index);

  void set_monochrome(bool value) { monochrome_ = value; }

 private:
  // Makes sure we've done one-time initialization of the default extension icon
  // default_icon_.
  void EnsureDefaultIcon();

  // Helper function to return a copy of |src| with the proper scaling and
  // coloring.
  SkBitmap ApplyTransforms(const SkBitmap& src);

  // Used for loading extension icons.
  ImageLoadingTracker image_tracker_;

  // Maps extension id to an SkBitmap with the icon for that extension.
  std::map<std::string, SkBitmap> icons_;

  // Set of extension IDs waiting for icons to load.
  std::set<std::string> pending_icons_;

  // The default icon we'll use if an extension doesn't have one.
  SkBitmap default_icon_;

  // If true, we will desaturate the icons to make them monochromatic.
  bool monochrome_;

  DISALLOW_COPY_AND_ASSIGN(ExtensionIconManager);
};

#endif  // CHROME_BROWSER_EXTENSIONS_EXTENSION_ICON_MANAGER_H_
