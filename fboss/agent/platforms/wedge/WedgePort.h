/*
 *  Copyright (c) 2004-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */
#pragma once

#include <boost/container/flat_map.hpp>

#include <folly/Optional.h>
#include <folly/futures/Future.h>

#include "fboss/agent/hw/bcm/BcmPlatformPort.h"
#include "fboss/agent/hw/bcm/BcmPort.h"
#include "fboss/agent/gen-cpp2/switch_config_types.h"

namespace facebook { namespace fboss {

class WedgePlatform;

class WedgePort : public BcmPlatformPort {
 protected:
  WedgePort(PortID id,
            WedgePlatform* platform,
            folly::Optional<TransceiverID> frontPanelPort,
            folly::Optional<ChannelID> channel);

 public:
  PortID getPortID() const override { return id_; }

  void setBcmPort(BcmPort* port) override;
  BcmPort* getBcmPort() const override {
    return bcmPort_;
  }

  void preDisable(bool temporary) override;
  void postDisable(bool temporary) override;
  void preEnable() override;
  void postEnable() override;
  bool isMediaPresent() override;
  void statusIndication(bool enabled, bool link,
                        bool ingress, bool egress,
                        bool discards, bool errors) override;
  void linkSpeedChanged(const cfg::PortSpeed& speed) override;
  void linkStatusChanged(bool up, bool adminUp) override;

  folly::Optional<TransceiverID> getTransceiverID() const override {
    return frontPanelPort_;
  }

  bool supportsTransceiver() const override {
    return getTransceiverID().hasValue();
  }

  virtual folly::Optional<ChannelID> getChannel() const {
    return channel_;
  }

  std::vector<int32_t> getChannels() const;

  folly::Future<TransmitterTechnology> getTransmitterTech(
      folly::EventBase* evb) const override;
  folly::Future<folly::Optional<TxSettings>> getTxSettings(
      folly::EventBase* evb) const override;
  void customizeTransceiver() override;

 protected:
  bool isControllingPort() const;
  bool isInSingleMode() const;
  bool shouldCustomizeTransceiver() const;

  PortID id_{0};
  WedgePlatform* platform_{nullptr};
  cfg::PortSpeed speed_{cfg::PortSpeed::DEFAULT};
  folly::Optional<TransceiverID> frontPanelPort_;
  folly::Optional<ChannelID> channel_;
  BcmPort* bcmPort_{nullptr};

 private:
  // Forbidden copy constructor and assignment operator
  WedgePort(WedgePort const &) = delete;
  WedgePort& operator=(WedgePort const &) = delete;

  virtual TxOverrides getTxOverrides() const override {
    return TxOverrides();
  }

  folly::Future<TransceiverInfo> getTransceiverInfo(
    folly::EventBase* evb) const;


};

}} // facebook::fboss
