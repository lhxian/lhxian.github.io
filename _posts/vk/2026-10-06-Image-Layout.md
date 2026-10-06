# Vulkan中的Image Layout(Vulkan学习笔记)
## Image索引方式
首先知道的是在Vulkan中，在GPU上Image的保存方式对程序员和Vulkan都是透明的，所以使用**VK_IMAGE_LAYOUT_XXX**来表示Image的状态。
在以下几种场景可能会使用到Image Layout
- 创建Mipmap,需要按顺序拷贝相邻两级的Image数据。
- 图像从Buffer上传到Image之后，将Layout转换为**VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL**，GPU可以对Image的储存做优化，例如使用tiling格式来保存，优化访问，这些格式对程序员是透明的。

有两个常见的结构体，**VkImageSubresource**和**VkImageSubresourceRange**。
定义如下：
```cpp
// Provided by VK_VERSION_1_0
typedef struct VkImageSubresource {
    VkImageAspectFlags    aspectMask;
    uint32_t              mipLevel;
    uint32_t              arrayLayer;
} VkImageSubresource;

// Provided by VK_VERSION_1_0
typedef struct VkImageSubresourceRange {
    VkImageAspectFlags    aspectMask;
    uint32_t              baseMipLevel;
    uint32_t              levelCount;
    uint32_t              baseArrayLayer;
    uint32_t              layerCount;
} VkImageSubresourceRange;

```

在Vulkan中，一个**VkImage**有两个信息，**mipLevels,layerCount**，表示image的级数和层数，所以一个Image对象同时包含$mipLevels \times layerCount$个小Image,在pipeline中，**mipLevels**通常是硬件自动选择，程序员不用控制，而layer可以在shader中进行采样，例如一个立方体贴图，层数为$6$，对应六个面，在采样的时候，可以通过`texture`函数来指定对应的层数。
**VkImageSubresource**可以当作一个三维索引，表示了一个Image三个不同方面的信息，**mipLevel/layer**选择对应的级数和层数，而**aspectMask**针对的数据平面的内容，例如，可以设置aspectMask为**VK_IMAGE_ASPECT_COLOR_BIT,VK_IMAGE_ASPECT_DEPTH_BIT,VK_IMAGE_ASPECT_STENCIL_BIT**，对于作为color attachment的Image,COLOR_BIT经常使用，而对于作为深度测试和模板测试的，图像的格式一个像素可能有两个数据分量，例如24位深度和8位深度的图像格式(VK_FORMAT_D24_UNORM_S8_UINT)，所以对于深度和模板都存在的Image格式，可以使用aspectMask来操作单一或多个数据平面。
**VkImageSubresourceRange**是**VkImageSubresource**的一个扩展，通过设置基址和索引数量，可以操作更多的小Image.

## Access方式
在Image Layout转换时，常见的方法是使用**vkCmdPipelineBarrier**来进行同步，这个API还可以处理多个Buffer,Memory的同步。对于Image,需要设置**VkImageMemoryBarrier**结构，
```cpp
// Provided by VK_VERSION_1_0
typedef struct VkImageMemoryBarrier {
    VkStructureType            sType;
    const void*                pNext;
    VkAccessFlags              srcAccessMask;
    VkAccessFlags              dstAccessMask;
    VkImageLayout              oldLayout;
    VkImageLayout              newLayout;
    uint32_t                   srcQueueFamilyIndex;
    uint32_t                   dstQueueFamilyIndex;
    VkImage                    image;
    VkImageSubresourceRange    subresourceRange;
} VkImageMemoryBarrier;

// Provided by VK_VERSION_1_3
typedef struct VkImageMemoryBarrier2 {
    VkStructureType            sType;
    const void*                pNext;
    VkPipelineStageFlags2      srcStageMask;
    VkAccessFlags2             srcAccessMask;
    VkPipelineStageFlags2      dstStageMask;
    VkAccessFlags2             dstAccessMask;
    VkImageLayout              oldLayout;
    VkImageLayout              newLayout;
    uint32_t                   srcQueueFamilyIndex;
    uint32_t                   dstQueueFamilyIndex;
    VkImage                    image;
    VkImageSubresourceRange    subresourceRange;
} VkImageMemoryBarrier2;
```
其中**oldLayout/newLayout**分别表示转换前和转换后的Image Layout,常见的Layout有：
- VK_IMAGE_LAYOUT_TRANSFER_SRC/DST_OPTIMAL 对传输优化的Layout,如将Buffer内容复制到Image中后，可以将原来的布局设置为DST(默认是UNDEFINED)，还有mipmap生成时使用blit操作缩放image时，涉及到image的传输，所以分别为SRC,DST。
- VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL shader只读，例如在shader中使用texture函数采样，这种Layout由GPU自主决定，可能会使用tiling格式来优化访存。
里面还有一个access参数，表示访问依赖，可以理解为dstImage的dstAccess访问类型需要依赖srcImage的srcAccess类型，例如，将图片上传到一张纹理然后生成采样器，过程如下，将图片内容复制到Buffer中，然后将Buffer内容复制到Image中，，这里设在复制Buffer时Image Layout已经转为了TRANSFER_DST_OPTIMAL，然后将Layout转为shader只读，那么src/dst Image都是相同一张，srcAccess为**VK_ACCESS_TRANSFER_WRITE_BIT**，即依赖Buffer复制到Image的过程完成，复制过程Image是当作TRANSFER_DST来使用的，所以会是写入，dstAccess为**VK_ACCESS_SHADER_READ_BIT**，描述的是当shader读取Image时，依赖srcImage需要被写入，实现数据同步的过程。
