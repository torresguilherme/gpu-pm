#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

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
    uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties);
    void build_uniform_buffers();
    void build_descriptor_pool();
    void build_descriptor_set();

    void build_command_buffer();
    void execute_command_buffer(int width, int height);
    void end_command_buffer();

    void cleanup();

    GPUInstance();
    ~GPUInstance();
};

namespace buffers {
    struct MeshData {
        std::vector<glm::vec3> position;
        std::vector<glm::vec3> normal;
        std::vector<glm::vec3> tex_coord;
        std::vector<glm::vec3> tangent;
        std::vector<glm::vec3> bitangent;
    };

    struct Image {
        std::vector<glm::vec4> data;
    };

    struct Specs {
        uint samples_per_pixel;
        uint image_width;
        uint image_height;
    };

    struct Camera {
        glm::mat4 view_matrix;
        float horizontal_fov;
        float aspect;
    };

    struct Lights {
        std::vector<glm::vec3> position;
        std::vector<glm::vec3> color_diffuse;
        std::vector<glm::vec3> color_specular;
        std::vector<glm::vec3> color_ambient;
        std::vector<float> attenuation_constant;
        std::vector<float> attenuation_linear;
        std::vector<float> attenuation_quadratic;
    };
};