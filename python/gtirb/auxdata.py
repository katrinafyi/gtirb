from io import BytesIO
import typing
import uuid

import AuxData_pb2
import AuxDataContainer_pb2

from .node import Node
from .serialization import Serialization
from .util import DictLike


class AuxData:
    """AuxData objects can be attached to the :class:`gtirb.IR` or individual
    :class:`gtirb.Module` s to store additional client-specific data in a
    portable way.

    AuxData represents a portable, language-independent manner of encoding
    rich data. To do this, all data is stored on disk as a series of bytes
    with a string describing the format of the data, called a *type name*. See
    :mod:`gtirb.serialization` for the list of all default types. Types may
    also be parameterized; for example, ``mapping<string,UUID>`` is a ``dict``
    from ``str`` objects to ``UUID`` objects. All ``AuxData`` requires
    a valid type name in order to be serialized.

    :ivar data: The value stored in this AuxData.
    :ivar type_name: A string describing the type of ``data``.
        Used to determine the proper codec for serializing this AuxData.
    """

    serializer = Serialization()
    """This is a :class:`gtirb.Serialization` instance, used to
    encode and decode ``data`` fields of all ``AuxData``. See
    :mod:`gtirb.serialization` for details.
    """

    data: typing.Any
    type_name: str

    def __init__(self, data: typing.Any, type_name: str):
        """
        :param data: The value stored in this AuxData.
        :param type_name: A string describing the type of ``data``.
            Used to determine the proper codec for serializing this AuxData.
        """

        self.data = data
        self.type_name = type_name

    @classmethod
    def _from_protobuf(cls, aux_data: AuxData_pb2.AuxData) -> "AuxData":
        """Deserialize AuxData from Protobuf.

        :param aux_data: The Protobuf AuxData object.
        """

        data = AuxData.serializer.decode(
            BytesIO(aux_data.data), aux_data.type_name
        )
        return cls(data=data, type_name=aux_data.type_name)

    def _to_protobuf(self) -> AuxData_pb2.AuxData:
        """Get a Protobuf representation of the AuxData."""

        out_bytes_array = BytesIO()
        AuxData.serializer.encode(out_bytes_array, self.data, self.type_name)
        proto_auxdata = AuxData_pb2.AuxData()
        proto_auxdata.type_name = self.type_name
        proto_auxdata.data = out_bytes_array.getvalue()
        return proto_auxdata

    def __repr__(self) -> str:
        return (
            "AuxData("
            "type_name={type_name!r}, "
            "data={data!r}, "
            ")".format(**self.__dict__)
        )


class AuxDataContainer(Node):
    """The base class for anything that holds AuxData tables; that is,
    :class:`gtirb.IR` and :class:`gtirb.Module`.

    :ivar aux_data: The auxiliary data associated
            with the object, as a mapping from names to
            :class:`gtirb.AuxData`.
    """

    aux_data: typing.Dict[str, AuxData]

    def __init__(
        self,
        aux_data: DictLike[str, AuxData] = {},
        uuid: typing.Optional[uuid.UUID] = None,
    ):
        """
        :param aux_data: The initial auxiliary data to be associated
            with the object, as a mapping from names to
            :class:`gtirb.AuxData`. Defaults to an empty :class:`dict`.
        :param uuid: the UUID of this Node,
            or None if a new UUID needs generated via :func:`uuid.uuid4`.
            Defaults to None.
        """
        super().__init__(uuid)
        self.aux_data = dict(aux_data)

    @classmethod
    def _decode_protobuf(
        cls,
        proto_container: AuxDataContainer_pb2.AuxDataContainer,
        uuid: uuid.UUID,
    ) -> "AuxDataContainer":
        aux_data = (
            (key, AuxData.from_protobuf(val))
            for key, val in proto_container.aux_data.items()
        )
        return cls(aux_data, uuid)

    def _to_protobuf(self) -> AuxDataContainer_pb2.AuxDataContainer:
        proto_auxdatacontainer = AuxDataContainer_pb2.AuxDataContainer()
        for k, v in self.aux_data.items():
            proto_auxdatacontainer.aux_data[k].CopyFrom(v._to_protobuf())
        return proto_auxdatacontainer

    def deep_eq(self, other) -> bool:
        """This overrides :func:`gtirb.Node.deep_eq` to check for
        AuxData equality.

        Because the values stored by AuxData are not necessarily
        amenable to deep checking, the auxiliary data dictionaries
        stored for ``self`` and ``other`` are not deeply checked. Instead,
        they are considered to be equal if their sets of keys are equal.
        """

        if not isinstance(other, AuxDataContainer):
            return False
        if (
            self.uuid != other.uuid
            or self.aux_data.keys() != other.aux_data.keys()
        ):
            return False
        return True
