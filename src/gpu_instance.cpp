#include <gpu_instance.hpp>
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <cstring>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>

const uint BATCH = 32;
const uint UBO_COUNT = 5;
const uint IMAGE_BINDING = 3;
const std::vector<const char*> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};

static std::vector<char> read_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file!\n");
    }
    size_t file_size = file.tellg();
    std::vector<char> contents(file_size);
    file.seekg(0);
    file.read(contents.data(), file_size);
    file.close();
    return contents;
}

GPUInstance::GPUInstance() {
    create_instance();
    pick_physical_device();
    create_logical_device();
    create_pipeline();
    build_command_pool();
}

void GPUInstance::create_instance() {
    VkApplicationInfo app_info {};
    app_info.apiVersion = VK_API_VERSION_1_1;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pApplicationName = "GPU Photon Mapper";
    app_info.pEngineName = "No Engine";
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

    VkInstanceCreateInfo create_info {};
    create_info.pApplicationInfo = &app_info;
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
#ifndef NDEBUG
    if (check_validation()) {
        create_info.enabledLayerCount = 1;
        create_info.ppEnabledLayerNames = VALIDATION_LAYERS.data();
    }
    else {
        throw std::runtime_error("Validation layers were requested but they're not available, aborting!\n");
    }
#else
    create_info.enabledLayerCount = 0;
    create_info.ppEnabledLayerNames = nullptr;
#endif

    auto result = vkCreateInstance(&create_info, nullptr, &this->vk_instance);
    if(result != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance!\n");
    }
    else {
        printf("Vulkan instance created with success!\n");
    }
}

bool GPUInstance::check_validation() {
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    std::vector<VkLayerProperties> available_layers(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

    for(const char* layer_name : VALIDATION_LAYERS) {
        bool layer_found = false;
        for(const auto& layer_properties : available_layers) {
            if (strcmp(layer_name, layer_properties.layerName)) {
                layer_found = true;
                break;
            }
        }

        if (!layer_found) {
            return false;
        }
    }

    return true;
}

uint32_t GPUInstance::rate_device_suitability(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);
    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(device, &features);

    if (find_queue_families(device).graphics_family == -1) {
        return 0;
    }

    uint32_t score = 0;
    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }
    score += properties.limits.maxImageDimension2D;
    
    return score;
}

QueueFamilyIndices GPUInstance::find_queue_families(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    indices.graphics_family = -1;
    uint32_t queue_family_count;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
    std::vector<VkQueueFamilyProperties> properties(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, properties.data());
    int i = 0;
    for (const auto& queue_family : properties) {
        if (queue_family.queueFlags & VK_QUEUE_COMPUTE_BIT) {
            indices.graphics_family = i;
        }

        i++;
    }

    return indices;
}

void GPUInstance::pick_physical_device() {
    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(this->vk_instance, &device_count, nullptr);
    if(device_count == 0) {
        throw std::runtime_error("Failed to find devices that support Vulkan!\n");
    }
    std::vector<VkPhysicalDevice> physical_devices(device_count);
    vkEnumeratePhysicalDevices(this->vk_instance, &device_count, physical_devices.data());

    uint32_t device_max_score = 0;
    for (const auto& device: physical_devices) {
        if(rate_device_suitability(device) > device_max_score) {
            device_max_score = rate_device_suitability(device);
            physical_device = device;
        }
    }

    if (physical_device == VK_NULL_HANDLE) {
        throw std::runtime_error("Could not find a device suitable for this application!\n");
    }

    this->physical_device = physical_device;
}

void GPUInstance::create_logical_device() {
    QueueFamilyIndices indices = find_queue_families(this->physical_device);
    VkDeviceQueueCreateInfo queue_create_info {};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = indices.graphics_family;
    queue_create_info.queueCount = 1;
    float queue_priority = 1.0;
    queue_create_info.pQueuePriorities = &queue_priority;

    VkPhysicalDeviceFeatures device_features {};

    VkDeviceCreateInfo device_create_info {};
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.pQueueCreateInfos = &queue_create_info;
    device_create_info.queueCreateInfoCount = 1;
    device_create_info.pEnabledFeatures = &device_features;
    device_create_info.enabledExtensionCount = 0;
    
    if (vkCreateDevice(physical_device, &device_create_info, nullptr, &this->logical_device) != VK_SUCCESS) {
        throw std::runtime_error("Failed at creating logical device!\n");
    }

    vkGetDeviceQueue(this->logical_device, indices.graphics_family, 0, &this->queue);

    printf("Device created with success!\n");
}

