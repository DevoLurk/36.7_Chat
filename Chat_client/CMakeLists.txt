cmake_minimum_required(VERSION 3.16)

project(Chat_client LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${CMAKE_SOURCE_DIR}/Src)

if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()
if ( MSVC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
endif()

# Ищем Qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)

# Инициализация пустого списка для библиотек чата
set(CHAT_LIBRARIES)

# Список используемых модулей Qt
set(Qt6Modules Core Widgets)

# Добавляем все модули Qt в список библиотек чата
foreach(QtModule IN LISTS Qt6Modules)
  list(APPEND CHAT_LIBRARIES Qt6::${QtModule})
endforeach(QtModule)

# Добавляем ws2_32 для winsok
if(WIN32)
    list(APPEND CHAT_LIBRARIES ws2_32)
endif()

# Поиск исходных файлов
file(GLOB_RECURSE SRC_FILES Src/*.cpp)
file(GLOB_RECURSE HEADERS_FILES Src/*.h)
file(GLOB_RECURSE UI_FILES Src/*.ui)

# Генерация интерфейсов UI файлов
qt_wrap_ui(QUI ${UI_FILES})

# Создание исполнимого файла
add_executable(Chat ${SRC_FILES} ${HEADERS_FILES} ${UI_FILES})

# Линкуем проект с библиотеками из нашего списка
target_link_libraries(Chat PUBLIC ${CHAT_LIBRARIES})
