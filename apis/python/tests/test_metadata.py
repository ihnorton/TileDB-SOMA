import math
from typing import Any, Dict

import pyarrow as pa
import pytest

import tiledbsoma as soma
from tiledbsoma import factory

""""
Metadata handling tests for all SOMA foundational datatypes.
"""


@pytest.fixture(
    scope="function",
    params=[
        "Collection",
        "DataFrame",
        "DenseNDArray",
        "SparseNDArray",
    ],
)
def soma_object(request, tmp_path):
    """
    Make an empty test object of the given foundational class name.
    """
    uri = tmp_path.joinpath("object").as_uri()
    class_name = request.param

    if class_name == "Collection":
        so = soma.Collection.create(uri)

    elif class_name == "DataFrame":
        so = soma.DataFrame.create(
            uri,
            schema=pa.schema([("C", pa.float32()), ("D", pa.uint32())]),
            index_column_names=["D"],
        )

    elif class_name == "DenseNDArray":
        so = soma.DenseNDArray.create(uri, type=pa.float64(), shape=(100, 10, 1))

    elif class_name == "SparseNDArray":
        so = soma.SparseNDArray.create(uri, type=pa.int8(), shape=(11,))
    else:
        raise ValueError(f"don't know how to make {class_name}")

    yield so
    so.close()


def test_metadata(soma_object):
    """Basic API endpoints"""
    # Verify the metadata is empty to start. "Empty" defined as no keys
    # other than soma_ keys.
    uri = soma_object.uri
    with soma_object:
        assert non_soma_metadata(soma_object) == {}
        non_soma_keys = [k for k in soma_object.metadata if not k.startswith("soma_")]
        assert non_soma_keys == []
        as_dict = dict(soma_object.metadata)
        assert frozenset(soma_object.metadata) == frozenset(as_dict)
        assert "foobar" not in soma_object.metadata

        soma_object.metadata["foobar"] = True
        assert non_soma_metadata(soma_object) == {"foobar": True}

    with pytest.raises(soma.SOMAError):
        soma_object.metadata["x"] = "y"

    with factory.open(uri, "r") as read_obj:
        assert non_soma_metadata(read_obj) == {"foobar": True}
        assert "foobar" in read_obj.metadata
        # Double-check the various getter methods
        for k, v in read_obj.metadata.items():
            assert k in read_obj.metadata
            assert read_obj.metadata.get(k) == v
            assert read_obj.metadata[k] == v
        assert read_obj.metadata.get("freeble", "blorp") == "blorp"
        with pytest.raises(soma.SOMAError):
            read_obj.metadata["x"] = "y"

    with factory.open(uri, "w") as second_write:
        second_write.metadata.update(stay="frosty", my="friends")
        assert non_soma_metadata(second_write) == {
            "foobar": True,
            "stay": "frosty",
            "my": "friends",
        }

    with factory.open(uri, "w") as third_write:
        del third_write.metadata["stay"]
        third_write.metadata["my"] = "enemies"
        assert non_soma_metadata(third_write) == {"foobar": True, "my": "enemies"}
        assert "stay" not in third_write.metadata
        assert third_write.metadata.get("stay", False) is False

    with factory.open(uri, "r") as second_read:
        assert non_soma_metadata(second_read) == {"foobar": True, "my": "enemies"}


def test_add_delete_metadata(soma_object):
    uri = soma_object.uri
    with soma_object:
        soma_object.metadata["heres"] = "johnny"
        assert non_soma_metadata(soma_object) == {"heres": "johnny"}
        del soma_object.metadata["heres"]
        assert non_soma_metadata(soma_object) == {}

    with factory.open(uri) as reader:
        assert non_soma_metadata(reader) == {}


def test_delete_add_metadata(soma_object):
    uri = soma_object.uri
    with soma_object:
        soma_object.metadata["hdyfn"] = "destruction"
        assert non_soma_metadata(soma_object) == {"hdyfn": "destruction"}

    with factory.open(uri, "w") as second_write:
        assert non_soma_metadata(second_write) == {"hdyfn": "destruction"}
        del second_write.metadata["hdyfn"]
        assert non_soma_metadata(second_write) == {}
        second_write.metadata["hdyfn"] = "somebody new"
        assert non_soma_metadata(second_write) == {"hdyfn": "somebody new"}

    with factory.open(uri, "r") as reader:
        assert non_soma_metadata(reader) == {"hdyfn": "somebody new"}


def test_set_set_metadata(soma_object):
    uri = soma_object.uri

    with soma_object:
        soma_object.metadata["content"] = "content"

    with factory.open(uri, "w") as second_write:
        second_write.metadata["content"] = "confidence"
        second_write.metadata["content"] = "doubt"

    with factory.open(uri, "r") as reader:
        assert non_soma_metadata(reader) == {"content": "doubt"}


def test_set_delete_metadata(soma_object):
    uri = soma_object.uri

    with soma_object:
        soma_object.metadata["possession"] = "obsession"

    with factory.open(uri, "w") as second_write:
        second_write.metadata["possession"] = "funny thing about opinions"
        del second_write.metadata["possession"]

    with factory.open(uri, "r") as reader:
        assert non_soma_metadata(reader) == {}


def non_soma_metadata(obj) -> Dict[str, Any]:
    return {k: v for (k, v) in obj.metadata.items() if not k.startswith("soma_")}


@pytest.mark.parametrize(
    "test_value",
    [
        True,
        False,
        0,
        1.00000001,
        -3.1415,
        "",
        "a string",
        math.nan,
        math.inf,
        -math.inf,
    ],
)
def test_metadata_marshalling_OK(soma_object, test_value):
    """
    Test the various data type marshalling we expect to work,
    which is any Arrow primitive and Arrow strings
    """
    soma_object.metadata["test_value"] = test_value
    assert "test_value" in soma_object.metadata

    val = soma_object.metadata["test_value"]
    if type(test_value) is float and math.isnan(test_value):
        # By definition, NaN != NaN, so we can't just compare
        assert math.isnan(val)
    else:
        assert val == test_value


@pytest.mark.parametrize(
    "test_value",
    [["a", "b", "c"], {"a": False}],
)
def test_metadata_marshalling_FAIL(soma_object, test_value):
    """Test the various data type marshalling we expect to FAIL"""

    with pytest.raises(TypeError):
        soma_object.metadata["test_value"] = test_value

    assert "test_value" not in soma_object.metadata
