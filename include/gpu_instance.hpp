#include <vulkan/vulkan.hpp>

typedef struct GPUInstance {
    VkInstance vk_instance;
    VkDevice device;

    void create_instance();
    bool check_validation();
    void cleanup();

    GPUInstance();
    ~GPUInstance();
} GPUInstance;