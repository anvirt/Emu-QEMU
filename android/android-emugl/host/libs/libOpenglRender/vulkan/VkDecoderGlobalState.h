// Copyright 2018 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include "VulkanHandleMapping.h"
#include "VulkanDispatch.h"

#include <vulkan/vulkan.h>

#include <memory>

#include "cereal/common/goldfish_vk_private_defs.h"
#include "cereal/common/goldfish_vk_transform.h"

namespace goldfish_vk {

// Class for tracking host-side state. Currently we only care about
// tracking VkDeviceMemory to make it easier to pass the right data
// from mapped pointers to the guest, but this could get more stuff
// added to it if for instance, we want to only expose a certain set
// of physical device capabilities, or do snapshots.

// This class may be autogenerated in the future.
// Currently, it works by interfacing with VkDecoder calling on_<apicall>
// functions.
class VkDecoderGlobalState {
public:
    VkDecoderGlobalState();
    ~VkDecoderGlobalState();

    // There should only be one instance of VkDecoderGlobalState
    // per process
    static VkDecoderGlobalState* get();

    // Fast way to get dispatch tables associated with a Vulkan object.
    // VkInstance
    // VkPhysicalDevice
    // VkDevice
    // VkQueue
    // VkCommandBuffer

    VkResult on_vkCreateInstance(
        const VkInstanceCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkInstance* pInstance);

    void on_vkDestroyInstance(
        VkInstance instance,
        const VkAllocationCallbacks* pAllocator);

    VkResult on_vkEnumeratePhysicalDevices(
        VkInstance instance,
        uint32_t* physicalDeviceCount,
        VkPhysicalDevice* physicalDevices);

    // Override features
    void on_vkGetPhysicalDeviceFeatures(
        VkPhysicalDevice physicalDevice,
        VkPhysicalDeviceFeatures* pFeatures);
    void on_vkGetPhysicalDeviceFeatures2(
        VkPhysicalDevice physicalDevice,
        VkPhysicalDeviceFeatures2* pFeatures);
    void on_vkGetPhysicalDeviceFeatures2KHR(
        VkPhysicalDevice physicalDevice,
        VkPhysicalDeviceFeatures2* pFeatures);

    // Override image format properties
    VkResult on_vkGetPhysicalDeviceImageFormatProperties(
            VkPhysicalDevice physicalDevice,
            VkFormat format,
            VkImageType type,
            VkImageTiling tiling,
            VkImageUsageFlags usage,
            VkImageCreateFlags flags,
            VkImageFormatProperties* pImageFormatProperties);
    VkResult on_vkGetPhysicalDeviceImageFormatProperties2(
            VkPhysicalDevice physicalDevice,
            const VkPhysicalDeviceImageFormatInfo2* pImageFormatInfo,
            VkImageFormatProperties2* pImageFormatProperties);
    VkResult on_vkGetPhysicalDeviceImageFormatProperties2KHR(
            VkPhysicalDevice physicalDevice,
            const VkPhysicalDeviceImageFormatInfo2* pImageFormatInfo,
            VkImageFormatProperties2* pImageFormatProperties);

    // Override format features
    void on_vkGetPhysicalDeviceFormatProperties(
            VkPhysicalDevice physicalDevice,
            VkFormat format,
            VkFormatProperties* pFormatProperties);
    void on_vkGetPhysicalDeviceFormatProperties2(
            VkPhysicalDevice physicalDevice,
            VkFormat format,
            VkFormatProperties2* pFormatProperties);
    void on_vkGetPhysicalDeviceFormatProperties2KHR(
            VkPhysicalDevice physicalDevice,
            VkFormat format,
            VkFormatProperties2* pFormatProperties);

    // Override API version
    void on_vkGetPhysicalDeviceProperties(
        VkPhysicalDevice physicalDevice,
        VkPhysicalDeviceProperties* pProperties);
    void on_vkGetPhysicalDeviceProperties2(
        VkPhysicalDevice physicalDevice,
        VkPhysicalDeviceProperties2* pProperties);
    void on_vkGetPhysicalDeviceProperties2KHR(
        VkPhysicalDevice physicalDevice,
        VkPhysicalDeviceProperties2* pProperties);

