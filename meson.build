project('gpu-pm', 'cpp', default_options : 'std=c++11 -g3')

incdir = 'include/'
pfx = 'src/'
shader_pfx = 'shaders/'
third_party = 'third_party/'

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

shaders = [
    'main.comp'
]

assimp = dependency('assimp', version : '>=5.0.0')
glm = dependency('glm', version : '>=0.9.9')
vulkan = dependency('vulkan', version : '>=1.1')

glslc = find_program('glslc')
shader_targets = []
foreach shader : shaders
    shader_targets += custom_target(
        'shader @0@'.format(shader),
        command : [glslc, '@INPUT@', '-o', '@OUTPUT@'],
        input : shader_pfx + shader,
        output: '@BASENAME@.spv',
        install : true,
        install_dir : shader_pfx
    )
endforeach

executable('demo', sources,
include_directories : [incdir, third_party],
dependencies : [
    assimp,
    glm,
    vulkan
])