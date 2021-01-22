project('gpu-pm', 'cpp', default_options : 'std=c++11 -g3')

incdir = 'include/'
pfx = 'src/'
sources = [
    pfx + 'main.cpp',
    pfx + 'gpu_instance.cpp',
    pfx + 'renderer.cpp',
    pfx + 'mesh.cpp',
    pfx + 'camera.cpp',
    pfx + 'light.cpp',
    pfx + 'texture.cpp',
    pfx + 'material.cpp',
    pfx + 'scene.cpp'
]

assimp = dependency('assimp', version : '>=5.0.0')
glm = dependency('glm', version : '>=0.9.9')
vulkan = dependency('vulkan', version : '>=1.1')

executable('demo', sources,
include_directories : incdir,
dependencies : [
    assimp,
    glm,
    vulkan
])