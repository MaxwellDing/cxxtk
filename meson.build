project('graph',
        ['cpp', 'c'],
        default_options : ['cpp_std=c++17'])

add_project_arguments('-fPIC', language : 'cpp')

cc = meson.get_compiler('cpp')

thread_dep = dependency('threads')
# curl_dep = cc.find_library('curl', dirs : '/usr/lib/x86_64-linux-gnu', required : true)

# foo_lib = shared_library('foo', 'temp.cpp')
# foo_dep = declare_dependency(link_with : foo_lib)

gsl_inc = './3rdparty/gsl-lite/include'
tk_inc = './src'

# libs = [thread_dep, curl_dep, foo_dep]
libs = [thread_dep]

incs = include_directories([gsl_inc, tk_inc])

src = ['tests/test_graph.cpp']
executable('graph_test',
           sources : src,
           include_directories : incs,
           dependencies : libs)

src = ['tests/test_obj_pool.cpp']
executable('objpool_test',
           sources : src,
           include_directories : incs,
           dependencies : libs)
