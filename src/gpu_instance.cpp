#include <gpu_instance.hpp>
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <cstring>
#include <fstream>

const uint BATCH = 64;
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
    create_info.enabledLayerCount = 0;

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

void GPUInstance::create_pipeline_stages() {
    VkPipelineShaderStageCreateInfo stage_create_info {};
    stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stage_create_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    stage_create_info.module = this->compute_module;
    stage_create_info.pName = "main";

    std::vector<VkDescriptorSetLayoutBinding> bindings(2);
    bindings[0].binding = 0;
    bindings[0].descriptorCount = 1;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    bindings[0].pImmutableSamplers = nullptr;

    bindings[1].binding = 1;
    bindings[1].descriptorCount = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    bindings[1].pImmutableSamplers = nullptr;

    bindings[2].binding = 2;
    bindings[2].descriptorCount = 1;
    bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    bindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    bindings[2].pImmutableSamplers = nullptr;

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

GPUInstance::~GPUInstance() {
    cleanup();
}

void GPUInstance::cleanup() {
    printf("Destroying GPU instance...\n");
    vkDestroyDescriptorSetLayout(this->logical_device, this->descriptor_set_layout, nullptr);
    vkDestroyPipelineLayout(this->logical_device, this->layout, nullptr);
    vkDestroyShaderModule(this->logical_device, this->compute_module, nullptr);
    vkDestroyPipeline(this->logical_device, this->pipeline, nullptr);
    vkDestroyDevice(this->logical_device, nullptr);
    vkDestroyInstance(this->vk_instance, nullptr);
}