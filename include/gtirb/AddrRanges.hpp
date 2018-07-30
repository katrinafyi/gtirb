#pragma once

#include <gtirb/Node.hpp>
#include <cstdint>
#include <map>
#include <utility>

namespace proto {
class AddrRanges;
}

namespace gtirb {
class EA;

///
/// \class AddrRanges
///
/// Thin list of non-overlapping non-adjacent EA ranges.  Each
/// range is [lb, ub), inclusive of lb but exclusive of ub.  This
/// class guarantees a canonical representation: no redundancy or
/// overlaps, adjacent ranges are coalesced, empty ranges are
/// excluded.
///
class GTIRB_EXPORT_API AddrRanges : public Node {
public:
  ///
  /// Default Constructor.
  ///
  AddrRanges() = default;

  ///
  /// Copy constructor. Assigns a new UUID to the copy.
  ///
  explicit AddrRanges(const AddrRanges& other) = default;

  ///
  /// Move constructor
  ///
  AddrRanges(AddrRanges&&) = default;

  ///
  /// Move assignment
  ///
  AddrRanges& operator=(AddrRanges&&) = default;

  ///
  /// Defaulted trivial destructor.
  ///
  ~AddrRanges() override = default;

  ///
  /// Add a pair of EA's to the ranges.
  ///
  /// This will perform a check on the EA's to ensure that the
  /// first is less than or equal to the second.
  ///
  /// Throws a gtirb::RuntimeError if the EA's are not properly ordered.
  ///
  /// \param  x   A pair of EA's to add to the container. (LB, UB)
  ///
  /// \return     True on success.
  ///
  bool addRange(std::pair<gtirb::EA, gtirb::EA> x);

  ///
  /// Add (union) x into this range.
  ///
  /// \return     True on success.
  ///
  bool addRange(const AddrRanges& x);

  ///
  /// Subtract a pair of EA's from the ranges.
  ///
  /// This will perform a check on the EA's to ensure that the
  /// first is less than or equal to the second.
  ///
  /// Throws a gtirb::RuntimeError if the EA's are not properly ordered.
  ///
  /// \param  x   A pair of EA's to subtract from the container. (LB, UB)
  ///
  bool subtractRange(std::pair<gtirb::EA, gtirb::EA> x);

  ///
  ///
  ///
  bool subtractRange(const AddrRanges& x);

  ///
  /// Given all of the ranges, how many bytes exist between all pairs?
  ///
  /// \return     The number of bytes in all of the ranges in the container.
  ///
  size_t getBytesCoveredByRanges() const;

  ///
  ///
  ///
  bool getContains(gtirb::EA x) const;

  ///
  ///
  ///
  void clearRanges();

  ///
  ///
  ///
  void swap(gtirb::AddrRanges& x);

  ///
  /// Return an iterator pointing to the first region that
  /// intersects with the given [lb,ub).
  ///
  /// \return std::end(std::map<gtirb::EA, gtirb::EA>) if there
  /// is no intersection.
  ///
  std::map<gtirb::EA, gtirb::EA>::const_iterator getFirstIntersecting(gtirb::EA lb,
                                                                      gtirb::EA ub) const;

  ///
  ///
  ///
  std::map<gtirb::EA, gtirb::EA>::const_iterator getRangeContaining(gtirb::EA x) const;

  ///
  /// Use this to "destructively retrieve" this AddrRanges' map.
  ///
  void swapAndClear(std::map<gtirb::EA, gtirb::EA>& x);

  ///
  /// Use this to iterate through ranges
  ///
  std::map<gtirb::EA, gtirb::EA>& data();

  ///
  /// Use this to iterate through ranges
  ///
  const std::map<gtirb::EA, gtirb::EA>& data() const;

  using MessageType = proto::AddrRanges;
  void toProtobuf(MessageType* message) const;
  void fromProtobuf(const MessageType& message);

private:
  std::map<gtirb::EA, gtirb::EA> ranges;
};
} // namespace gtirb