    // Override memory types advertised from host
    //
    void on_vkGetPhysicalDeviceMemoryProperties(
        VkPhysicalDevice physicalDevice,
        VkPhysicalDeviceMemoryProperties* pMemoryProperties);
    void on_vkGetPhysicalDeviceMemoryProperties2(
        VkPhysicalDevice physicalDevice,
        VkPhysicalDeviceMemoryProperties2* pMemoryProperties);
    void on_vkGetPhysicalDeviceMemoryProperties2KHR(
        VkPhysicalDevice physicalDevice,
        VkPhysicalDeviceMemoryProperties2* pMemoryProperties);

    VkResult on_vkCreateDevice(
        VkPhysicalDevice physicalDevice,
        const VkDeviceCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDevice* pDevice);

    void on_vkGetDeviceQueue(
        VkDevice device,
        uint32_t queueFamilyIndex,
        uint32_t queueIndex,
        VkQueue* pQueue);

    void on_vkDestroyDevice(
        VkDevice device,
        const VkAllocationCallbacks* pAllocator);

    VkResult on_vkCreateBuffer(VkDevice device,
                               const VkBufferCreateInfo* pCreateInfo,
                               const VkAllocationCallbacks* pAllocator,
                               VkBuffer* pBuffer);

    void on_vkDestroyBuffer(VkDevice device,
                            VkBuffer buffer,
                            const VkAllocationCallbacks* pAllocator);

    VkResult on_vkBindBufferMemory(VkDevice device,
                                   VkBuffer buffer,
                                   VkDeviceMemory memory,
                                   VkDeviceSize memoryOffset);
    VkResult on_vkBindBufferMemory2(VkDevice device,
                                    uint32_t bindInfoCount,
                                    const VkBindBufferMemoryInfo* pBindInfos);
    VkResult on_vkBindBufferMemory2KHR(VkDevice device,
                                       uint32_t bindInfoCount,
                                       const VkBindBufferMemoryInfo* pBindInfos);

    VkResult on_vkCreateImage(
        VkDevice device,
        const VkImageCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkImage* pImage);

    void on_vkDestroyImage(
        VkDevice device,
        VkImage image,
        const VkAllocationCallbacks* pAllocator);

    VkResult on_vkCreateImageView(
        VkDevice device,
        const VkImageViewCreateInfo* pCreateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkImageView* pView);

    void on_vkDestroyImageView(VkDevice device,
                               VkImageView imageView,
                               const VkAllocationCallbacks* pAllocator);

    VkResult on_vkCreateSampler(VkDevice device,
                                const VkSamplerCreateInfo* pCreateInfo,
                                const VkAllocationCallbacks* pAllocator,
                                VkSampler* pSampler);

    void on_vkDestroySampler(VkDevice device,
                             VkSampler sampler,
                             const VkAllocationCallbacks* pAllocator);

    void on_vkUpdateDescriptorSets(
            VkDevice device,
            uint32_t descriptorWriteCount,
            const VkWriteDescriptorSet* pDescriptorWrites,
            uint32_t descriptorCopyCount,
            const VkCopyDescriptorSet* pDescriptorCopies);

    void on_vkCmdCopyBufferToImage(
        VkCommandBuffer commandBuffer,
        VkBuffer srcBuffer,
        VkImage dstImage,
        VkImageLayout dstImageLayout,
        uint32_t regionCount,
        const VkBufferImageCopy* pRegions);

    // Do we need to wrap vk(Create|Destroy)Instance to
    // update our maps of VkDevices? Spec suggests no:
    // https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/vkDestroyInstance.html
    // Valid Usage
    // All child objects created using instance
    // must have been destroyed prior to destroying instance
    //
    // This suggests that we should emulate the invalid behavior by
    // not destroying our own VkDevice maps on instance destruction.

    VkResult on_vkAllocateMemory(
        VkDevice device,
        const VkMemoryAllocateInfo* pAllocateInfo,
        const VkAllocationCallbacks* pAllocator,
        VkDeviceMemory* pMemory);

    void on_vkFreeMemory(
        VkDevice device,
        VkDeviceMemory memory,
        const VkAllocationCallbacks* pAllocator);

    VkResult on_vkMapMemory(VkDevice device,
                            VkDeviceMemory memory,
                            VkDeviceSize offset,
                            VkDeviceSize size,
                            VkMemoryMapFlags flags,
                            void** ppData);

    void on_vkUnmapMemory(VkDevice device, VkDeviceMemory memory);

    uint8_t* getMappedHostPointer(VkDeviceMemory memory);
    VkDeviceSize getDeviceMemorySize(VkDeviceMemory memory);
    bool usingDirectMapping() const;

