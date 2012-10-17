// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Some helpers for quic

#ifndef NET_QUIC_QUIC_UTILS_H_
#define NET_QUIC_QUIC_UTILS_H_

#include <string>

#include "net/base/net_export.h"
#include "net/quic/quic_protocol.h"
#include "net/quic/uint128.h"

class SocketAddress;

namespace gfe2 {
  class BalsaHeaders;
}

namespace net {

class NET_EXPORT_PRIVATE QuicUtils {
 public:
  // The overhead the quic framing will add for a packet with num_fragments
  // fragments.
  static int StreamFragmentPacketOverhead(int num_fragments);

  // returns the 128 bit FNV1a hash of the data.  See
  // http://www.isthe.com/chongo/tech/comp/fnv/index.html#FNV-param
  static uint128 FNV1a_128_Hash(const char* data, int len);

  // Returns the name of the quic error code as a char*
  static const char* ErrorToString(QuicErrorCode error);
};

}  // namespace net

#endif  // NET_QUIC_QUIC_UTILS_H_
