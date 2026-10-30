cmake_minimum_required(VERSION 3.16)
project(LaPulpe)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(LaPulpe
        main.cpp
        VentanaLogin.h
        VentanaLogin.cpp
        VentanaPrincipal.h
        VentanaPrincipal.cpp
        VistaVentas.h
        VistaVentas.cpp
        VistaInventario.h
        VistaInventario.cpp
        VistaHistorial.h
        VistaHistorial.cpp
        GestorJSON.h
        GestorJSON.cpp
        IGeneradorTicket.h
        VistaUsuarios.cpp
        VistaUsuarios.h
)

target_link_libraries(LaPulpe PRIVATE Qt6::Widgets)