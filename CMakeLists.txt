cmake_minimum_required(VERSION 3.5)

project(ComTool VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets SerialPort Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets SerialPort Network)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ComTool
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        mytcpsocket.h mytcpsocket.cpp


        modbus_rtu.h modbus_rtu.cpp
        modbus_ascii.h modbus_ascii.cpp
        modbus_tcp.h modbus_tcp.cpp

        coap.h coap.cpp


        floatbox.h floatbox.cpp
        openroutedialog.h openroutedialog.cpp openroutedialog.ui
        modbuswidget.h modbuswidget.cpp modbuswidget.ui
        protocolwidget.h protocolwidget.cpp
        addregdialog.h addregdialog.cpp addregdialog.ui
        regsviewwidget.h regsviewwidget.cpp regsviewwidget.ui
        ModbusFrameInfo.h
        utils.h utils.cpp
        myudpsocket.h myudpsocket.cpp
        displaycommunication.h displaycommunication.cpp displaycommunication.ui
        modbuswritesinglecoildialog.h modbuswritesinglecoildialog.cpp modbuswritesinglecoildialog.ui
        modbuswritesingleregisterdialog.h modbuswritesingleregisterdialog.cpp modbuswritesingleregisterdialog.ui
        modbuswritemultiplecoilsdialog.h modbuswritemultiplecoilsdialog.cpp modbuswritemultiplecoilsdialog.ui
        modbuswritemultipleregistersdialog.h modbuswritemultipleregistersdialog.cpp modbuswritemultipleregistersdialog.ui
        errorcounterdialog.h errorcounterdialog.cpp errorcounterdialog.ui
        ModbusBase.h

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ComTool APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ComTool SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ComTool
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(ComTool PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::SerialPort Qt${QT_VERSION_MAJOR}::Network)
# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ComTool)
endif()
set_target_properties(ComTool PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ComTool
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ComTool)
endif()
