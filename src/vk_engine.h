// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include<vector>
#include <vk_types.h>
#include <functional>
#include <deque>
#include <vk_mesh.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
struct Material {
	VkPipeline pipeline;
	VkPipelineLayout pipelineLayout;
};

class Camera
{
	private:
	glm:: vec3 cameraPos;
	glm:: vec3 up;
	glm:: vec3 cameraFront;
	glm::mat4 view;
	float speed;
	float pitch;
	float yaw;
	float lastX;
	float lastY;

	public:

	Camera()
	: cameraPos(glm::vec3(0.0,0.0,3.0)), up(glm::vec3(0.0, 1.0, 0.0)), cameraFront(glm::vec3(0.0,0.0,0.0)), speed(0.05), lastX(850), lastY(450)
	{
		calculateViewMatrix();			
	}

	Camera(glm::vec3& pos, glm::vec3& up, glm::vec3& front, float& speed)
	: cameraPos(pos), up(up), cameraFront(front), speed(speed)
	{
		calculateViewMatrix();
	}

	void calculateViewMatrix()
	{
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, up);
	}

	void incrementPosition(glm::vec3& displacement)
	{
		cameraPos += displacement;
		calculateViewMatrix();
	}

	float getSpeed()
	{
		return speed;
	}

	void setSpeed(float& sp)
	{
		speed = sp;
	}

	void onW()
	{
		cameraPos += speed*cameraFront;
		calculateViewMatrix();
	}

	void onS()
	{
		cameraPos -= speed*cameraFront;
		calculateViewMatrix();	
	}

	void onA()
	{
		
		cameraPos -= glm::normalize(glm::cross(cameraFront, up)) * speed;
		calculateViewMatrix();
	}

	void onD()
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, up)) * speed;
		calculateViewMatrix();
	}

	void onMouse(int xpos, int ypos)
	{
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; 
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw   += xoffset;
		pitch += yoffset;

		if(pitch > 89.0f)
			pitch = 89.0f;
		if(pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(direction);

		calculateViewMatrix();

	}

	glm::mat4& getViewMatrix()
	{
		return view;
	}

};

struct RenderObject {
	Mesh* mesh;

	Material* material;

	glm::mat4 transformMatrix;
};

struct MeshPushConstants {
	glm::vec4 data;
	glm::mat4 render_matrix;
};

struct DeletionQueue
{
	public:
	std::deque<std::function<void()>> deletors;

	void push_function(std::function<void()>&& fun)
	{
		deletors.push_back(fun);
	}

	void flush()
	{
		for (auto it = deletors.begin(); it != deletors.end(); it++)
		{
			(*it)();
		}
		deletors.clear();
	}

};

class VulkanEngine {
public:

	VkInstance _instance;
	int _selectedShader{ 0 };
	VkDebugUtilsMessengerEXT _debug_messenger;
	VkPhysicalDevice _chosenGPU;
	VkDevice _device;
	VkSurfaceKHR _surface;

	bool _isInitialized{ false };
	int _frameNumber {0};

	VkExtent2D _windowExtent{ 1700 , 900 };

	struct SDL_Window* _window{ nullptr };

	//initializes everything in the engine
	void init();

	//shuts down the engine
	void cleanup();

	//draw loop
	void draw();

	//run main loop
	void run();

	bool load_shader_module(const char* filePath, VkShaderModule* outShaderModule);

	VkSwapchainKHR _swapchain; // from other articles

	// image format expected by the windowing system
	VkFormat _swapchainImageFormat;

	//array of images from the swapchain
	std::vector<VkImage> _swapchainImages;

	//array of image-views from the swapchain
	std::vector<VkImageView> _swapchainImageViews;

	VkQueue _graphicsQueue; //queue we will submit to
	uint32_t _graphicsQueueFamily; //family of that queue

	VkCommandPool _commandPool; //the command pool for our commands
	
	VkCommandBuffer _mainCommandBuffer; //the buffer we will record into
	VkRenderPass _renderPass;
	std::vector<VkFramebuffer> _framebuffers;
	VkSemaphore _presentSemaphore, _renderSemaphore;
	VkFence _renderFence;
	VkPipelineLayout _trianglePipelineLayout;
	VkPipeline _trianglePipeline;
	VkPipeline _redTrianglePipeline;
	DeletionQueue _mainDeletionQueue;
	VmaAllocator _allocator;
	VkPipeline _meshPipeline;
	Mesh _triangleMesh;
	VkPipelineLayout _meshPipelineLayout;
	Mesh _monkeyMesh;
	VkImageView _depthImageView;
	AllocatedImage _depthImage;
	VkFormat _depthFormat;

	//default array of renderable objects
	std::vector<RenderObject> _renderables;

	std::unordered_map<std::string,Material> _materials;
	std::unordered_map<std::string,Mesh> _meshes;
	//functions

	//create material and add it to the map
	Material* create_material(VkPipeline pipeline, VkPipelineLayout layout,const std::string& name);

	//returns nullptr if it can't be found
	Material* get_material(const std::string& name);

	//returns nullptr if it can't be found
	Mesh* get_mesh(const std::string& name);

	Camera cam;
	float currFrame;
	float lastFrame;
	
private:
	void init_vulkan();
	void init_swapchain();
	void init_commands();
	void init_default_renderpass();
	void init_framebuffers();
	void init_sync_structures();
	void init_pipelines();
	void load_meshes();
	void upload_mesh(Mesh& mesh);
	void draw_objects(VkCommandBuffer cmd,RenderObject* first, int count);
	void init_scene();
	

};

class PipelineBuilder
{
	public:
	VkPipelineDepthStencilStateCreateInfo _depthStencil;
	std::vector<VkPipelineShaderStageCreateInfo> _shaderStages;
	VkPipelineVertexInputStateCreateInfo _vertexInputInfo;
	VkPipelineInputAssemblyStateCreateInfo _inputAssembly;
	VkViewport _viewport;
	VkRect2D _scissor;
	VkPipelineRasterizationStateCreateInfo _rasterizer;
	VkPipelineColorBlendAttachmentState _colorBlendAttachment;
	VkPipelineMultisampleStateCreateInfo _multisampling;
	VkPipelineLayout _pipelineLayout;
	
	VkPipeline build_pipeline(VkDevice device, VkRenderPass pass);
	


};


