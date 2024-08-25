project('loki', 'cpp',
        default_options : [
            'default_library=static',
            'cpp_std=c++20',
        ])

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
    dependency('blp'),
    dependency('freeimage'),
    dependency('squish'),
]

c_compiler = meson.get_compiler('c')

if host_machine.system() == 'windows'
    dependencies += [declare_dependency(link_args : ['-lWs2_32'])]
    dependencies += [c_compiler.find_library('dbghelp')]
endif

sources = [
    'main.cpp',
    'engine/engine_app.cpp',
    'engine/utils/string_manager.cpp',
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
    'engine/datasource/mpq/mpq_file_manager.cpp',
    'engine/asset/asset.cpp',
    'engine/model/m_2_model.cpp',
    'engine/model/m_2_model_view.cpp',
    'engine/texture/blp_texture.cpp',
    'engine/mt/main_thread_queue.cpp',
    'game/game_app.cpp',
]

executable('loki', sources,
           dependencies : dependencies,
           cpp_args : '-DWIN32_LEAN_AND_MEAN')