// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "extensions/common/permissions/socket_permission_entry.h"

#include <cstdlib>
#include <sstream>
#include <vector>

#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_split.h"
#include "base/strings/string_util.h"
#include "extensions/common/permissions/api_permission.h"
#include "extensions/common/permissions/socket_permission.h"
#include "url/url_canon.h"

namespace {

using content::SocketPermissionRequest;

const char kColon = ':';
const char kDot = '.';
const char kWildcard[] = "*";
const int kWildcardPortNumber = 0;
const int kInvalidPort = -1;

bool StartsOrEndsWithWhitespace(const std::string& str) {
  if (str.find_first_not_of(base::kWhitespaceASCII) != 0)
    return true;
  if (str.find_last_not_of(base::kWhitespaceASCII) != str.length() - 1)
    return true;
  return false;
}

}  // namespace

namespace extensions {

SocketPermissionEntry::SocketPermissionEntry()
    : pattern_(SocketPermissionRequest::NONE, std::string(), kInvalidPort),
      match_subdomains_(false) {}

SocketPermissionEntry::~SocketPermissionEntry() {}

bool SocketPermissionEntry::operator<(const SocketPermissionEntry& rhs) const {
  if (pattern_.type < rhs.pattern_.type)
    return true;
  if (pattern_.type > rhs.pattern_.type)
    return false;

  if (pattern_.host < rhs.pattern_.host)
    return true;
  if (pattern_.host > rhs.pattern_.host)
    return false;

  if (match_subdomains_ < rhs.match_subdomains_)
    return true;
  if (match_subdomains_ > rhs.match_subdomains_)
    return false;

  if (pattern_.port < rhs.pattern_.port)
    return true;
  return false;
}

bool SocketPermissionEntry::operator==(const SocketPermissionEntry& rhs) const {
  return (pattern_.type == rhs.pattern_.type) &&
         (pattern_.host == rhs.pattern_.host) &&
         (match_subdomains_ == rhs.match_subdomains_) &&
         (pattern_.port == rhs.pattern_.port);
}

bool SocketPermissionEntry::Check(
    const content::SocketPermissionRequest& request) const {
  if (pattern_.type != request.type)
    return false;

  std::string lhost = StringToLowerASCII(request.host);
  if (pattern_.host != lhost) {
    if (!match_subdomains_)
      return false;

    if (!pattern_.host.empty()) {
      // Do not wildcard part of IP address.
      url_parse::Component component(0, lhost.length());
      url_canon::RawCanonOutputT<char, 128> ignored_output;
      url_canon::CanonHostInfo host_info;
      url_canon::CanonicalizeIPAddress(
          lhost.c_str(), component, &ignored_output, &host_info);
      if (host_info.IsIPAddress())
        return false;

      // host should equal one or more chars + "." +  host_.
      int i = lhost.length() - pattern_.host.length();
      if (i < 2)
        return false;

      if (lhost.compare(i, pattern_.host.length(), pattern_.host) != 0)
        return false;

      if (lhost[i - 1] != kDot)
        return false;
    }
  }

  if (pattern_.port != request.port && pattern_.port != kWildcardPortNumber)
    return false;

  return true;
}

SocketPermissionEntry::HostType SocketPermissionEntry::GetHostType() const {
  return pattern_.host.empty()
             ? SocketPermissionEntry::ANY_HOST
             : match_subdomains_ ? SocketPermissionEntry::HOSTS_IN_DOMAINS
                                 : SocketPermissionEntry::SPECIFIC_HOSTS;
}

bool SocketPermissionEntry::IsAddressBoundType() const {
  return pattern_.type == SocketPermissionRequest::TCP_CONNECT ||
         pattern_.type == SocketPermissionRequest::TCP_LISTEN ||
         pattern_.type == SocketPermissionRequest::UDP_BIND ||
         pattern_.type == SocketPermissionRequest::UDP_SEND_TO;
}

// static
bool SocketPermissionEntry::ParseHostPattern(
    SocketPermissionRequest::OperationType type,
    const std::string& pattern,
    SocketPermissionEntry* entry) {
  std::vector<std::string> tokens;
  base::SplitStringDontTrim(pattern, kColon, &tokens);
  return ParseHostPattern(type, tokens, entry);
}

// static
bool SocketPermissionEntry::ParseHostPattern(
    SocketPermissionRequest::OperationType type,
    const std::vector<std::string>& pattern_tokens,
    SocketPermissionEntry* entry) {

  SocketPermissionEntry result;

  if (type == SocketPermissionRequest::NONE)
    return false;

  if (pattern_tokens.size() > 2)
    return false;

  result.pattern_.type = type;
  result.pattern_.port = kWildcardPortNumber;
  result.match_subdomains_ = true;

  if (pattern_tokens.size() == 0) {
    *entry = result;
    return true;
  }

  // Return an error if address is specified for permissions that don't
  // need it (such as 'resolve-host').
  if (!result.IsAddressBoundType())
    return false;

  result.pattern_.host = pattern_tokens[0];
  if (!result.pattern_.host.empty()) {
    if (StartsOrEndsWithWhitespace(result.pattern_.host))
      return false;
    result.pattern_.host = StringToLowerASCII(result.pattern_.host);

    // The first component can optionally be '*' to match all subdomains.
    std::vector<std::string> host_components;
    base::SplitString(result.pattern_.host, kDot, &host_components);
    DCHECK(!host_components.empty());

    if (host_components[0] == kWildcard || host_components[0].empty()) {
      host_components.erase(host_components.begin(),
                            host_components.begin() + 1);
    } else {
      result.match_subdomains_ = false;
    }
    result.pattern_.host = JoinString(host_components, kDot);
  }

  if (pattern_tokens.size() == 1 || pattern_tokens[1].empty() ||
      pattern_tokens[1] == kWildcard) {
    *entry = result;
    return true;
  }

  if (StartsOrEndsWithWhitespace(pattern_tokens[1]))
    return false;

  if (!base::StringToInt(pattern_tokens[1], &result.pattern_.port) ||
      result.pattern_.port < 1 || result.pattern_.port > 65535)
    return false;

  *entry = result;
  return true;
}

std::string SocketPermissionEntry::GetHostPatternAsString() const {
  std::string result;

  if (!IsAddressBoundType())
    return result;

  if (match_subdomains()) {
    result.append(kWildcard);
    if (!pattern_.host.empty())
      result.append(1, kDot).append(pattern_.host);
  } else {
    result.append(pattern_.host);
  }

  if (pattern_.port == kWildcardPortNumber)
    result.append(1, kColon).append(kWildcard);
  else
    result.append(1, kColon).append(base::IntToString(pattern_.port));

  return result;
}

}  // namespace extensions