    struct HostFeatureSupport {
        bool supportsVulkan = false;
        bool supportsVulkan1_1 = false;
        bool supportsExternalMemory = false;
        uint32_t apiVersion = 0;
        uint32_t driverVersion = 0;
        uint32_t deviceID = 0;
        uint32_t vendorID = 0;
    };

    HostFeatureSupport getHostFeatureSupport() const;

    // VK_ANDROID_native_buffer
    VkResult on_vkGetSwapchainGrallocUsageANDROID(
        VkDevice device,
        VkFormat format,
        VkImageUsageFlags imageUsage,
        int* grallocUsage);
    VkResult on_vkGetSwapchainGrallocUsage2ANDROID(
        VkDevice device,
        VkFormat format,
        VkImageUsageFlags imageUsage,
        VkSwapchainImageUsageFlagsANDROID swapchainImageUsage,
        uint64_t* grallocConsumerUsage,
        uint64_t* grallocProducerUsage);
    VkResult on_vkAcquireImageANDROID(
        VkDevice device,
        VkImage image,
        int nativeFenceFd,
        VkSemaphore semaphore,
        VkFence fence);
    VkResult on_vkQueueSignalReleaseImageANDROID(
        VkQueue queue,
        uint32_t waitSemaphoreCount,
        const VkSemaphore* pWaitSemaphores,
        VkImage image,
        int* pNativeFenceFd);

    // VK_GOOGLE_address_space
    VkResult on_vkMapMemoryIntoAddressSpaceGOOGLE(
       VkDevice device,
       VkDeviceMemory memory,
       uint64_t* pAddress);

    // VK_GOOGLE_color_buffer
    VkResult on_vkRegisterImageColorBufferGOOGLE(
       VkDevice device, VkImage image, uint32_t colorBuffer);
    VkResult on_vkRegisterBufferColorBufferGOOGLE(
       VkDevice device, VkBuffer buffer, uint32_t colorBuffer);

    VkResult on_vkAllocateCommandBuffers(
            VkDevice device,
            const VkCommandBufferAllocateInfo* pAllocateInfo,
            VkCommandBuffer* pCommandBuffers);

    void on_vkCmdExecuteCommands(VkCommandBuffer commandBuffer,
                                 uint32_t commandBufferCount,
                                 const VkCommandBuffer* pCommandBuffers);

    VkResult on_vkQueueSubmit(VkQueue queue,
                          uint32_t submitCount,
                          const VkSubmitInfo* pSubmits,
                          VkFence fence);

    VkResult on_vkResetCommandBuffer(VkCommandBuffer commandBuffer,
                                     VkCommandBufferResetFlags flags);

    void on_vkFreeCommandBuffers(VkDevice device,
                                 VkCommandPool commandPool,
                                 uint32_t commandBufferCount,
                                 const VkCommandBuffer* pCommandBuffers);

    VkResult on_vkCreateCommandPool(VkDevice device,
                                    const VkCommandPoolCreateInfo* pCreateInfo,
                                    const VkAllocationCallbacks* pAllocator,
                                    VkCommandPool* pCommandPool);

    void on_vkDestroyCommandPool(VkDevice device,
                                 VkCommandPool commandPool,
                                 const VkAllocationCallbacks* pAllocator);

    VkResult on_vkResetCommandPool(VkDevice device,
                                   VkCommandPool commandPool,
                                   VkCommandPoolResetFlags flags);

    // Transformations

