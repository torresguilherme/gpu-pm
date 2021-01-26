#include <vulkan/vulkan.hpp>

struct QueueFamilyIndices {
    int graphics_family;
};

typedef struct GPUInstance {
    VkInstance vk_instance;
    VkPhysicalDevice physical_device;
    VkDevice logical_device;
    VkShaderModule compute_module;
    VkDescriptorSetLayout descriptor_set_layout;
    VkPipelineLayout layout;
    VkPipeline pipeline;
    VkQueue queue;
    VkCommandBuffer command_buffer;
    VkCommandPool command_pool;
    VkFence fence;

    void create_instance();
    bool check_validation();

    uint32_t rate_device_suitability(VkPhysicalDevice device);
    QueueFamilyIndices find_queue_families(VkPhysicalDevice device);
    void pick_physical_device();
    void create_logical_device();

    VkShaderModule create_shader_module(const std::vector<char>& code);
    void create_ubo_binding(std::vector<VkDescriptorSetLayoutBinding>& bindings, uint index);
    void create_pipeline_stages();
    void create_pipeline();
    
    void shader_pass();

    void cleanup();

    GPUInstance();
    ~GPUInstance();
} GPUInstance;