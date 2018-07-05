#pragma once

#include <gtirb/EA.hpp>
#include <gtirb/Node.hpp>
#include <set>

namespace proto
{
    class Region;
}
namespace gtirb
{
    ///
    /// \class Region
    /// \author John E. Farrier
    ///
    /// A base class for Module Regions.
    ///
    /// \todo   Several different gtirb::Region subclasses may be created to encompass Global,
    /// Module, Abstract, Heap, Stack, and Extern regions.  What goes into these classes is
    /// undefined in my notes.
    ///
    class GTIRB_GTIRB_EXPORT_API Region : public Node
    {
    public:
        Region() = default;
        ~Region() override = default;

        std::set<gtirb::EA>& getEAs();
        const std::set<gtirb::EA>& getEAs() const;

        ///
        /// Serialization support.
        ///
        template <class Archive>
        void serialize(Archive& ar, const unsigned int /*version*/)
        {
            ar& boost::serialization::base_object<Node>(*this);
            ar & this->eas;
        }

        using MessageType = proto::Region;
        void toProtobuf(MessageType* message) const;
        void fromProtobuf(const MessageType& message);

    private:
        std::set<gtirb::EA> eas;
    };
}

BOOST_CLASS_EXPORT_KEY(gtirb::Region);
