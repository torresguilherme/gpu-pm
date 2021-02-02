#include <vulkan/vulkan.hpp>

struct QueueFamilyIndices {
    int graphics_family;
};

struct GPUInstance {
    VkInstance vk_instance;
    VkPhysicalDevice physical_device;
    VkDevice logical_device;
    VkShaderModule compute_module;
    VkDescriptorSetLayout descriptor_set_layout;
    VkPipelineLayout layout;
    VkPipeline pipeline;
    VkCommandBuffer command_buffer;
    VkCommandPool command_pool;
    VkDescriptorPool descriptor_pool;
    VkDescriptorSet descriptor_set;
    std::vector<VkBuffer> buffers;
    std::vector<VkDeviceMemory> device_memory;

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
    void build_command_pool();
    void build_descriptor_pool();
    void build_descriptor_set();
    void build_uniform_buffers();

    void build_command_buffer();
    void execute_command_buffer(int width, int height);
    void end_command_buffer();

    void cleanup();

    GPUInstance();
    ~GPUInstance();
};