cmake_minimum_required (VERSION 3.14)

project(Python)

set(HEADER_FILES
    Include/abstract.h
    Include/boolobject.h
    Include/bytearrayobject.h
    Include/bytesobject.h
    Include/ceval.h
    # Include/code.h
    Include/codecs.h
    Include/compile.h
    Include/complexobject.h
    Include/datetime.h
    Include/descrobject.h
    Include/dictobject.h
    Include/dynamic_annotations.h
    Include/enumobject.h
    Include/errcode.h
    Include/fileobject.h
    Include/fileutils.h
    Include/floatobject.h
    Include/frameobject.h
    Include/import.h
    Include/intrcheck.h
    Include/iterobject.h
    Include/listobject.h
    Include/longobject.h
    Include/marshal.h
    Include/memoryobject.h
    Include/methodobject.h
    Include/modsupport.h
    Include/moduleobject.h
    Include/object.h
    Include/objimpl.h
    Include/osdefs.h
    Include/osmodule.h
    Include/patchlevel.h
    Include/py_curses.h
    Include/pybuffer.h
    Include/pycapsule.h
    Include/pyerrors.h
    Include/pyexpat.h
    Include/pylifecycle.h
    Include/pymath.h
    Include/pymacro.h
    Include/pymem.h
    Include/pyport.h
    Include/pystate.h
    Include/pystats.h
    Include/pystrcmp.h
    Include/pystrtod.h
    Include/Python.h
    Include/pythonrun.h
    Include/pythread.h
    Include/pytypedefs.h
    Include/rangeobject.h
    Include/setobject.h
    Include/sliceobject.h
    Include/structmember.h
    Include/structseq.h
    # Include/symtable.h
    Include/sysmodule.h
    Include/traceback.h
    Include/tracemalloc.h
    Include/tupleobject.h
    Include/unicodeobject.h
    Include/weakrefobject.h
    Modules/_math.h
    Modules/rotatingtree.h
    Modules/_io/_iomodule.h
    Modules/cjkcodecs/alg_jisx0201.h
    Modules/cjkcodecs/cjkcodecs.h
    Modules/cjkcodecs/emu_jisx0213_2000.h
    Modules/cjkcodecs/mappings_cn.h
    Modules/cjkcodecs/mappings_hk.h
    Modules/cjkcodecs/mappings_jisx0213_pair.h
    Modules/cjkcodecs/mappings_jp.h
    Modules/cjkcodecs/mappings_kr.h
    Modules/cjkcodecs/mappings_tw.h
    Modules/cjkcodecs/multibytecodec.h
    Objects/stringlib/count.h
    Objects/stringlib/fastsearch.h
    Objects/stringlib/find.h
    Objects/stringlib/partition.h
    Objects/stringlib/replace.h
    Objects/stringlib/split.h
    Objects/unicodetype_db.h
    Parser/tokenizer.h
    # PC/errmap.h
    # PC/pyconfig.h
    Python/importdl.h
    Python/stdlib_module_names.h
    Python/thread_nt.h
    Python/condvar.h
    Include/pyhash.h
    Include/interpreteridobject.h
    # Modules/hashtable.h
    Parser/pegen.h
    Parser/string_parser.h
    Include/pyframe.h
    Include/cpython/abstract.h      
    Include/cpython/bytearrayobject.h      
    Include/cpython/bytesobject.h      
    Include/cpython/cellobject.h      
    Include/cpython/ceval.h      
    Include/cpython/classobject.h      
    Include/cpython/code.h      
    Include/cpython/compile.h
    Include/cpython/complexobject.h
    Include/cpython/context.h
    Include/cpython/descrobject.h      
    Include/cpython/dictobject.h      
    Include/cpython/fileobject.h      
    Include/cpython/fileutils.h      
    Include/cpython/floatobject.h      
    Include/cpython/import.h      
    Include/cpython/listobject.h      
    Include/cpython/longintrepr.h
    Include/cpython/longobject.h
    Include/cpython/memoryobject.h
    Include/cpython/odictobject.h
    Include/cpython/unicodeobject.h      
    Include/cpython/warnings.h      
    Include/cpython/weakrefobject.h      
    Include/cpython/methodobject.h      
    Include/cpython/modsupport.h      
    Include/cpython/objimpl.h      
    Include/cpython/object.h      
    # Include/cpython/parser_interface.h
    Include/cpython/picklebufobject.h
    # Include/cpython/pyarena.h
    Include/cpython/pyctype.h
    Include/cpython/pydebug.h
    Include/cpython/pyerrors.h      
    Include/cpython/pyfpe.h
    Include/cpython/pymem.h      
    Include/cpython/pyframe.h      
    Include/cpython/pylifecycle.h      
    Include/cpython/pytime.h      
    Include/cpython/tupleobject.h      
    Include/cpython/traceback.h      
    Include/cpython/frameobject.h      
    Include/cpython/funcobject.h      
    Include/cpython/genobject.h
    Include/cpython/interpreteridobject.h      
    Include/cpython/pythonrun.h      
    Include/cpython/pythread.h      
    Include/cpython/setobject.h      
    Include/cpython/sysmodule.h      
    Include/cpython/pystate.h      
    Include/cpython/initconfig.h      
    Include/internal/pycore_unicodeobject.h      
    Include/internal/pycore_unicodeobject_generated.h      
    Include/internal/pycore_warnings.h      
    Include/internal/pycore_abstract.h      
    Include/internal/pycore_asdl.h      
    Include/internal/pycore_ast.h      
    Include/internal/pycore_ast_state.h      
    Include/internal/pycore_atexit.h      
    Include/internal/pycore_atomic.h      
    Include/internal/pycore_atomic_funcs.h
    Include/internal/pycore_bitutils.h      
    Include/internal/pycore_bytes_methods.h      
    Include/internal/pycore_bytesobject.h      
    Include/internal/pycore_call.h      
    Include/internal/pycore_ceval.h      
    Include/internal/pycore_ceval_state.h      
    Include/internal/pycore_code.h      
    Include/internal/pycore_compile.h      
    Include/internal/pycore_condvar.h      
    Include/internal/pycore_context.h      
    Include/internal/pycore_descrobject.h      
    Include/internal/pycore_dict.h      
    Include/internal/pycore_dict_state.h      
    Include/internal/pycore_dtoa.h      
    Include/internal/pycore_exceptions.h      
    Include/internal/pycore_faulthandler.h      
    Include/internal/pycore_fileutils.h      
    Include/internal/pycore_fileutils_windows.h      
    Include/internal/pycore_floatobject.h      
    Include/internal/pycore_format.h      
    Include/internal/pycore_function.h      
    Include/internal/pycore_gc.h      
    Include/internal/pycore_genobject.h      
    Include/internal/pycore_getopt.h      
    Include/internal/pycore_gil.h      
    Include/internal/pycore_global_objects.h      
    Include/internal/pycore_global_objects_fini_generated.h      
    Include/internal/pycore_hamt.h      
    Include/internal/pycore_hashtable.h      
    Include/internal/pycore_import.h      
    Include/internal/pycore_initconfig.h      
    Include/internal/pycore_interp.h      
    Include/internal/pycore_intrinsics.h      
    Include/internal/pycore_list.h      
    Include/internal/pycore_long.h      
    Include/internal/pycore_moduleobject.h      
    Include/internal/pycore_namespace.h
    Include/internal/pycore_object.h      
    Include/internal/pycore_object_state.h      
    Include/internal/pycore_obmalloc.h      
    Include/internal/pycore_obmalloc_init.h      
    Include/internal/pycore_pathconfig.h      
    Include/internal/pycore_pyarena.h      
    Include/internal/pycore_pyerrors.h      
    Include/internal/pycore_pyhash.h      
    Include/internal/pycore_pylifecycle.h      
    Include/internal/pycore_pymem.h      
    Include/internal/pycore_pymem_init.h      
    Include/internal/pycore_pystate.h      
    Include/internal/pycore_pythread.h      
    Include/internal/pycore_range.h      
    Include/internal/pycore_runtime.h      
    Include/internal/pycore_runtime_init.h      
    Include/internal/pycore_runtime_init_generated.h      
    Include/internal/pycore_signal.h      
    Include/internal/pycore_sliceobject.h      
    Include/internal/pycore_strhex.h      
    Include/internal/pycore_sysmodule.h      
    Include/internal/pycore_symtable.h      
    Include/internal/pycore_time.h      
    Include/internal/pycore_token.h      
    Include/internal/pycore_traceback.h      
    Include/internal/pycore_tracemalloc.h      
    Include/internal/pycore_tuple.h      
    Include/internal/pycore_typeobject.h      
    Include/internal/pycore_typevarobject.h      
    Include/internal/pycore_ucnhash.h      
    Include/internal/pycore_unionobject.h      
    # $(zlibDir)/crc32.h
    # $(zlibDir)/deflate.h
    # $(zlibDir)/inffast.h
    # $(zlibDir)/inffixed.h
    # $(zlibDir)/inflate.h
    # $(zlibDir)/inftrees.h
    # $(zlibDir)/trees.h
    # $(zlibDir)/zconf.h
    # $(zlibDir)/zconf.in.h
    # $(zlibDir)/zlib.h
    # $(zlibDir)/zutil.h
    Include/internal/pycore_structseq.h
)