VkShaderModule GPUInstance::create_shader_module(const std::vector<char>& code) {
    VkShaderModuleCreateInfo create_info {};
    create_info.codeSize = code.size();
    create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;

    VkShaderModule module;
    if (vkCreateShaderModule(this->logical_device, &create_info, nullptr, &module) != VK_SUCCESS) {
        throw std::runtime_error("Cannot create shader module!\n");
    }

    printf("Created a shader module successfully!\n");
    return module;
}

void GPUInstance::create_ubo_binding(std::vector<VkDescriptorSetLayoutBinding>& bindings, uint index) {
    bindings[index].binding = index;
    bindings[index].descriptorCount = 1;
    if (index >= IMAGE_BINDING) {
        bindings[index].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    }
    else {
        bindings[index].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }
    bindings[index].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    bindings[index].pImmutableSamplers = nullptr;
}

void GPUInstance::create_pipeline_stages() {
    VkPipelineShaderStageCreateInfo stage_create_info {};
    stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage_create_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    stage_create_info.module = this->compute_module;
    stage_create_info.pName = "main";

    std::vector<VkDescriptorSetLayoutBinding> bindings(UBO_COUNT);
    for (uint i = 0; i < UBO_COUNT; i++) {
        create_ubo_binding(bindings, i);
    }

    VkDescriptorSetLayoutCreateInfo set_layout_create_info {};
    set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    set_layout_create_info.bindingCount = bindings.size();
    set_layout_create_info.pBindings = bindings.data();
    if (vkCreateDescriptorSetLayout(this->logical_device, &set_layout_create_info, nullptr, &this->descriptor_set_layout) != VK_SUCCESS) {
        throw std::runtime_error("Could not create pipeline set layouts!\n");
    }

    VkPipelineLayoutCreateInfo layout_create_info {};
    layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_create_info.setLayoutCount = 1;
    layout_create_info.pSetLayouts = &this->descriptor_set_layout;
    if (vkCreatePipelineLayout(this->logical_device, &layout_create_info, nullptr, &this->layout) != VK_SUCCESS) {
        throw std::runtime_error("Could not create pipeline layout, aborting!\n");
    }

    VkComputePipelineCreateInfo pipeline_create_info {};
    pipeline_create_info.stage = stage_create_info;
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipeline_create_info.layout = this->layout;
    if (vkCreateComputePipelines(this->logical_device,
    VK_NULL_HANDLE,
    1,
    &pipeline_create_info,
    nullptr, 
    &this->pipeline) != VK_SUCCESS) {
        throw std::runtime_error("Could not create compute pipeline, aborting!\n");
    }
}

void GPUInstance::create_pipeline() {
    auto main_compute_code = read_file("main.spv");
    this->compute_module = create_shader_module(main_compute_code);
    create_pipeline_stages();
    printf("Compute pipeline successfully created!\n");
}

