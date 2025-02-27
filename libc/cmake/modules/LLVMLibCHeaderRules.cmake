# A rule for self contained header file targets.
# This rule merely copies the header file from the current source directory to
# the current binary directory.
# Usage:
#     add_header(
#       <target name>
#       HDR <header file>
#     )
function(add_header target_name)
  cmake_parse_arguments(
    "ADD_HEADER"
    ""    # No optional arguments
    "HDR" # Single value arguments
    "DEPENDS"
    ${ARGN}
  )
  if(NOT ADD_HEADER_HDR)
    message(FATAL_ERROR "'add_header' rules requires the HDR argument specifying a headef file.")
  endif()

  set(absolute_path ${CMAKE_CURRENT_SOURCE_DIR}/${ADD_HEADER_HDR})
  file(RELATIVE_PATH relative_path ${LIBC_INCLUDE_SOURCE_DIR} ${absolute_path})
  set(dest_file ${LIBC_INCLUDE_DIR}/${relative_path})
  set(src_file ${CMAKE_CURRENT_SOURCE_DIR}/${ADD_HEADER_HDR})

  add_custom_command(
    OUTPUT ${dest_file}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${src_file} ${dest_file}
    DEPENDS ${src_file}
  )

  get_fq_target_name(${target_name} fq_target_name)
  set(copied_hdr_target ${fq_target_name}.__copied_hdr__)
  add_custom_target(
    ${copied_hdr_target}
    DEPENDS ${dest_file}
  )

  if(ADD_HEADER_DEPENDS)
    get_fq_deps_list(fq_deps_list ${ADD_HEADER_DEPENDS})
    # Dependencies of a add_header target can only be another add_header target
    # or an add_gen_header target.
    foreach(dep IN LISTS fq_deps_list)
      get_target_property(header_file ${dep} HEADER_FILE_PATH)
      if(NOT header_file)
        message(FATAL_ERROR "Invalid dependency '${dep}' for '${fq_target_name}'.")
      endif()
    endforeach()
    add_dependencies(
      ${copied_hdr_target} ${fq_deps_list}
    )
  endif()

  add_header_library(
    ${target_name}
    HDRS
      ${dest_file}
  )
  set_target_properties(
    ${fq_target_name}
    PROPERTIES
      HEADER_FILE_PATH ${dest_file}
      DEPS "${fq_deps_list}"
  )
endfunction(add_header)

