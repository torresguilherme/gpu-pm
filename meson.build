project('gpu-pm', 'cpp')

incdir = 'include/'
pfx = 'src/'
sources = [
    pfx + 'main.cpp',
    pfx + 'mesh.cpp',
    pfx + 'scene.cpp'
]

assimp = dependency('assimp', version : '>=5.0.0')

executable('demo', sources,
include_directories : incdir,
dependencies : [
    assimp
])