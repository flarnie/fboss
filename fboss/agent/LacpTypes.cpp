#include "fboss/agent/LacpTypes.h"

#include <folly/Conv.h>
#include <folly/Range.h>

namespace facebook {
namespace fboss {

LACPDU::LACPDU(ParticipantInfo actor, ParticipantInfo partner)
    : actorInfo(actor), partnerInfo(partner) {}

bool ParticipantInfo::operator==(const ParticipantInfo& rhs) const {
  return systemPriority == rhs.systemPriority && systemID == rhs.systemID &&
      key == rhs.key && portPriority == rhs.portPriority && port == rhs.port &&
      state == rhs.state;
}

bool ParticipantInfo::operator!=(const ParticipantInfo& rhs) const {
  return !(*this == rhs);
}

std::string ParticipantInfo::describe() const {
  auto systemAsMacAddr = folly::MacAddress::fromBinary(
      folly::ByteRange(systemID.begin(), systemID.end()));

  return folly::to<std::string>(
      "(SystemPriority ",
      systemPriority,
      ", SystemID ",
      systemAsMacAddr.toString(),
      ", Key ",
      key,
      ", PortPriority ",
      portPriority,
      ", Port ",
      port,
      ", State ",
      state,
      ")");
}

bool LACPDU::isValid() const {
  // TODO(samank): validate frame
  return true;
}

std::string LACPDU::describe() const {
  return folly::to<std::string>(
      "version=",
      version,
      " actorInfo=",
      actorInfo.describe(),
      " partnerInfo=",
      partnerInfo.describe(),
      " maxDelay=",
      maxDelay);
}

LinkAggregationGroupID LinkAggregationGroupID::from(
    const ParticipantInfo& actorInfo,
    const ParticipantInfo& partnerInfo) {
  LinkAggregationGroupID lagID;

  lagID.actorSystemID = actorInfo.systemID;
  lagID.actorSystemPriority = actorInfo.systemPriority;
  lagID.actorKey = actorInfo.key;
  if (!(actorInfo.state & LacpState::AGGREGATABLE)) {
    lagID.actorPort = actorInfo.port;
    lagID.actorPortPriority = actorInfo.portPriority;
  }

  lagID.partnerSystemID = partnerInfo.systemID;
  lagID.partnerSystemPriority = partnerInfo.systemPriority;
  lagID.partnerKey = partnerInfo.key;
  if (!(partnerInfo.state & LacpState::AGGREGATABLE)) {
    lagID.partnerPort = partnerInfo.port;
    lagID.partnerPortPriority = partnerInfo.portPriority;
  }

  return lagID;
}

std::string LinkAggregationGroupID::describe() const {
  // TODO(samank): zero out physical ports if not individual
  // TODO(samank): put smaller of S and T first
  std::string description;

  auto actorSystemMacAddr = folly::MacAddress::fromBinary(
      folly::ByteRange(actorSystemID.begin(), actorSystemID.end()));
  auto partnerSystemMacAddr = folly::MacAddress::fromBinary(
      folly::ByteRange(partnerSystemID.begin(), partnerSystemID.end()));

  // TODO(samank): write out as hex
  return folly::to<std::string>(
      "[(",
      actorSystemPriority,
      ",",
      actorSystemMacAddr.toString(),
      ",",
      actorKey,
      ",",
      actorPortPriority,
      ",",
      actorPort,
      "),(",
      partnerSystemPriority,
      ",",
      partnerSystemMacAddr.toString(),
      ",",
      partnerKey,
      ",",
      partnerPortPriority,
      ",",
      partnerPort,
      ")]");
}
} // namespace fboss
} // namespace facebook
