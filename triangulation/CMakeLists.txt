include(GNUInstallDirs)

set(SOURCES
    src/dcel_polygon2d.cpp
    src/decompose_to_monotones.cpp
    src/geom_utils.cpp
    src/polygon2d.cpp
    src/triangulate_monotone.cpp
    src/triangulation.cpp)
set(PUBLIC_HEADERS
    include/triangulation.h
    include/triangulation_base_geometry.h)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES
    PUBLIC_HEADER "${PUBLIC_HEADERS}")
target_include_directories(${PROJECT_NAME} PRIVATE include src)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

configure_file(triangulation.pc.in triangulation.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/triangulation/triangulation.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
