#include <gtest/gtest.h>
#include <gtirb/AddrRanges.hpp>
#include <gtirb/CFG.hpp>
#include <gtirb/IR.hpp>
#include <gtirb/ImageByteMap.hpp>
#include <gtirb/Module.hpp>
#include <gtirb/ModuleAux.hpp>
#include <gtirb/ModuleCore.hpp>
#include <gtirb/ModuleSummary.hpp>
#include <gtirb/NodeUtilities.hpp>
#include <gtirb/ProcedureSet.hpp>
#include <gtirb/SymbolSet.hpp>
#include <memory>

TEST(Unit_Module, ctor_0)
{
    EXPECT_NO_THROW(gtirb::Module());
}

TEST(Unit_Module, validParent)
{
    auto parent = std::make_unique<gtirb::IR>();
    auto child = std::make_unique<gtirb::Module>();
    EXPECT_TRUE(child->getIsValidParent(parent.get()));
    EXPECT_NO_THROW(parent->push_back(std::move(child)));
}

TEST(Unit_Module, invalidParent)
{
    auto notAParent = std::make_unique<gtirb::Node>();
    auto child = std::make_unique<gtirb::Module>();

    EXPECT_FALSE(child->getIsValidParent(notAParent.get()));
    EXPECT_THROW(notAParent->push_back(std::move(child)), gtirb::NodeStructureError);
}

TEST(Unit_Module, setBinaryPath)
{
    const std::string strPath("/home/gt/irb/foo");
    auto m = std::make_shared<gtirb::Module>();

    EXPECT_NO_THROW(m->setBinaryPath(strPath));

    auto path = m->getBinaryPath();
    EXPECT_EQ(boost::filesystem::path(strPath), path);
}

TEST(Unit_Module, getFileFormatDefault)
{
    auto m = std::make_shared<gtirb::Module>();
    EXPECT_EQ(gtirb::FileFormat::Undefined, m->getFileFormat());
}

TEST(Unit_Module, setFileFormat)
{
    auto m = std::make_shared<gtirb::Module>();

    EXPECT_NO_THROW(m->setFileFormat(gtirb::FileFormat::COFF));
    EXPECT_EQ(gtirb::FileFormat::COFF, m->getFileFormat());

    EXPECT_NO_THROW(m->setFileFormat(gtirb::FileFormat::MACHO));
    EXPECT_EQ(gtirb::FileFormat::MACHO, m->getFileFormat());

    EXPECT_NO_THROW(m->setFileFormat(gtirb::FileFormat::Undefined));
    EXPECT_EQ(gtirb::FileFormat::Undefined, m->getFileFormat());
}

TEST(Unit_Module, getRebaseDeltaDefault)
{
    auto m = std::make_shared<gtirb::Module>();
    EXPECT_EQ(int64_t{0}, m->getRebaseDelta());
}

TEST(Unit_Module, setRebaseDelta)
{
    auto m = std::make_shared<gtirb::Module>();

    EXPECT_NO_THROW(m->setRebaseDelta(1));
    EXPECT_EQ(int64_t{1}, m->getRebaseDelta());

    EXPECT_NO_THROW(m->setRebaseDelta(-1));
    EXPECT_EQ(int64_t{-1}, m->getRebaseDelta());

    EXPECT_NO_THROW(m->setRebaseDelta(std::numeric_limits<int64_t>::max()));
    EXPECT_EQ(std::numeric_limits<int64_t>::max(), m->getRebaseDelta());

    EXPECT_NO_THROW(m->setRebaseDelta(std::numeric_limits<int64_t>::min()));
    EXPECT_EQ(std::numeric_limits<int64_t>::min(), m->getRebaseDelta());

    EXPECT_NO_THROW(m->setRebaseDelta(std::numeric_limits<int64_t>::lowest()));
    EXPECT_EQ(std::numeric_limits<int64_t>::lowest(), m->getRebaseDelta());
}

TEST(Unit_Module, getEAMinMaxDefault)
{
    auto m = std::make_shared<gtirb::Module>();

    EXPECT_NO_THROW(m->getEAMinMax());
    EXPECT_EQ(gtirb::EA{}, m->getEAMinMax().first);
    EXPECT_EQ(gtirb::EA{}, m->getEAMinMax().second);
}

TEST(Unit_Module, setEAMinMax)
{
    auto m = std::make_shared<gtirb::Module>();

    gtirb::EA minimum{64};
    gtirb::EA maximum{1024};

    EXPECT_TRUE(m->setEAMinMax({minimum, maximum}));
    EXPECT_EQ(minimum, m->getEAMinMax().first);
    EXPECT_EQ(maximum, m->getEAMinMax().second);

    EXPECT_FALSE(m->setEAMinMax({maximum, minimum}));
    EXPECT_EQ(gtirb::EA{}, m->getEAMinMax().first);
    EXPECT_EQ(gtirb::EA{}, m->getEAMinMax().second);
}

