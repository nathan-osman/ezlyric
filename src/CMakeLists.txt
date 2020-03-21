set(SRC
    main.cpp
    mainwindow.h
    mainwindow.cpp
    resource.qrc)

add_executable(ezlyric WIN32 ${SRC})
set_target_properties(ezlyric PROPERTIES
    CXX_STANDARD          11
    CXX_STANDARD_REQUIRED ON
)

target_link_libraries(ezlyric Qt5::Widgets)

install(TARGETS ezlyric RUNTIME DESTINATION bin)

set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
include(InstallRequiredSystemLibraries)
