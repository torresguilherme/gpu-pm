#include <vulkan/vulkan.hpp>

struct QueueFamilyIndices {
    int graphics_family;
};

typedef struct GPUInstance {
    VkInstance vk_instance;
    VkPhysicalDevice physical_device;
    VkDevice logical_device;

    void create_instance();
    bool check_validation();

    uint32_t rate_device_suitability(VkPhysicalDevice device);
    QueueFamilyIndices find_queue_families(VkPhysicalDevice device);
    void pick_physical_device();
    void create_logical_device();
    void cleanup();

    GPUInstance();
    ~GPUInstance();
} GPUInstance;