cmake_minimum_required(VERSION 3.20)

# Helper macro
macro(add_example name)
    add_executable(${name} ${name}.cpp)

    target_link_libraries(${name}
        PRIVATE
        qr::qr
    )

    target_compile_features(${name} PRIVATE cxx_std_20)
endmacro()

add_example(hello_world)
add_example(ec_levels)
add_example(encoding_modes)
add_example(svg_output)
add_example(ascii_rendering)
add_example(ppm_export)
add_example(builder_api)
add_example(builder_advanced)
add_example(custom_renderer)
add_example(batch_generation)
add_example(batch_generator)
add_example(svg_embed)
add_example(svg_export)
