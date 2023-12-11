function(force_redefine_file_macro_for_sources targetname)
    # 获取目标的所有源文件列表
    get_target_property(source_files "${targetname}" SOURCES)
    # 对源文件列表进行迭代
    foreach(sourcefile ${source_files})
        # 获得当前源文件的编译列表，存储到defs中
        get_property(defs SOURCE "${sourcefile}"
            PROPERTY COMPILE_DEFINITIONS)
        # 获得源文件的绝对路径，存储到filepath中
        get_filename_component(filepath "${sourcefile}" ABSOLUTE)
        # 将源文件路径中的项目源目录替换为空字符串得到相对路径
        string(REPLACE ${PROJECT_SOURCE_DIR}/ "" relpath ${filepath})
        # 将__FILE__宏定义为源文件的相对路径
        list(APPEND defs "__FILE__=\"${relpath}\"")
        # 将跟新后的编译定义列表应用到源文件
        set_property(
            SOURCE "${sourcefile}"
            PROPERTY COMPILE_DEFINITIONS ${defs}
            )
    endforeach()
endfunction()