#include <vk_engine.h>
#include<iostream>
int main(int argc, char* argv[])
{
	VulkanEngine engine;

	engine.init();	
	
	engine.run();	

	engine.cleanup();
	std::cout<<"16";
	// dummy comment



	return 0;
}
