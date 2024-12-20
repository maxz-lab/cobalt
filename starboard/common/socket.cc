// Copyright 2019 The Cobalt Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "starboard/common/socket.h"

#include <iomanip>

#include "starboard/common/log.h"
#include "starboard/configuration.h"

namespace starboard {

SbSocketAddress GetUnspecifiedAddress(SbSocketAddressType address_type,
                                      int port) {
  SbSocketAddress address = {};
  address.type = address_type;
  address.port = port;
  return address;
}

bool GetLocalhostAddress(SbSocketAddressType address_type,
                         int port,
                         SbSocketAddress* address) {
  if (address_type != kSbSocketAddressTypeIpv4 &&
      address_type != kSbSocketAddressTypeIpv6) {
    SB_LOG(ERROR) << __FUNCTION__ << ": unknown address type: " << address_type;
    return false;
  }
  *address = GetUnspecifiedAddress(address_type, port);
  switch (address_type) {
    case kSbSocketAddressTypeIpv4:
      address->address[0] = 127;
      address->address[3] = 1;
      break;
    case kSbSocketAddressTypeIpv6:
      address->address[15] = 1;
      break;
  }

  return true;
}

bool Socket::IsValid() {
  return SbSocketIsValid(socket_);
}

SbSocket Socket::socket() {
  return socket_;
}

Socket::Socket(SbSocket socket) : socket_(socket) {}

}  // namespace starboard

std::ostream& operator<<(std::ostream& os, const SbSocketAddress& address) {
  if (address.type == kSbSocketAddressTypeIpv6) {
    os << std::hex << "[";
    const uint16_t* fields = reinterpret_cast<const uint16_t*>(address.address);
    int i = 0;
    while (fields[i] == 0) {
      if (i == 0) {
        os << ":";
      }
      ++i;
      if (i == 8) {
        os << ":";
      }
    }
    for (; i < 8; ++i) {
      if (i != 0) {
        os << ":";
      }
#if SB_IS(LITTLE_ENDIAN)
      const uint8_t* fields8 = reinterpret_cast<const uint8_t*>(&fields[i]);
      const uint16_t value = static_cast<uint16_t>(fields8[0]) * 256 |
                             static_cast<uint16_t>(fields8[1]);
      os << value;
#else
      os << fields[i];
#endif
    }
    os << "]" << std::dec;
  } else {
    for (int i = 0; i < 4; ++i) {
      if (i != 0) {
        os << ".";
      }
      os << static_cast<int>(address.address[i]);
    }
  }
  os << ":" << address.port;
  return os;
}