set(SOURCE_FILES
    Modules/_abc.c
    Modules/_bisectmodule.c
    Modules/_blake2/blake2module.c
    Modules/_blake2/blake2b_impl.c
    Modules/_blake2/blake2s_impl.c
    Modules/_codecsmodule.c
    Modules/_collectionsmodule.c
    Modules/_csv.c
    Modules/_functoolsmodule.c
    Modules/_hacl/Hacl_Hash_SHA1.c
    Modules/_hacl/Hacl_Hash_SHA2.c
    Modules/_hacl/Hacl_Hash_SHA3.c
    Modules/_heapqmodule.c
    Modules/_json.c
    Modules/_localemodule.c
    Modules/_lsprof.c
    Modules/_pickle.c
    Modules/_randommodule.c
    Modules/_sre/sre.c
    Modules/_sre/sre.h
    Modules/_sre/sre_constants.h
    Modules/_sre/sre_lib.h
    Modules/_statisticsmodule.c
    Modules/_typingmodule.c
    Modules/_struct.c
    Modules/_weakref.c
    Modules/arraymodule.c
    Modules/atexitmodule.c
    Modules/audioop.c
    Modules/binascii.c
    Modules/cmathmodule.c
    Modules/_datetimemodule.c
    Modules/errnomodule.c
    Modules/faulthandler.c
    Modules/gcmodule.c
    Modules/itertoolsmodule.c
    Modules/main.c
    Modules/mathmodule.c
    Modules/_hacl/Hacl_Hash_MD5.c
    Modules/md5module.c
    Modules/mmapmodule.c
    Modules/_operator.c
    Modules/posixmodule.c
    Modules/rotatingtree.c
    Modules/sha1module.c
    Modules/sha2module.c
    Modules/sha3module.c
    Modules/signalmodule.c
    Modules/symtablemodule.c
    Modules/_threadmodule.c
    Modules/timemodule.c
    Modules/xxsubtype.c
    # Modules/zlibmodule.c
    Modules/_io/fileio.c
    Modules/_io/bytesio.c
    Modules/_io/stringio.c
    Modules/_io/bufferedio.c
    Modules/_io/iobase.c
    Modules/_io/textio.c
    Modules/_io/winconsoleio.c
    Modules/_io/_iomodule.c
    Modules/cjkcodecs/_codecs_cn.c
    Modules/cjkcodecs/_codecs_hk.c
    Modules/cjkcodecs/_codecs_iso2022.c
    Modules/cjkcodecs/_codecs_jp.c
    Modules/cjkcodecs/_codecs_kr.c
    Modules/cjkcodecs/_codecs_tw.c
    Modules/cjkcodecs/multibytecodec.c
    Objects/abstract.c
    # Objects/accu.c
    Objects/boolobject.c
    Objects/bytes_methods.c
    Objects/bytearrayobject.c
    Objects/bytesobject.c
    Objects/call.c
    Objects/capsule.c
    Objects/cellobject.c
    Objects/classobject.c
    Objects/codeobject.c
    Objects/complexobject.c
    Objects/descrobject.c
    Objects/dictobject.c
    Objects/enumobject.c
    Objects/exceptions.c
    Objects/fileobject.c
    Objects/floatobject.c
    Objects/frameobject.c
    Objects/funcobject.c
    Objects/genobject.c
    Objects/iterobject.c
    Objects/listobject.c
    Objects/longobject.c
    Objects/memoryobject.c
    Objects/methodobject.c
    Objects/moduleobject.c
    Objects/object.c
    Objects/obmalloc.c
    Objects/picklebufobject.c
    Objects/rangeobject.c
    Objects/setobject.c
    Objects/sliceobject.c
    Objects/structseq.c
    Objects/tupleobject.c
    Objects/typeobject.c
    Objects/unicodectype.c
    Objects/unicodeobject.c
    Objects/weakrefobject.c
    Parser/myreadline.c
    Parser/tokenizer.c
    Parser/token.c
    # PC/winreg.c
    # PC/config.c
    # PC/dl_nt.c
    # PC/msvcrtmodule.c
    Python/_warnings.c
    Python/asdl.c
    Python/assemble.c
    Python/ast.c
    Python/ast_opt.c
    Python/ast_unparse.c
    Python/bltinmodule.c
    Python/ceval.c
    Python/codecs.c
    Python/compile.c
    Python/dynamic_annotations.c
    Python/dynload_win.c
    Python/errors.c
    Python/fileutils.c
    Python/flowgraph.c
    Python/formatter_unicode.c
    Python/frozen.c
    Python/future.c
    Python/getargs.c
    Python/getcompiler.c
    Python/getcopyright.c
    Python/getopt.c
    Python/getplatform.c
    Python/getversion.c
    Python/ceval_gil.c
    Python/hashtable.c
    Python/import.c
    Python/importdl.c
    Python/initconfig.c
    Python/intrinsics.c
    Python/instrumentation.c
    Python/legacy_tracing.c
    Python/marshal.c
    Python/modsupport.c
    Python/mysnprintf.c
    Python/mystrtoul.c
    Python/pathconfig.c
    Python/perf_trampoline.c
    Python/preconfig.c
    Python/pyarena.c
    Python/pyctype.c
    Python/pyfpe.c
    Python/pylifecycle.c
    Python/pymath.c
    Python/pytime.c
    Python/pystate.c
    Python/pystrcmp.c
    Python/pystrhex.c
    Python/pystrtod.c
    Python/dtoa.c
    Python/Python-ast.c
    Python/pythonrun.c
    Python/specialize.c
    Python/structmember.c
    Python/symtable.c
    Python/sysmodule.c
    Python/thread.c
    Python/traceback.c
    Python/tracemalloc.c
    Python/bootstrap_hash.c
    Modules/_winapi.c
    Modules/_stat.c
    Python/pyhash.c
    Objects/namespaceobject.c
    Objects/interpreteridobject.c
    Modules/_opcode.c
    Modules/_tracemalloc.c
    # PC/invalid_parameter_handler.c
    Objects/odictobject.c
    Modules/_contextvarsmodule.c
    # $(zlibDir)/adler32.c
    # $(zlibDir)/compress.c
    # $(zlibDir)/crc32.c
    # $(zlibDir)/deflate.c
    # $(zlibDir)/infback.c
    # $(zlibDir)/inffast.c
    # $(zlibDir)/inflate.c
    # $(zlibDir)/inftrees.c
    # $(zlibDir)/trees.c
    # $(zlibDir)/uncompr.c
    # $(zlibDir)/zutil.c
    Python/hamt.c
    Parser/parser.c
    Parser/pegen.c
    Parser/pegen_errors.c
    Parser/action_helpers.c
    Parser/peg_api.c
    Modules/_xxsubinterpretersmodule.c
    Modules/_xxinterpchannelsmodule.c
    Parser/string_parser.c
    Python/context.c
    Objects/genericaliasobject.c
    Objects/typevarobject.c
    Objects/unionobject.c
    Python/frame.c
    Modules/getpath.c
    Python/suggestions.c
    Python/Python-tokenize.c
    Modules/getbuildinfo.c
)
add_compile_definitions(Py_BUILD_CORE)
add_compile_definitions(Py_BUILD_CORE_BUILTIN)
include_directories("${PROJECT_SOURCE_DIR}/Include")
include_directories("${PROJECT_SOURCE_DIR}/Include/internal")
include_directories("${PROJECT_SOURCE_DIR}/Modules/_hacl/include")
include_directories("${PROJECT_SOURCE_DIR}/Modules/_hacl/internal")
include_directories("${PROJECT_SOURCE_DIR}/Python")
if (WIN32)
    include_directories("${PROJECT_SOURCE_DIR}/PC")
endif()

add_library (Python STATIC ${SOURCE_FILES} ${HEADER_FILES})

# target_include_directories(Python
#     INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#     PRIVATE   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
# )

# if (NOT MINI_URHO)
#     install (DIRECTORY include/ DESTINATION ${DEST_THIRDPARTY_HEADERS_DIR}/ FILES_MATCHING PATTERN *.h)
#     if (NOT URHO3D_MERGE_STATIC_LIBS)
#         install(TARGETS Python EXPORT Urho3D ARCHIVE DESTINATION ${DEST_ARCHIVE_DIR_CONFIG})
#     endif ()
# endif ()