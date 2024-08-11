project('loki', 'cpp',
        default_options : [
            'default_library=static',
            'cpp_std=c++20',
        ])

# Define the Winsock library dependency

dependencies = [
    dependency('glfw3'),
    dependency('glew'),
    dependency('imgui'),
    dependency('glm'),
    dependency('spdlog'),
    dependency('cli11'),
    dependency('mimalloc'),
    dependency('stormlib'),
    dependency('sockpp'),
    dependency('glob'),
    dependency('libassert'),
    dependency('cpptrace'),
    dependency('libcrypto'),
    dependency('libssl'),
    dependency('openssl'),
    dependency('pfr'),
]

c_compiler = meson.get_compiler('c')

if host_machine.system() == 'windows'
    dependencies += [declare_dependency(link_args : ['-lWs2_32'])]
    dependencies += [c_compiler.find_library('dbghelp')]
endif

sources = [
    'main.cpp',
    'engine/engine_app.cpp',
    'engine/string_manager.cpp',
    'engine/utils/big_num.cpp',
    'engine/utils/byte_buffer.cpp',
    'engine/crypto/crypto_random.cpp',
    'engine/crypto/arc_4.cpp',
    'engine/crypto/srp_6.cpp',
    'engine/network/auth_session.cpp',
    'engine/network/auth_crypt.cpp',
    'engine/network/world_session.cpp',
    'engine/render/shader.cpp',
    'engine/datasource/mpq/mpq_archive.cpp',
    'engine/datasource/mpq/mpq_chain.cpp',
    'engine/datasource/mpq/mpq_file.cpp',
    'game/game_app.cpp',
]

executable('loki', sources,
           dependencies : dependencies)