void GPUInstance::build_command_pool() {
    QueueFamilyIndices family_indices = find_queue_families(this->physical_device);
    VkCommandPoolCreateInfo pool_info {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = family_indices.graphics_family;
    pool_info.flags = 0;

    if (vkCreateCommandPool(this->logical_device, &pool_info, nullptr, &this->command_pool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!\n");
    }
}

uint32_t GPUInstance::find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties mem_properties;
    vkGetPhysicalDeviceMemoryProperties(this->physical_device, &mem_properties);

    for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
        if (type_filter & (1 << i) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

void GPUInstance::build_uniform_buffers(int width, int height) {
    this->buffers.resize(UBO_COUNT);
    this->device_memory.resize(UBO_COUNT);

    for (uint i = 0; i < UBO_COUNT; i++) {
        VkBufferCreateInfo buffer_info {};
        buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_info.size = get_buffer_size(i);
        buffer_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(this->logical_device, &buffer_info, nullptr, &this->buffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("Couldn't create a uniform buffer!\n");
        }

        VkMemoryRequirements memory_requirements;
        vkGetBufferMemoryRequirements(this->logical_device, this->buffers[i], &memory_requirements);

        VkMemoryAllocateInfo alloc_info {};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = memory_requirements.size;
        alloc_info.memoryTypeIndex = find_memory_type(memory_requirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        if (vkAllocateMemory(this->logical_device, &alloc_info, nullptr, &this->device_memory[i]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate vertex buffer memory!\n");
        }

        if (vkBindBufferMemory(this->logical_device, this->buffers[i], this->device_memory[i], 0) != VK_SUCCESS) {
            throw std::runtime_error("Failed to bind contiguous buffer memory!\n");
        }
    }
}

void GPUInstance::build_descriptor_pool() {
    VkDescriptorPoolSize pool_size {};
    pool_size.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    pool_size.descriptorCount = UBO_COUNT;

    VkDescriptorPoolCreateInfo pool_info {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = 1;
    pool_info.pPoolSizes = &pool_size;
    pool_info.maxSets = 1;

    if (vkCreateDescriptorPool(this->logical_device, &pool_info, nullptr, &this->descriptor_pool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor pool!\n");
    }
}

uint GPUInstance::get_aligned_buffer_size(uint index) {
    return (get_buffer_size(index) / 0x10 + 1) * 0x10;
}

uint GPUInstance::get_buffer_size(uint index) {
    if (index == 0) return sizeof(uniform_buffers::Specs);
    if (index == 1) return sizeof(uniform_buffers::Camera);
    if (index == 2) return sizeof(uniform_buffers::MaterialData) * this->material_data.size();
    if (index == IMAGE_BINDING) return this->image_size;
    else return sizeof(uniform_buffers::Vertex) * this->mesh_data.num_vertices + 2 * sizeof(uint);
}

void GPUInstance::build_descriptor_set() {
    VkDescriptorSetAllocateInfo alloc_info {};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = this->descriptor_pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &this->descriptor_set_layout;

    this->descriptor_sets.resize(1);
    if (vkAllocateDescriptorSets(this->logical_device, &alloc_info, this->descriptor_sets.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate descriptor sets!\n");
    }
}

void GPUInstance::allocate_uniform_data(const Scene& scene, uint width, uint height, uint samples_per_pixel) {
    this->mesh_data.vertices = (uniform_buffers::Vertex*) malloc(sizeof(uniform_buffers::Vertex) * scene.total_scene_vertices);
    this->mesh_data.num_vertices = scene.total_scene_vertices;
    uint global_index = 0;
    for (uint i = 0; i < scene.meshes.size(); i++) {
        for (uint j = 0; j < scene.meshes[i].vertices.size(); j++) {
            this->mesh_data.vertices[global_index].position = scene.meshes[i].vertices[j];
            this->mesh_data.vertices[global_index].normal = scene.meshes[i].normals[j];
            this->mesh_data.vertices[global_index].tangent = scene.meshes[i].tangents[j];
            this->mesh_data.vertices[global_index].bitangent = scene.meshes[i].bitangents[j];
            this->mesh_data.vertices[global_index].tex_coord = scene.meshes[i].tex_coords[j];
            this->mesh_data.vertices[global_index].indices = glm::ivec4(
                i,
                scene.meshes[i].material,
                0, 0
            );
        }
    }

    this->material_data.resize(scene.materials.size());
    for(uint i = 0; i < scene.materials.size(); i++) {
        this->material_data[i].albedo = scene.materials[i].albedo;
        this->material_data[i].emissive = scene.materials[i].emissive;
        this->material_data[i].metallic_roughness = glm::vec4(
            scene.materials[i].metallic,
            scene.materials[i].roughness,
            0.0, 0.0
        );
    }

    this->specs.num_materials = scene.materials.size();
    this->specs.num_meshes = scene.meshes.size();
    this->specs.image_width = width;
    this->specs.image_height = height;
    this->specs.samples_per_pixel = samples_per_pixel;

    this->camera.aspect = scene.cameras[scene.current_camera].aspect;
    this->camera.horizontal_fov = scene.cameras[scene.current_camera].horizontal_fov;
    glm::vec3 up = scene.cameras[scene.current_camera].up;
    glm::vec3 eye = scene.cameras[scene.current_camera].eye;
    glm::vec3 target = scene.cameras[scene.current_camera].target;
    this->camera.view_matrix = glm::lookAt(eye, target, up);

    // this->image.data = (glm::vec4*)calloc(width * height, sizeof(glm::vec4));
    this->image_size = width * height * sizeof(float) * 4;
}

void GPUInstance::send_uniform_data_struct(uint index, void* data) {
    void* data_pointer;
    vkMapMemory(this->logical_device, this->device_memory[index], 0, get_buffer_size(index), 0, &data_pointer);
    memcpy(data_pointer, data, get_buffer_size(index));
    vkUnmapMemory(this->logical_device, this->device_memory[index]);
}


void* GPUInstance::get_uniform_data_struct(uint index) {
    void* data_pointer;
    vkMapMemory(this->logical_device, this->device_memory[index], 0, get_buffer_size(index), 0, &data_pointer);
    return data_pointer;
}

void GPUInstance::send_uniform_data() {
    send_uniform_data_struct(0, &specs);
    send_uniform_data_struct(1, &camera);
    send_uniform_data_struct(2, material_data.data());
    send_uniform_data_struct(4, &mesh_data);

    VkDescriptorBufferInfo buffer_info;
    VkWriteDescriptorSet descriptor_write;
    buffer_info.buffer = this->buffers[IMAGE_BINDING];
    buffer_info.offset = 0;
    buffer_info.range = get_buffer_size(IMAGE_BINDING);

    descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptor_write.dstSet = this->descriptor_sets[0];
    descriptor_write.dstBinding = IMAGE_BINDING;
    descriptor_write.dstArrayElement = 0;
    descriptor_write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptor_write.descriptorCount = 1;
    descriptor_write.pBufferInfo = &buffer_info;
    descriptor_write.pNext = nullptr;
    descriptor_write.pImageInfo = nullptr;
    descriptor_write.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(this->logical_device, 1, &descriptor_write, 0, nullptr);
}

void GPUInstance::build_command_buffer() {
    VkCommandBufferAllocateInfo command_buffer_info {};
    command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_info.commandPool = this->command_pool;
    command_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_info.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(this->logical_device, &command_buffer_info, &this->command_buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!\n");
    }

    VkCommandBufferBeginInfo begin_info {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0;
    begin_info.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(this->command_buffer, &begin_info) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!\n");
    }

    vkCmdBindPipeline(this->command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, this->pipeline);
    vkCmdBindDescriptorSets(this->command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, this->layout, 0, 1,
        this->descriptor_sets.data(), 0, nullptr);
}

void GPUInstance::execute_command_buffer(int width, int height) {
    vkCmdDispatch(this->command_buffer, width / BATCH, height / BATCH, 1);
}

void GPUInstance::end_command_buffer() {
    vkEndCommandBuffer(this->command_buffer);
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1; // submit a single command buffer
    submitInfo.pCommandBuffers = &this->command_buffer; // the command buffer to submit.

    VkFence fence;
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;
    if (vkCreateFence(this->logical_device, &fenceCreateInfo, NULL, &fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create fence!\n");
    }
    vkQueueSubmit(this->queue, 1, &submitInfo, fence);
    vkWaitForFences(this->logical_device, 1, &fence, VK_TRUE, 100000000000);
    vkDestroyFence(this->logical_device, fence, nullptr);

    this->image.data = (float*) get_uniform_data_struct(IMAGE_BINDING);
}

GPUInstance::~GPUInstance() {
    cleanup();
}

void GPUInstance::destroy_image_data() {
    vkUnmapMemory(this->logical_device, this->device_memory[IMAGE_BINDING]);
    // free(this->image.data);
}

void GPUInstance::cleanup() {
    printf("Destroying GPU instance...\n");
    destroy_image_data();
    vkDestroyDescriptorPool(this->logical_device, this->descriptor_pool, nullptr);
    vkDestroyCommandPool(this->logical_device, this->command_pool, nullptr);
    vkDestroyDescriptorSetLayout(this->logical_device, this->descriptor_set_layout, nullptr);
    vkDestroyPipelineLayout(this->logical_device, this->layout, nullptr);
    vkDestroyShaderModule(this->logical_device, this->compute_module, nullptr);
    vkDestroyPipeline(this->logical_device, this->pipeline, nullptr);
    for (uint i = 0; i < this->device_memory.size(); i++) {
        vkFreeMemory(this->logical_device, this->device_memory[i], nullptr);
    }
    for (uint i = 0; i < this->buffers.size(); i++) {
        vkDestroyBuffer(this->logical_device, this->buffers[i], nullptr);
    }
    vkDestroyDevice(this->logical_device, nullptr);
    vkDestroyInstance(this->vk_instance, nullptr);
}