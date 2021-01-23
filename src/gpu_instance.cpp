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
    pick_physical_device();
    create_logical_device();
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
}

void GPUInstance::create_compute_pipeline() {
    
}

GPUInstance::~GPUInstance() {
    cleanup();
}

void GPUInstance::cleanup() {
    printf("Destroying GPU instance...\n");
    vkDestroyDevice(this->logical_device, nullptr);
    vkDestroyInstance(this->vk_instance, nullptr);
}