project('linear_monte_carlo', 'cpp')

# Place path to boost here
inc = include_directories('')

# I so not know what kind of build target requires it
thread_dep = dependency('threads')

executable('linear_monte_carlo',
           'linear_monte_carlo.cpp',
           include_directories : inc,
           dependencies : thread_dep,
           )
