cmake_minimum_required(VERSION 3.10)
project(projet_mcts)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Répertoires contenant les fichiers d'en-tête
include_directories(include)

# Fichiers source à compiler (extensibles)
set(SOURCES
    src/P4.cpp
    src/JHP4.cpp
)

# Exécutable principal (jeu des allumettes)
add_executable(main src/main.cpp ${SOURCES})

# Exécutable pour le championnat entre JMCTS2
add_executable(test_jmcts2 src/test_jmcts2.cpp ${SOURCES})

# Exécutable pour une partie P4 entre humain et IA
add_executable(test_p4 tests/test_p4.cpp ${SOURCES})