# A rule for generated header file targets.
# Usage:
#     add_gen_header(
#       <target name>
#       DEF_FILE <.h.def file>
#       GEN_HDR <generated header file name>
#       PARAMS <list of name=value pairs>
#       DATA_FILES <list input data files>
#     )
function(add_gen_header target_name)
  cmake_parse_arguments(
    "ADD_GEN_HDR"
    "PUBLIC" # No optional arguments
    "DEF_FILE;GEN_HDR" # Single value arguments
    "PARAMS;DATA_FILES;DEPENDS"     # Multi value arguments
    ${ARGN}
  )
  get_fq_target_name(${target_name} fq_target_name)
  if(NOT LLVM_LIBC_FULL_BUILD)
    # We don't want to use generated headers if we are doing a non-full-build.
    add_custom_target(${fq_target_name})
    return()
  endif()
  if(NOT ADD_GEN_HDR_DEF_FILE)
    message(FATAL_ERROR "`add_gen_hdr` rule requires DEF_FILE to be specified.")
  endif()
  if(NOT ADD_GEN_HDR_GEN_HDR)
    message(FATAL_ERROR "`add_gen_hdr` rule requires GEN_HDR to be specified.")
  endif()

  set(absolute_path ${CMAKE_CURRENT_SOURCE_DIR}/${ADD_GEN_HDR_GEN_HDR})
  file(RELATIVE_PATH relative_path ${LIBC_INCLUDE_SOURCE_DIR} ${absolute_path})
  set(out_file ${LIBC_INCLUDE_DIR}/${relative_path})
  set(in_file ${CMAKE_CURRENT_SOURCE_DIR}/${ADD_GEN_HDR_DEF_FILE})

  set(fq_data_files "")
  if(ADD_GEN_HDR_DATA_FILES)
    foreach(data_file IN LISTS ADD_GEN_HDR_DATA_FILES)
      list(APPEND fq_data_files "${CMAKE_CURRENT_SOURCE_DIR}/${data_file}")
    endforeach(data_file)
  endif()

  set(replacement_params "")
  if(ADD_GEN_HDR_PARAMS)
    list(APPEND replacement_params "--args" ${ADD_GEN_HDR_PARAMS})
  endif()

  set(gen_hdr_script "${LIBC_BUILD_SCRIPTS_DIR}/gen_hdr.py")

  file(GLOB td_includes ${LIBC_SOURCE_DIR}/spec/*.td)

  set(ENTRYPOINT_NAME_LIST_ARG ${TARGET_ENTRYPOINT_NAME_LIST})
  list(TRANSFORM ENTRYPOINT_NAME_LIST_ARG PREPEND "--e=")

  if(LIBC_HDRGEN_EXE)
    set(hdrgen_exe ${LIBC_HDRGEN_EXE})
  else()
    set(hdrgen_exe ${LIBC_TABLEGEN_EXE})
    set(hdrgen_deps "${LIBC_TABLEGEN_EXE};${LIBC_TABLEGEN_TARGET}")
  endif()
  add_custom_command(
    OUTPUT ${out_file}
    COMMAND ${hdrgen_exe} -o ${out_file} --header ${ADD_GEN_HDR_GEN_HDR}
            --def ${in_file} ${replacement_params} -I ${LIBC_SOURCE_DIR}
           ${ENTRYPOINT_NAME_LIST_ARG}
           ${LIBC_SOURCE_DIR}/config/${LIBC_TARGET_OS}/api.td

    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${in_file} ${fq_data_files} ${td_includes}
            ${LIBC_SOURCE_DIR}/config/${LIBC_TARGET_OS}/api.td
            ${hdrgen_deps}
  )

  if(LIBC_TARGET_ARCHITECTURE_IS_GPU)
    file(MAKE_DIRECTORY ${LIBC_INCLUDE_DIR}/llvm-libc-decls)
    set(decl_out_file ${LIBC_INCLUDE_DIR}/llvm-libc-decls/${relative_path})
    add_custom_command(
      OUTPUT ${decl_out_file}
      COMMAND ${hdrgen_exe} -o ${decl_out_file}
              --header ${ADD_GEN_HDR_GEN_HDR} --def ${in_file} --export-decls
              ${replacement_params} -I ${LIBC_SOURCE_DIR} ${ENTRYPOINT_NAME_LIST_ARG}
              ${LIBC_SOURCE_DIR}/config/${LIBC_TARGET_OS}/api.td

      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      DEPENDS ${in_file} ${fq_data_files} ${td_includes}
              ${LIBC_SOURCE_DIR}/config/${LIBC_TARGET_OS}/api.td
              ${hdrgen_deps}
    )
  endif()

  if(ADD_GEN_HDR_DEPENDS)
    get_fq_deps_list(fq_deps_list ${ADD_GEN_HDR_DEPENDS})
    # Dependencies of a add_header target can only be another add_gen_header target
    # or an add_header target.
    foreach(dep IN LISTS fq_deps_list)
      get_target_property(header_file ${dep} HEADER_FILE_PATH)
      if(NOT header_file)
        message(FATAL_ERROR "Invalid dependency '${dep}' for '${fq_target_name}'.")
      endif()
    endforeach()
  endif()
  set(generated_hdr_target ${fq_target_name}.__generated_hdr__)
  add_custom_target(
    ${generated_hdr_target}
    DEPENDS ${out_file} ${fq_deps_list} ${decl_out_file}
  )

  add_header_library(
    ${target_name}
    HDRS
      ${out_file}
  )

  set_target_properties(
    ${fq_target_name}
    PROPERTIES
      HEADER_FILE_PATH ${out_file}
      DECLS_FILE_PATH "${decl_out_file}"
      DEPS "${fq_deps_list}"
  )
endfunction(add_gen_header)
