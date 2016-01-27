add_executable(assignment assignment.cpp)
add_test(test.assignment assignment)

add_executable(braced_init_constructors braced_init_constructors.cpp)
add_test(test.braced_init_constructors braced_init_constructors)

add_executable(creation creation.cpp)
add_test(test.creation creation)

add_executable(get get.cpp)
add_test(test.get get)

add_executable(invoke invoke.cpp)
add_test(test.invoke invoke)

add_executable(io io.cpp)
add_test(test.io io)

add_executable(operator operator.cpp)
add_test(test.operator operator)

add_executable(perfect_forwarding_constructors perfect_forwarding_constructors.cpp)
add_test(test.perfect_forwarding_constructors perfect_forwarding_constructors)

add_executable(piecewise_forwarding_constructors piecewise_forwarding_constructors.cpp)
add_test(test.piecewise_forwarding_constructors piecewise_forwarding_constructors)

add_executable(swap swap.cpp)
add_test(test.swap swap)

add_executable(tuple_forwarding_constructors tuple_forwarding_constructors.cpp)
add_test(test.tuple_forwarding_constructors tuple_forwarding_constructors)