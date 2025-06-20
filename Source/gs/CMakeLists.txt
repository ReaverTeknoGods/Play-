# Create optimized graphics library
add_library(GraphicsCore STATIC
    # Your GS files here
)

# Apply aggressive optimizations to graphics code
if(MSVC)
    target_compile_options(GraphicsCore PRIVATE
        $<$<CONFIG:Release>:/arch:AVX2>
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Release>:/Oi>
        $<$<CONFIG:Release>:/Ot>
        $<$<CONFIG:Release>:/fp:fast>
        $<$<CONFIG:Release>:/favor:INTEL64>
    )
    
    # Enable specific optimizations for OpenGL/Vulkan renderers
    target_compile_definitions(GraphicsCore PRIVATE
        $<$<CONFIG:Release>:NDEBUG>
        $<$<CONFIG:Release>:GRAPHICS_OPTIMIZED>
    )
endif()