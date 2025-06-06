cmake_minimum_required(VERSION 3.10)

project(ComTool VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_PARALLEL_LEVEL 12)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets SerialPort Network PrintSupport)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets SerialPort Network PrintSupport)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(QASocket STATIC IMPORTED)

set(Asio_path F:/Libs/asio)

if(WIN32)
    include_directories(${Asio_path}/include)
    set_target_properties(QASocket PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/lib/win/libQASocketWin.a)
elseif(LINUX)
    set_target_properties(QASocket PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/lib/linux/libQASocketLinux.a)
endif()

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

set(TS_FILES ./translations/zh_CN.ts)


find_package(Qt${QT_VERSION_MAJOR} COMPONENTS ${REQUIRED_LIBS} LinguistTools REQUIRED)

qt6_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ComTool
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}


        modbus_rtu.h modbus_rtu.cpp
        modbus_ascii.h modbus_ascii.cpp
        modbus_tcp.h modbus_tcp.cpp
        qcustomplot.h
        qcustomplot.cpp

        floatbox.h floatbox.cpp
        openroutedialog.h openroutedialog.cpp openroutedialog.ui
        modbuswidget.h modbuswidget.cpp modbuswidget.ui
        protocolwidget.h protocolwidget.cpp
        addregdialog.h addregdialog.cpp addregdialog.ui
        regsviewwidget.h regsviewwidget.cpp regsviewwidget.ui
        ModbusFrameInfo.h
        utils.h utils.cpp
        displaycommunication.h displaycommunication.cpp displaycommunication.ui
        modbuswritesinglecoildialog.h modbuswritesinglecoildialog.cpp modbuswritesinglecoildialog.ui
        modbuswritesingleregisterdialog.h modbuswritesingleregisterdialog.cpp modbuswritesingleregisterdialog.ui
        modbuswritemultiplecoilsdialog.h modbuswritemultiplecoilsdialog.cpp modbuswritemultiplecoilsdialog.ui
        modbuswritemultipleregistersdialog.h modbuswritemultipleregistersdialog.cpp modbuswritemultipleregistersdialog.ui
        errorcounterdialog.h errorcounterdialog.cpp errorcounterdialog.ui
        ModbusBase.h

        plotwindow.h plotwindow.cpp plotwindow.ui
        ModbusRegReadDefinitions.h
        EnumHeader.h
        register_value_t.h
        plotwindow.h plotwindow.cpp plotwindow.ui
        testcenterwindow.h testcenterwindow.cpp testcenterwindow.ui
        mapdefines.h mapdefines.cpp

        ${QM_FILES}

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

if(WIN32)
    target_link_libraries(ComTool PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::SerialPort Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::PrintSupport QASocket ws2_32 wsock32)
else()
    target_link_libraries(ComTool PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::SerialPort Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::PrintSupport QASocket)
endif()

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