    void deviceMemoryTransform_tohost(
        VkDeviceMemory* memory, uint32_t memoryCount,
        VkDeviceSize* offset, uint32_t offsetCount,
        VkDeviceSize* size, uint32_t sizeCount,
        uint32_t* typeIndex, uint32_t typeIndexCount,
        uint32_t* typeBits, uint32_t typeBitsCount);
    void deviceMemoryTransform_fromhost(
        VkDeviceMemory* memory, uint32_t memoryCount,
        VkDeviceSize* offset, uint32_t offsetCount,
        VkDeviceSize* size, uint32_t sizeCount,
        uint32_t* typeIndex, uint32_t typeIndexCount,
        uint32_t* typeBits, uint32_t typeBitsCount);

#define DEFINE_TRANSFORMED_TYPE_PROTOTYPE(type) \
    void transformImpl_##type##_tohost(const type*, uint32_t); \
    void transformImpl_##type##_fromhost(const type*, uint32_t); \
    
LIST_TRANSFORMED_TYPES(DEFINE_TRANSFORMED_TYPE_PROTOTYPE)

private:
    class Impl;
    std::unique_ptr<Impl> mImpl;
};

#define DEFINE_BOXED_DISPATCHABLE_HANDLE_TYPE(type) \
struct BoxedDispatchable_##type { \
    type underlying; \
    VulkanDispatch* dispatch; \
}; \
static inline BoxedDispatchable_##type* new_boxed_##type( \
    type underlying, VulkanDispatch* otherDispatch = nullptr) { \
    BoxedDispatchable_##type* res = new BoxedDispatchable_##type; \
    res->underlying = underlying; \
    res->dispatch = otherDispatch ? otherDispatch : new VulkanDispatch; \
    return res; \
} \
static inline void delete_boxed_##type( \
    BoxedDispatchable_##type* boxed, bool ownDispatch = true) { \
    if (!boxed) return; \
    if (ownDispatch) delete boxed->dispatch; boxed->dispatch = nullptr; \
    delete boxed; \
} \
static inline void delete_boxed_##type(type boxed) { \
    if (!boxed) return; \
    delete_boxed_##type((BoxedDispatchable_##type*)boxed); \
} \
static inline type unbox_##type(type x) { \
    if (!x) return (type)0; \
    return ((BoxedDispatchable_##type*)(x))->underlying; \
} \
static inline VulkanDispatch* dispatch_##type(type x) { \
    if (!x) return nullptr; \
    return ((BoxedDispatchable_##type*)(x))->dispatch; \
} \

GOLDFISH_VK_LIST_DISPATCHABLE_HANDLE_TYPES(DEFINE_BOXED_DISPATCHABLE_HANDLE_TYPE)

#define MAKE_HANDLE_MAPPING_FOREACH(type_name, map_impl, map_to_u64_impl, map_from_u64_impl) \
    void mapHandles_##type_name(type_name* handles, size_t count) override { \
        for (size_t i = 0; i < count; ++i) { \
            map_impl; \
        } \
    } \
    void mapHandles_##type_name##_u64(const type_name* handles, uint64_t* handle_u64s, size_t count) override { \
        for (size_t i = 0; i < count; ++i) { \
            map_to_u64_impl; \
        } \
    } \
    void mapHandles_u64_##type_name(const uint64_t* handle_u64s, type_name* handles, size_t count) override { \
        for (size_t i = 0; i < count; ++i) { \
            map_from_u64_impl; \
        } \
    } \

#define BOXED_DISPATCHABLE_UNWRAP_IMPL(type_name) \
    MAKE_HANDLE_MAPPING_FOREACH(type_name, \
        if (handles[i]) { handles[i] = ((BoxedDispatchable_##type_name*)(handles[i]))->underlying; } else { handles[i] = nullptr; } ;, \
        if (handles[i]) { handle_u64s[i] = (uint64_t)((BoxedDispatchable_##type_name*)(handles[i]))->underlying; } else { handle_u64s[i] = 0; }, \
        if (handle_u64s[i]) { handles[i] = (type_name)((BoxedDispatchable_##type_name*)(handle_u64s[i]))->underlying; } else { handles[i] = nullptr; })

#define BOXED_NON_DISPATCHABLE_UNWRAP_IMPL(type_name) \
    MAKE_HANDLE_MAPPING_FOREACH(type_name, \
        (void)handles[i], \
        handle_u64s[i] = (uint64_t)(uintptr_t)handles[i], \
        handles[i] = (type_name)(uintptr_t)handle_u64s[i])

class BoxedHandleUnwrapMapping : public VulkanHandleMapping {
public:
    virtual ~BoxedHandleUnwrapMapping() { }
    GOLDFISH_VK_LIST_DISPATCHABLE_HANDLE_TYPES(BOXED_DISPATCHABLE_UNWRAP_IMPL)
    GOLDFISH_VK_LIST_NON_DISPATCHABLE_HANDLE_TYPES(BOXED_NON_DISPATCHABLE_UNWRAP_IMPL)
};

// We currently don't need a BoxedHandleWrapMapping because there are, now, no
// APIs where dispatchable handle types are created and not touched by us: all are wrapped:
// vkCreateInstance
// vkEnumeratePhysicalDevices
// vkCreateDevice
// vkGetDeviceQueue
// vkAllocateCommandBuffers

} // namespace goldfish_vk
