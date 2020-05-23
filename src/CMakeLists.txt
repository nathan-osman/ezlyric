configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")

set(SRC
    main.cpp
    mainwindow.h
    mainwindow.cpp
    resource.qrc
    song.h
    song.cpp
)

if(WIN32)
    set(SRC ${SRC} resource.rc)
endif()

add_executable(ezlyric WIN32 ${SRC})

set_target_properties(ezlyric PROPERTIES
    CXX_STANDARD          11
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(ezlyric PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(ezlyric Qt5::Widgets)

install(TARGETS ezlyric RUNTIME DESTINATION bin)

set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
include(InstallRequiredSystemLibraries)
