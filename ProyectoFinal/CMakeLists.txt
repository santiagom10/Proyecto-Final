cmake_minimum_required(VERSION 3.19)
project(ProyectoFinal LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(ProyectoFinal
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    jugador.h jugador.cpp
    obstaculo.h obstaculo.cpp
)

qt_add_resources(ProyectoFinal "resources"
    PREFIX "/"
    FILES
        Imagenes/Escenario1.png
        Imagenes/Boton_inicio.png
        Imagenes/Boton_personajes.png
        Imagenes/Boton_salir.png
        Imagenes/Fondo_menu.png
        Imagenes/Ozzy.png
        Imagenes/Skater.png
)

target_link_libraries(ProyectoFinal
    PRIVATE
        Qt::Core
        Qt::Widgets
)

include(GNUInstallDirs)
install(TARGETS ProyectoFinal
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET ProyectoFinal
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
