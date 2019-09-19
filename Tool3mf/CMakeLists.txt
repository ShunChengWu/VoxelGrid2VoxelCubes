if (NOT libm3f_DIR)
        # configure and install lib3mf (if not already done by super-project)
        configure_file(${CMAKE_SOURCE_DIR}/cmake/lib3mf.CMakeLists.txt.in ${CMAKE_BINARY_DIR}/lib3mf/download/CMakeLists.txt)
        execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
                RESULT_VARIABLE result
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/lib3mf/download)
        if(result)
            message(FATAL_ERROR "CMake step for lib3mf failed: ${result}")
        endif()
        execute_process(COMMAND ${CMAKE_COMMAND} --build .
                RESULT_VARIABLE result
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/lib3mf/download)
        if(result)
            message(FATAL_ERROR "Build step for lib3mf failed: ${result}")
        endif()
        set(libm3f_DIR ${CMAKE_BINARY_DIR}/lib3mf/include)
endif()

message("CMAKE_BINARY_DIR: " ${CMAKE_BINARY_DIR})

ADD_LIBRARY(Tool3MF Tool3mf.cpp Tool3mf/tool3mf.h Tool3mf/tool3mf.hpp)
TARGET_INCLUDE_DIRECTORIES(Tool3MF PUBLIC ${CMAKE_BINARY_DIR}/lib3mf/include)
#TARGET_LINK_DIRECTORIES(Tool3MF PUBLIC ${CMAKE_BINARY_DIR}/lib3mf/lib)
TARGET_INCLUDE_DIRECTORIES(Tool3MF PUBLIC ./)
TARGET_LINK_LIBRARIES(Tool3MF PUBLIC ${CMAKE_BINARY_DIR}/lib3mf/src/lib3mf-build/lib3MF_s.a)