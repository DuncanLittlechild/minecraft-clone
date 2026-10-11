#include "create_gpu_assets.h"
#include "SDL3/SDL_gpu.h"
#include <filesystem>


SDL_GPUShader* LoadShader(SDL_GPUDevice* device, const DL_ShaderInfo& dlShaderInfo)
{
    SDL_GPUShaderStage stage;
    if(dlShaderInfo.shaderFilename.contains(".vert"))
    {
        stage = SDL_GPU_SHADERSTAGE_VERTEX;
    }
    else if (dlShaderInfo.shaderFilename.contains(".frag"))
    {
        stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
    }
    else
    {
        SDL_Log("Couldn't deduce shader stage from file name: %s", dlShaderInfo.shaderFilename.c_str());
        return nullptr;
    }
    std::filesystem::path fullPath;
    #if PRODUCTION_BUILD==1
        fullPath = std::filesystem::path{SDL_GetBasePath()} / "shaders";
    #else
        fullPath = SHADER_PATH;
    #endif

    SDL_GPUShaderFormat format {SDL_GPU_SHADERFORMAT_INVALID};
    const char* entryPoint;

    SDL_GPUShaderFormat backendFormats {SDL_GetGPUShaderFormats(device)};

    if(backendFormats & SDL_GPU_SHADERFORMAT_SPIRV)
    {
        fullPath /= dlShaderInfo.shaderFilename + ".spv";
        format = SDL_GPU_SHADERFORMAT_SPIRV;
        entryPoint = "main";
    }
    else if(backendFormats & SDL_GPU_SHADERFORMAT_MSL)
    {
        fullPath /= dlShaderInfo.shaderFilename + ".msl";
        format = SDL_GPU_SHADERFORMAT_MSL;
        entryPoint = "main0";
    }
    else if(backendFormats & SDL_GPU_SHADERFORMAT_DXIL)
    {
        fullPath /= dlShaderInfo.shaderFilename + ".dxil";
        format = SDL_GPU_SHADERFORMAT_DXIL;
        entryPoint = "main";
    }
    else
    {
        SDL_Log("Couldn't find a supported shader for backend type %s!", SDL_GetGPUDeviceDriver(device));
        return nullptr;
    }
    size_t fileSize;
    void* code {SDL_LoadFile(fullPath.string().c_str(), &fileSize)};
    if (code == nullptr)
    {
        SDL_Log("Couldn't load shader file from disk!\n\t%s", SDL_GetError());
        return nullptr;
    }
    SDL_GPUShaderCreateInfo shaderInfo {SDL_GPUShaderCreateInfo{
        .code_size = fileSize,
        .code = static_cast<Uint8*>(code),
        .entrypoint = entryPoint,
        .format = format,
        .stage = stage,
        .num_samplers = dlShaderInfo.numSamplers,
        .num_storage_textures = dlShaderInfo.numStorageTextures,
        .num_storage_buffers = dlShaderInfo.numStorageBuffers,
        .num_uniform_buffers = dlShaderInfo.numUniformBuffers
    }};

    SDL_GPUShader* shader {SDL_CreateGPUShader(device, &shaderInfo)};
    if (shader == nullptr)
    {
        SDL_Log("Couldn't create shader from file %s: %s", fullPath.string().c_str(), SDL_GetError());
    }

    SDL_free(code);

    return shader;
}

bool CreateGPUVertexBuffer(SDL_GPUDevice* device, const std::vector<Vertex>& vertices, SDL_GPUBuffer** buffer)
{
    return CreateGPUBuffer(device, std::span<const Vertex>(vertices), SDL_GPU_BUFFERUSAGE_VERTEX, buffer);
}

bool CreateGPUIndexBuffer(SDL_GPUDevice* device, const std::vector<uint16_t>& vertices, SDL_GPUBuffer** buffer, Uint32& indexCount)
{
    return CreateGPUBuffer(device, std::span<const uint16_t>(vertices), SDL_GPU_BUFFERUSAGE_INDEX, buffer, &indexCount);
}

