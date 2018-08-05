#include <vsg/vk/PhysicalDevice.h>

#include <iostream>

namespace vsg
{

PhysicalDevice::PhysicalDevice(Instance* instance, VkPhysicalDevice device, int graphicsFamily, int presentFamily, int computeFamily, Surface* surface) :
    _instance(instance),
    _device(device),
    _graphicsFamily(graphicsFamily),
    _presentFamily(presentFamily),
    _computeFamily(computeFamily),
    _surface(surface)
{
    vkGetPhysicalDeviceProperties(_device, &_properties);
}

PhysicalDevice::Result PhysicalDevice::create(Instance* instance, VkQueueFlags queueFlags, Surface* surface)
{
    if (!instance)
    {
        return PhysicalDevice::Result("Error: vsg::PhysicalDevice::create(...) failed to create physical device, undefined Instance.", VK_ERROR_INVALID_EXTERNAL_HANDLE);
    }

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(*instance, &deviceCount, nullptr);

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(*instance, &deviceCount, devices.data());

    std::cout<<"PhysicalDevice::create(...) deviceCount="<<deviceCount<<std::endl;

    for (const auto& device : devices)
    {
        VkPhysicalDeviceFeatures supportedFeatures;
        vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

        std::cout<<"supportedFeatures.samplerAnisotropy = "<<supportedFeatures.samplerAnisotropy<<std::endl;


        // Checked the DeviceQueueFamilyProperties for support for graphics
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamiles(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamiles.data());

        int graphicsFamily = -1;
        int presentFamily = -1;
        int computeFamily = -1;

        VkQueueFlags matchedQueues = 0;

        for (int i=0; i<queueFamilyCount; ++i)
        {
            const auto& queueFamily = queueFamiles[i];
            if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)!=0)
            {
                graphicsFamily = i;
                matchedQueues |= VK_QUEUE_GRAPHICS_BIT;
            }

            if ((queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)!=0)
            {
                computeFamily = i;
                matchedQueues |= VK_QUEUE_COMPUTE_BIT;
            }

            if (surface)
            {
                VkBool32 presentSupported = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, *surface, &presentSupported);
                if (queueFamily.queueCount>0 && presentSupported)
                {
                    presentFamily = i;
                }
            }
        }

        if (((matchedQueues & queueFlags)==queueFlags) && (surface==nullptr || presentFamily>=0))
        {
            std::cout<<"   created PhysicalDevice, matchedQueues="<<matchedQueues<<std::endl;
            return new PhysicalDevice(instance, device, graphicsFamily, presentFamily, computeFamily, surface);
        }
        else
        {
            std::cout<<"   PhysicalDevice doesn't have required queue supoort matchedQueues="<<matchedQueues<<std::endl;
        }
    }

    return PhysicalDevice::Result("Error: vsg::Device::create(...) failed to create physical device.", VK_INCOMPLETE);
}

PhysicalDevice::~PhysicalDevice()
{
    std::cout<<"PhysicalDevice()::~PhysicalDevice()"<<std::endl;
}

}
