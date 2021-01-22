#include <gpu_instance.hpp>
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <cstring>

const std::vector<const char*> VALIDATION_LAYERS = {
    "VK_LAYER_KHRONOS_validation"
};

GPUInstance::GPUInstance() {
    create_instance();
}

void GPUInstance::create_instance() {
    if (!check_validation()) {
        throw std::runtime_error("Khronos validation layer not supported, aborting.\n");
    }

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

GPUInstance::~GPUInstance() {
    cleanup();
}

void GPUInstance::cleanup() {
    printf("Destroying GPU instance...\n");
    vkDestroyInstance(this->vk_instance, nullptr);
}