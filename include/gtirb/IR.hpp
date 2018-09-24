//===- IR.hpp ---------------------------------------------------*- C++ -*-===//
//
//  Copyright (C) 2018 GrammaTech, Inc.
//
//  This code is licensed under the MIT license. See the LICENSE file in the
//  project root for license terms.
//
//  This project is sponsored by the Office of Naval Research, One Liberty
//  Center, 875 N. Randolph Street, Arlington, VA 22203 under contract #
//  N68335-17-C-0700.  The content of the information does not necessarily
//  reflect the position or policy of the Government and no official
//  endorsement should be inferred.
//
//===----------------------------------------------------------------------===//
#ifndef GTIRB_IR_H
#define GTIRB_IR_H

#include <gtirb/Addr.hpp>
#include <gtirb/Module.hpp>
#include <gtirb/Node.hpp>
#include <gtirb/Table.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/range/iterator_range.hpp>
#include <map>
#include <string>
#include <vector>

/// \file IR.hpp
/// \ingroup TABLE_GROUP
/// \brief Class gtirb::IR.
/// \see TABLE_GROUP

namespace proto {
class IR;
}
namespace gtirb {
class Module;

/// \class IR
///
/// \brief A complete internal representation consisting of Modules
/// (\ref Module).
///
/// \dot
/// digraph example {
///     node [shape=record, fontname=Helvetica, fontsize=10];
///
///     ir [ label="IR" URL="\ref IR"];
///     module [ label="Module" URL="\ref Module"];
///     table [ label="Table" URL="\ref Table"];
///     imageByteMap [label="ImageByteMap" URL="\ref ImageByteMap"]
///     blocks [label="Block" URL="\ref Block"]
///     data [label="DataObject"  URL="\ref DataObject"]
///     symbolicExpressions  [label="SymbolicExpression"
///                           URL="\ref SymbolicExpression"]
///     sections [label="Section" URL="\ref Section"]
///     symbols [label="Symbol" URL="\ref Symbol"]
///     cfg [label="CFG" URL="\ref CFG"]
///
///     ir -> module;
///     ir -> table;
///     module -> cfg;
///     module -> data
///     module -> imageByteMap;
///     module -> sections
///     module -> symbols;
///     module -> symbolicExpressions
///     cfg -> blocks;
/// }
/// \enddot
class GTIRB_EXPORT_API IR : public Node {
  IR(Context& C) : Node(C, Kind::IR) {}

public:
  /// \brief Create an IR object in its default state.
  ///
  /// \param C  The Context in which this object will be held.
  ///
  /// \return The newly created object.
  static IR* Create(Context& C) { return new (C) IR(C); }

  /// \brief Iterator over \ref Module "Modules".
  using iterator = boost::indirect_iterator<std::vector<Module*>::iterator>;
  /// \brief Constant iterator over \ref Module "Modules".
  using const_iterator =
      boost::indirect_iterator<std::vector<Module*>::const_iterator>;

  /// \brief Returns an iterator to the first Module.
  iterator begin() { return Modules.begin(); }
  /// \brief Returns an iterator to the element following the last Module.
  iterator end() { return Modules.end(); }
  /// \brief Returns a constant iterator to the first Module.
  const_iterator begin() const { return Modules.begin(); }
  /// \brief Returns a constant iterator to the element following the last
  /// Module.
  const_iterator end() const { return Modules.end(); }

  /// \brief Range of \ref Module "Modules".
  using range = boost::iterator_range<iterator>;
  /// \brief Constant range of \ref Module "Modules".
  using const_range = boost::iterator_range<const_iterator>;

  /// \brief Returns a range of the \ref Module "Modules".
  range modules() { return boost::make_iterator_range(begin(), end()); }
  /// \brief Returns a constant range of the \ref Module "Modules".
  const_range modules() const {
    return boost::make_iterator_range(begin(), end());
  }

  /// \brief Adds a single module to the IR.
  ///
  /// \param M The Module object to add.
  ///
  /// \return void
  void addModule(Module* M) { Modules.push_back(M); }

  /// \brief Adds one or more modules to the IR.
  ///
  /// \param Ms The list of Module objects to add.
  ///
  /// \return void
  void addModule(std::initializer_list<Module*> Ms) {
    Modules.insert(Modules.end(), Ms);
  }

  /// \brief Serialize to an output stream.
  ///
  /// \param Out The output stream.
  ///
  /// \return void
  void save(std::ostream& Out) const;

  /// \brief Deserialize from an input stream.
  ///
  /// \param C   The Context in which this IR will be loaded.
  /// \param In  The input stream.
  ///
  /// \return The deserialized IR object.
  static IR* load(Context& C, std::istream& In);

  /// \brief The protobuf message type used for serializing IR.
  using MessageType = proto::IR;

  /// \brief Serialize into a protobuf message.
  ///
  /// \param[out] Message   Serialize into this message.
  ///
  /// \return void
  void toProtobuf(MessageType* Message) const;

  /// \brief Construct a IR from a protobuf message.
  ///
  /// \param C   The Context in which the deserialized IR will be held.
  /// \param Message  The protobuf message from which to deserialize.
  ///
  /// \return The deserialized IR object, or null on failure.
  static IR* fromProtobuf(Context& C, const MessageType& Message);

  /// \name Table Properties
  /// @{

  /// \brief Add a new table, transferring ownership.
  /// The table can be populated from anywhere.
  ///
  /// \param Name     The name to assign to the table so it can be found later.
  /// \param X        The table itself.
  ///
  /// \return void
  ///
  /// \ingroup TABLE_GROUP
  void addTable(const std::string& Name, Table&& X);

  /// \brief Get a table by name.
  ///
  /// \param  X   The name of the table to search for.
  ///
  /// \return     A non-owning pointer to the table if found,
  ///             \c nullptr otherwise.
  ///
  /// \ingroup TABLE_GROUP
  const gtirb::Table* getTable(const std::string& X) const;

  /// \brief Get a table by name.
  ///
  /// \param  X   The name of the table to search for.
  ///
  /// \return     A non-owning pointer to the table if found,
  ///             \c nullptr otherwise.
  ///
  /// \ingroup TABLE_GROUP
  gtirb::Table* getTable(const std::string& X);

  /// \brief Remove a table by name.
  ///
  /// This will invalidate any pointers that may have been held externally.
  ///
  /// \param  X   The name of the table to search for.
  /// \return     \c true on success, \c false otherwise.
  ///
  /// \ingroup TABLE_GROUP
  bool removeTable(const std::string& X);

  /// \brief Get the total number of tables in this IR.
  ///
  /// \return     The total number of tables this node owns.
  ///
  /// \ingroup TABLE_GROUP
  size_t getTableSize() const { return Tables.size(); }

  /// \brief Check: Is the number of tables in this IR zero?
  ///
  /// \return \c true if this node does not own any tables, \c false
  /// if it owns one or more tables.
  ///
  /// \ingroup TABLE_GROUP
  bool getTablesEmpty() const { return Tables.empty(); }

  /// \brief Clear all locally owned tables.
  ///
  /// \return void
  ///
  /// \ingroup TABLE_GROUP
  void clearTables() { Tables.clear(); }

  /// @}

  /// \cond INTERNAL
  static bool classof(const Node* N) { return N->getKind() == Kind::IR; }
  /// \endcond

private:
  std::map<std::string, gtirb::Table> Tables;
  std::vector<Module*> Modules;
};
} // namespace gtirb

#endif // GTIRB_IR_H
