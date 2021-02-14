#include <scene.hpp>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

struct QueueFamilyIndices {
    int graphics_family;
};

namespace uniform_buffers {
    struct MeshData {
        const glm::vec3* position;
        const glm::vec3* normal;
        const glm::vec2* tex_coord;
        const glm::vec3* tangent;
        const glm::vec3* bitangent;
        const uint* indices;
        uint num_vertices;
        uint num_indices;
        glm::mat4 global_transform;
    };

    struct Image {
        glm::vec4* data;
    };

    struct Specs {
        uint samples_per_pixel;
        uint image_width;
        uint image_height;
        uint num_meshes;
        uint num_lights;
    };

    struct Camera {
        glm::mat4 view_matrix;
        float horizontal_fov;
        float aspect;
    };
};

struct GPUInstance {
    // vulkan objects
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
    std::vector<VkDescriptorSet> descriptor_sets;
    std::vector<VkBuffer> buffers;
    std::vector<VkDeviceMemory> device_memory;

    // buffers
    std::vector<uniform_buffers::MeshData> meshes_data;
    uniform_buffers::Image image;
    uniform_buffers::Specs specs;
    uniform_buffers::Camera camera;

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
    uint get_buffer_size(uint index);
    void build_descriptor_set();

    void allocate_uniform_data(const Scene& scene, uint width, uint height, uint samples_per_pixel);
    void send_uniform_data_struct(uint index, int struct_size, int buffer_length, void* data);
    void send_uniform_data();
    void build_command_buffer();
    void execute_command_buffer(int width, int height);
    void end_command_buffer();

    void destroy_image_data();
    void cleanup();

    GPUInstance();
    ~GPUInstance();
};