bool CreateDepthTexture(SDL_GPUDevice* device, SDL_GPUTexture** depthTexture, int w, int h)
{
    SDL_GPUTextureCreateInfo createInfo {
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
        .usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
        .width = (Uint32)w,
        .height = (Uint32)h,
        .layer_count_or_depth = 1,
        .num_levels = 1
    };
    *depthTexture = SDL_CreateGPUTexture(device, &createInfo);

    return true;
}

bool CreateGPUSampler(SDL_GPUSampler** sampler, SDL_GPUDevice* device)
{
    SDL_GPUSamplerCreateInfo pointClamp {
        .min_filter = SDL_GPU_FILTER_NEAREST,
        .mag_filter = SDL_GPU_FILTER_NEAREST,
        .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
        .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
        .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
        .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
    };
    // PointClamp
    *sampler = SDL_CreateGPUSampler(device, &pointClamp);
    return true;
}

bool CreatePipeline(SDL_GPUGraphicsPipeline** pipeline, SDL_GPUDevice* device, SDL_Window* window, DL_ShaderInfo& vertexShaderInfo, DL_ShaderInfo& fragmentShaderInfo)
{
    SDL_GPUShader* vertexColourShader {LoadShader(device, vertexShaderInfo)};
    if(vertexColourShader == nullptr)
    {
        SDL_Log("Couldn'tcreate vertex shader\n");
        return false;
    }

    SDL_GPUShader* basicFragShader {LoadShader(device, fragmentShaderInfo)};
    if(basicFragShader == nullptr)
    {
        SDL_Log("Couldn'tcreate fragment shader\n");
        return false;
    }

    std::array colourTargetDescriptions {
        SDL_GPUColorTargetDescription{
            .format = SDL_GetGPUSwapchainTextureFormat(device, window)
        }
    };

    std::array vertexBufferDescriptions {
        SDL_GPUVertexBufferDescription {
            .slot = 0,
            .pitch = sizeof(Vertex),
            .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
            .instance_step_rate = 0
        }
        /*
        SDL_GPUVertexBufferDescription {
            .slot = 1,
            .pitch = sizeof(TexRect),
            .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
            .instance_step_rate = 0
        }
        */
    };

    std::array vertexAttributes {
        SDL_GPUVertexAttribute{
            .location = 0,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
            .offset = 0
        },
        SDL_GPUVertexAttribute{
            .location = 1,
            .buffer_slot = 0,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
            .offset = sizeof(float) * 3
        }
        /*
        SDL_GPUVertexAttribute{
            .location = 2,
            .buffer_slot = 1,
            .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
            .offset = sizeof(float) * 6
        }
            */
    };

    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo {
        .vertex_shader = vertexColourShader,
        .fragment_shader = basicFragShader,
        .vertex_input_state = {
            .vertex_buffer_descriptions = vertexBufferDescriptions.data(),
            .num_vertex_buffers = vertexBufferDescriptions.size(),
            .vertex_attributes = vertexAttributes.data(),
            .num_vertex_attributes = vertexAttributes.size()
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .rasterizer_state = {
            .fill_mode = SDL_GPU_FILLMODE_FILL,/*
            .cull_mode = SDL_GPU_CULLMODE_BACK,
            .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE,*/
        },
        .depth_stencil_state = {
            .compare_op = SDL_GPU_COMPAREOP_LESS,
            .enable_depth_test = true,
            .enable_depth_write = true,
            .enable_stencil_test = false
        },
        .target_info = {
            .color_target_descriptions = colourTargetDescriptions.data(),
            .num_color_targets = colourTargetDescriptions.size(),
            .depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
            .has_depth_stencil_target = true,
        }
    };

    *pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipelineCreateInfo);

    SDL_ReleaseGPUShader(device, vertexColourShader);
    SDL_ReleaseGPUShader(device, basicFragShader);

    return true;
}