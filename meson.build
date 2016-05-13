project('linear_monte_carlo', 'cpp', default_options : ['cpp_std=c++14'])

# Place path to boost here, TODO: use environment variables
inc = include_directories('')

# I so not know what kind of build target requires it
thread_dep = dependency('threads')

executable('linear_monte_carlo',
           'linear_monte_carlo.cpp',
           include_directories : inc,
           dependencies : thread_dep,
           )
