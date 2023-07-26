/**
 * @file   soma_sparse_ndarray.h
 *
 * @section LICENSE
 *
 * The MIT License
 *
 * @copyright Copyright (c) 2023 TileDB, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @section DESCRIPTION
 *
 *   This file defines the SOMASparseNDArray class.
 */

#ifndef SOMA_SPARSE_NDARRAY
#define SOMA_SPARSE_NDARRAY

#include <tiledb/tiledb>
#include "enums.h"
#include "soma_object.h"

namespace tiledbsoma {

class SOMAArray;
class ArrayBuffers;

using namespace tiledb;

class SOMASparseNDArray : public SOMAObject {
   public:
    //===================================================================
    //= public static
    //===================================================================

    /**
     * @brief Create a SOMASparseNDArray object at the given URI.
     *
     * @param uri URI to create the SOMASparseNDArray
     * @param schema TileDB ArraySchema
     * @param platform_config Optional config parameter dictionary
     * @return std::unique_ptr<SOMASparseNDArray> opened in read mode
     */
    static std::unique_ptr<SOMASparseNDArray> create(
        std::string_view uri,
        ArraySchema schema,
        std::map<std::string, std::string> platform_config = {});

    /**
     * @brief Create a SOMASparseNDArray object at the given URI.
     *
     * @param uri URI to create the SOMASparseNDArray
     * @param schema TileDB ArraySchema
     * @param ctx TileDB context
     * @return std::unique_ptr<SOMASparseNDArray> opened in read mode
     */
    static std::unique_ptr<SOMASparseNDArray> create(
        std::string_view uri, ArraySchema schema, std::shared_ptr<Context> ctx);

    /**
     * @brief Open and return a SOMASparseNDArray object at the given URI.
     *
     * @param mode read or write
     * @param uri URI to create the SOMASparseNDArray
     * @param column_names A list of column names to use as user-defined index
     * columns (e.g., ``['cell_type', 'tissue_type']``). All named columns must
     * exist in the schema, and at least one index column name is required.
     * @param platform_config Platform-specific options used to create this
     * SOMASparseNDArray
     * @param result_order Read result order: automatic (default), rowmajor, or
     * colmajor
     * @param timestamp If specified, overrides the default timestamp used to
     * open this object. If unset, uses the timestamp provided by the context.
     * @return std::unique_ptr<SOMASparseNDArray> SOMASparseNDArray
     */
    static std::unique_ptr<SOMASparseNDArray> open(
        std::string_view uri,
        OpenMode mode,
        std::map<std::string, std::string> platform_config = {},
        std::vector<std::string> column_names = {},
        ResultOrder result_order = ResultOrder::automatic,
        std::optional<std::pair<uint64_t, uint64_t>> timestamp = std::nullopt);

    /**
     * @brief Open and return a SOMASparseNDArray object at the given URI.
     *
     * @param mode read or write
     * @param ctx TileDB context
     * @param uri URI to create the SOMASparseNDArray
     * @param schema TileDB ArraySchema
     * @param column_names A list of column names to use as user-defined index
     * columns (e.g., ``['cell_type', 'tissue_type']``). All named columns must
     * exist in the schema, and at least one index column name is required.
     * @param result_order Read result order: automatic (default), rowmajor, or
     * colmajor
     * @param timestamp If specified, overrides the default timestamp used to
     * open this object. If unset, uses the timestamp provided by the context.
     * @return std::unique_ptr<SOMASparseNDArray> SOMASparseNDArray
     */
    static std::unique_ptr<SOMASparseNDArray> open(
        std::string_view uri,
        OpenMode mode,
        std::shared_ptr<Context> ctx,
        std::vector<std::string> column_names = {},
        ResultOrder result_order = ResultOrder::automatic,
        std::optional<std::pair<uint64_t, uint64_t>> timestamp = std::nullopt);

    //===================================================================
    //= public non-static
    //===================================================================

    /**
     * @brief Construct a new SOMASparseNDArray object.
     *
     * @param mode read or write
     * @param uri URI of the array
     * @param ctx TileDB context
     * @param result_order Read result order: automatic (default), rowmajor, or
     * colmajor
     * @param timestamp Timestamp
     */
    SOMASparseNDArray(
        OpenMode mode,
        std::string_view uri,
        std::shared_ptr<Context> ctx,
        std::vector<std::string> column_names,
        ResultOrder result_order,
        std::optional<std::pair<uint64_t, uint64_t>> timestamp);

    /**
     * Open the SOMASparseNDArray object.
     *
     * @param mode read or write
     * @param timestamp Timestamp
     */
    void open(
        OpenMode mode,
        std::optional<std::pair<uint64_t, uint64_t>> timestamp = std::nullopt);

    /**
     * Closes the SOMASparseNDArray object.
     */
    void close();

    /**
     * Returns the constant "SOMASparseNDArray".
     *
     * @return std::string
     */
    const std::string type() const {
        return "SOMASparseNDArray";
    }

    /**
     * Get the Context associated with the SOMASparseNDArray.
     *
     * @return std::shared_ptr<Context>
     */
    std::shared_ptr<Context> ctx();

    /**
     * Return whether the NDArray is sparse.
     *
     * @return true
     */
    bool is_sparse() {
        return true;
    };

    /**
     * @brief Get URI of the SOMASparseNDArray.
     *
     * @return std::string URI
     */
    const std::string uri() const;

    /**
     * Return data schema, in the form of a TileDB ArraySchema.
     *
     * @return std::shared_ptr<ArraySchema>
     */
    std::shared_ptr<ArraySchema> schema() const;

    /**
     * @brief Get the capacity of each dimension.
     *
     * @return A vector with length equal to the number of dimensions; each
     * value in the vector is the capcity of each dimension.
     */
    std::vector<int64_t> shape() const;

    /**
     * Return the number of dimensions.
     *
     * @return int64_t
     */
    int64_t ndim() const;

    /**
     * @brief Get the total number of shared cells in the array.
     *
     * @return uint64_t Total number of shared cells
     */
    uint64_t nnz() const;

    /**
     * @brief Read the next chunk of results from the query. If all results have
     * already been read, std::nullopt is returned.
     */
    std::optional<std::shared_ptr<ArrayBuffers>> read_next();

    /**
     * @brief Write ArrayBuffers data to the dataframe.
     * @param buffers The ArrayBuffers to write
     */
    void write(std::shared_ptr<ArrayBuffers> buffers);

   private:
    //===================================================================
    //= private non-static
    //===================================================================

    // SOMAArray
    std::shared_ptr<SOMAArray> array_;
};
}  // namespace tiledbsoma

#endif  // SOMA_SPARSE_NDARRAY