TEST(Unit_Module, getPreferredEADefault)
{
    auto m = std::make_shared<gtirb::Module>();

    EXPECT_NO_THROW(m->getPreferredEA());
    EXPECT_EQ(gtirb::EA{}, m->getPreferredEA());
}

TEST(Unit_Module, setPreferredEA)
{
    auto m = std::make_shared<gtirb::Module>();
    const gtirb::EA preferred{64};

    EXPECT_NO_THROW(m->getPreferredEA());
    EXPECT_NO_THROW(m->setPreferredEA(preferred));

    EXPECT_EQ(preferred, m->getPreferredEA());
}

TEST(Unit_Module, getOrCreateModuleSummary)
{
    auto m = std::make_shared<gtirb::Module>();
    auto children = gtirb::GetChildrenOfType<gtirb::ModuleSummary>(m.get());
    EXPECT_TRUE(children.empty());

    EXPECT_NO_THROW(m->getOrCreateModuleSummary());
    EXPECT_TRUE(m->getOrCreateModuleSummary() != nullptr);

    children = gtirb::GetChildrenOfType<gtirb::ModuleSummary>(m.get());
    EXPECT_FALSE(children.empty());
}

TEST(Unit_Module, getOrCreateModuleCore)
{
    auto m = std::make_shared<gtirb::Module>();
    auto children = gtirb::GetChildrenOfType<gtirb::ModuleCore>(m.get());
    EXPECT_TRUE(children.empty());

    EXPECT_NO_THROW(m->getOrCreateModuleCore());
    EXPECT_TRUE(m->getOrCreateModuleCore() != nullptr);

    children = gtirb::GetChildrenOfType<gtirb::ModuleCore>(m.get());
    EXPECT_FALSE(children.empty());
}

TEST(Unit_Module, getOrCreateModuleAux)
{
    auto m = std::make_shared<gtirb::Module>();
    auto children = gtirb::GetChildrenOfType<gtirb::ModuleAux>(m.get());
    EXPECT_TRUE(children.empty());

    EXPECT_NO_THROW(m->getOrCreateModuleAux());
    EXPECT_TRUE(m->getOrCreateModuleAux() != nullptr);

    children = gtirb::GetChildrenOfType<gtirb::ModuleAux>(m.get());
    EXPECT_FALSE(children.empty());
}

TEST(Unit_Module, getOrCreateAddrRanges)
{
    auto m = std::make_shared<gtirb::Module>();
    auto children = gtirb::GetChildrenOfType<gtirb::AddrRanges>(m.get());
    EXPECT_TRUE(children.empty());

    EXPECT_NO_THROW(m->getOrCreateAddrRanges());
    EXPECT_TRUE(m->getOrCreateAddrRanges() != nullptr);

    children = gtirb::GetChildrenOfType<gtirb::AddrRanges>(m.get());
    EXPECT_FALSE(children.empty());
}

TEST(Unit_Module, getOrCreateSymbolSet)
{
    auto m = std::make_shared<gtirb::Module>();
    auto children = gtirb::GetChildrenOfType<gtirb::SymbolSet>(m.get());
    EXPECT_TRUE(children.empty());

    EXPECT_NO_THROW(m->getOrCreateSymbolSet());
    EXPECT_TRUE(m->getOrCreateSymbolSet() != nullptr);

    children = gtirb::GetChildrenOfType<gtirb::SymbolSet>(m.get());
    EXPECT_FALSE(children.empty());
}

TEST(Unit_Module, getOrCreateProcedureSet)
{
    auto m = std::make_shared<gtirb::Module>();
    auto children = gtirb::GetChildrenOfType<gtirb::ProcedureSet>(m.get());
    EXPECT_TRUE(children.empty());

    EXPECT_NO_THROW(m->getOrCreateProcedureSet());
    EXPECT_TRUE(m->getOrCreateProcedureSet() != nullptr);

    children = gtirb::GetChildrenOfType<gtirb::ProcedureSet>(m.get());
    EXPECT_FALSE(children.empty());
}

TEST(Unit_Module, getOrCreateImageByteMap)
{
    auto m = std::make_shared<gtirb::Module>();
    auto children = gtirb::GetChildrenOfType<gtirb::ImageByteMap>(m.get());
    EXPECT_TRUE(children.empty());

    EXPECT_NO_THROW(m->getOrCreateImageByteMap());
    EXPECT_TRUE(m->getOrCreateImageByteMap() != nullptr);

    children = gtirb::GetChildrenOfType<gtirb::ImageByteMap>(m.get());
    EXPECT_FALSE(children.empty());
}

TEST(Unit_Module, getOrCreateCFG)
{
    auto m = std::make_shared<gtirb::Module>();
    auto children = gtirb::GetChildrenOfType<gtirb::CFG>(m.get());
    EXPECT_TRUE(children.empty());

    EXPECT_NO_THROW(m->getOrCreateCFG());
    EXPECT_TRUE(m->getOrCreateCFG() != nullptr);

    children = gtirb::GetChildrenOfType<gtirb::CFG>(m.get());
    EXPECT_FALSE(children.empty());
}
