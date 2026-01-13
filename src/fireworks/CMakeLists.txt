# CMakeLists.txt pour le module fireworks

# Collecter tous les fichiers sources (.cpp)
file(GLOB_RECURSE FIREWORKS_SOURCES
    # Template
    template/*.cpp
    
    # Simulation
    simulation/*.cpp
    
    # Particle
    particle/*.cpp
    
    # Instance
    instance/*.cpp
    
    # Shapes
    shapes/*.cpp
)

# Headers importants à inclure explicitement pour l'IDE
set(FIREWORKS_HEADERS
    # Template
    template/PhysicsProfile.h
    template/BranchLayout.h
    template/ColorScheme.h
    template/BranchDescriptor.h
    template/GeneratedBranch.h
    template/FireworkTemplate.h
    
    # Simulation
    simulation/BranchLayoutGenerator.h
    simulation/ColorSchemeEvaluator.h
    simulation/BranchGenerator.h
    
    # Particle
    particle/Particle.h
    particle/ParticlePool.h
    
    # Instance
    instance/FireworkInstance.h
    
    # Shapes (ancien)
    shapes/Shape.h
    shapes/ShapeRegistry.h
)

# Créer la bibliothèque
add_library(FireworksLib ${FIREWORKS_SOURCES} ${FIREWORKS_HEADERS} "instance/BranchInstance.cpp" "instance/InstanceManager.cpp" "particle/ParticleEmitter.cpp" "simulation/ParticlePhysics.cpp")

# Inclure le répertoire courant pour les includes
target_include_directories(FireworksLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Lier avec les dépendances
target_link_libraries(FireworksLib PUBLIC VendorLibs)


