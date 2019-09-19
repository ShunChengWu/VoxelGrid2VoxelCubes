file(GLOB SRC src/*.c src/*.cc src/*.cpp src/*.h src/*.hpp)
file(GLOB HEADER_SOURCES ../Utils/*.h ../Utils/*.hpp)
file(GLOB BIN_SOURCES *.cpp *.cu *.cc)
foreach(filename ${BIN_SOURCES})
  get_filename_component(name ${filename} NAME_WE)
  add_executable(${name} ${filename} ${HEADER_SOURCES} ${SRC})

    IF(USE_3MF)
      target_link_libraries(${name} PUBLIC Tool3MF)
    ENDIF()
endforeach()

