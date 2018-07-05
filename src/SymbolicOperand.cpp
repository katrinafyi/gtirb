#include <proto/SymbolicOperand.pb.h>
#include <boost/serialization/export.hpp>
#include <boost/serialization/string.hpp>
#include <boost/variant/static_visitor.hpp>
#include <gtirb/Module.hpp>
#include <gtirb/Serialization.hpp>
#include <gtirb/Symbol.hpp>
#include <gtirb/SymbolicOperand.hpp>

BOOST_CLASS_EXPORT_IMPLEMENT(gtirb::SymStackConst);
BOOST_CLASS_EXPORT_IMPLEMENT(gtirb::SymAddrConst);
BOOST_CLASS_EXPORT_IMPLEMENT(gtirb::SymAddrAddr);

namespace gtirb
{
    template <class Archive>
    void SymStackConst::serialize(Archive& ar, const unsigned int /*version*/)
    {
        ar & this->negate;
        ar & this->offset;
        ar & this->displacement;
        ar & this->symbol;
    }

    template <class Archive>
    void SymAddrConst::serialize(Archive& ar, const unsigned int /*version*/)
    {
        ar & this->displacement;
        ar & this->symbol;
    }

    template <class Archive>
    void SymAddrAddr::serialize(Archive& ar, const unsigned int /*version*/)
    {
        ar & this->scale;
        ar & this->offset;
        ar & this->symbol1;
        ar & this->symbol2;
    }

    class SymbolicVisitor : public boost::static_visitor<>
    {
    public:
        proto::SymbolicOperand* message;

        SymbolicVisitor(proto::SymbolicOperand* m) : message(m)
        {
        }

        void operator()(const SymStackConst& val) const
        {
            auto m = message->mutable_stack_const();
            m->set_negate(val.negate);
            m->set_offset(val.offset);
            m->set_displacement(val.displacement);
            uuidToBytes(val.symbol.getUUID(), *m->mutable_symbol_uuid());
        }

        void operator()(const SymAddrConst& val) const
        {
            auto m = message->mutable_addr_const();
            m->set_displacement(val.displacement);
            uuidToBytes(val.symbol.getUUID(), *m->mutable_symbol_uuid());
        }

        void operator()(const SymAddrAddr& val) const
        {
            auto m = message->mutable_addr_addr();
            m->set_scale(val.scale);
            m->set_offset(val.offset);
            uuidToBytes(val.symbol1.getUUID(), *m->mutable_symbol1_uuid());
            uuidToBytes(val.symbol2.getUUID(), *m->mutable_symbol2_uuid());
        }
    };

    proto::SymbolicOperand toProtobuf(const SymbolicOperand& operand)
    {
        proto::SymbolicOperand message;
        boost::apply_visitor(SymbolicVisitor(&message), operand);
        return message;
    }

    void fromProtobuf(SymbolicOperand& result, const proto::SymbolicOperand& message)
    {
        switch(message.value_case())
        {
            case proto::SymbolicOperand::kStackConst:
            {
                auto val = message.stack_const();
                result = SymStackConst{val.negate(), val.offset(), val.displacement(),
                                       uuidFromBytes(val.symbol_uuid())};
                break;
            }
            case proto::SymbolicOperand::kAddrConst:
            {
                auto val = message.addr_const();
                result = SymAddrConst{val.displacement(), uuidFromBytes(val.symbol_uuid())};
                break;
            }
            case proto::SymbolicOperand::kAddrAddr:
            {
                auto val = message.addr_addr();
                result = SymAddrAddr{val.scale(), val.offset(), uuidFromBytes(val.symbol1_uuid()),
                                     uuidFromBytes(val.symbol2_uuid())};
                break;
            }
            case proto::SymbolicOperand::VALUE_NOT_SET:
                assert(false);
        }
    }
}
