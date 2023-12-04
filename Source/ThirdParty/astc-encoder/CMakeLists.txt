project(astc)
cmake_minimum_required(VERSION 3.15)
if(MSVC)
    add_compile_options("/wd4324") # Disable structure was padded due to alignment specifier
endif()
add_library(astc
    STATIC
        astcenc_averages_and_directions.cpp
        astcenc_block_sizes.cpp
        astcenc_color_quantize.cpp
        astcenc_color_unquantize.cpp
        astcenc_compress_symbolic.cpp
        astcenc_compute_variance.cpp
        astcenc_decompress_symbolic.cpp
        astcenc_diagnostic_trace.cpp
        astcenc_entry.cpp
        astcenc_find_best_partitioning.cpp
        astcenc_ideal_endpoints_and_weights.cpp
        astcenc_image.cpp
        astcenc_integer_sequence.cpp
        astcenc_mathlib.cpp
        astcenc_mathlib_softfloat.cpp
        astcenc_partition_tables.cpp
        astcenc_percentile_tables.cpp
        astcenc_pick_best_endpoint_format.cpp
        astcenc_quantization.cpp
        astcenc_symbolic_physical.cpp
        astcenc_weight_align.cpp
        astcenc_weight_quant_xfer_tables.cpp
)
target_compile_definitions(astc PRIVATE ASTCENC_DECOMPRESS_ONLY)
file(COPY astcenc.h DESTINATION ${PROJECT_BINARY_DIR}/include/)
install(DIRECTORY ${PROJECT_BINARY_DIR}/include/ DESTINATION ${DEST_THIRDPARTY_HEADERS_DIR}/ FILES_MATCHING PATTERN *.h)
if (NOT URHO3D_MERGE_STATIC_LIBS)
    install(TARGETS astc EXPORT Urho3D ARCHIVE DESTINATION ${DEST_ARCHIVE_DIR_CONFIG})
endif ()