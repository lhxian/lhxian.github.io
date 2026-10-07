---
title: Graphics Pipeline
layout: post
date: 2026-10-06
---
# graphicPipeline（vulkan笔记）
无论是使用**RenderPass**还是现代化的**Dynamic Rendering**，渲染管线(Grahics Pipeline)在创建的时候仍然需要固定。
创建graphics pipeline需要设置的内容如下：
## Stage
Stage也就是渲染的多个阶段，用shader来描述，分为几类：
Vertex Shader,Tesselation Shader,  Geometry Shader, Fragment Shaer.
负责通用计算的**Compute Shader**不属于**Grahpics Pipeline**，而是在另外一类pipeline(Compute Pipeline)中创建。

为Stage绑定对应的Shader,首先加载**spv**字节码，转换为**VkShaderModule**的形式，
得到**VkShaderModule**之后，需要包装为**VkPipelineShaderStageCreateInfo**，该结构体中的**stage**成员可以设置shader的属性，可以使用**VK_SHADER_STAGE_VERTEX_BIT/VK_SHADER_STAGE_FRAGMENT_BIT**等flag指定，同时**pName**字段指定shader的入口函数名称。
例如
```cpp
VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
fragShaderStageInfo.module = fragShaderModule;
fragShaderStageInfo.pName = "main";
```
stage在函数`vkCreateGraphicsPipelines()`中创建，里面的**VkGraphicsPipelineCreateInfo**中，有**stageCount/pStage**，包含stage数量和对应的信息描述。

## Vertex Input State
这部分需要设置的是graphics pipeline的输入格式，和openGL中的vertex attribute类似.
两个概念：
Binding
vulkan中，允许一个顶点的数据来自多个Vertex Buffer,例如，一个顶点的数据有pos,color,texCoord等，可以使用多个VertexBuffer来保存定点，如一个VertexBuffer保存pos,一个保存color和texCoord,设置binding可以告诉GPU,获取定点输入时，凑够一个顶点数据可能需要到那些binding位置来查找。
Location
对应vertex shader中输入数据的位置，例如location=0对应pos,location=1对应color等等。
将上面两个信息组合起来可以得到顶点输入的完整信息。
这是一个**VkVertexInputAttributeDescription**结构体的设置。

```cpp
VkVertexInputAttributeDescription attributes[] = {
    {
        .location = 0,
        .binding = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = 0
    },
    {
        .location = 1,
        .binding = 1,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = 0
    }
};
```
需要注意的是，**Attribute**已经包含了顶点会使用到的binding位置，但是无法的找下一个定点在各binding的buffer的位置，也就是**VkVertexInputBindingDescription::stride**信息，所以这两个结构是对输入描述两方面的拆分。

## Input Assembly State
这里描述的是图元信息，也就是定点输入，使用什么图元格式来解释，是**triangle_list,triangle_fan**等等，描述topology，比较简单。
## Viewport State
在pipeline中，viewport和scissor可以有多个。
viewport
viewport用于将NDC坐标转换为设备坐标，一般来说，viewort的宽高比例和vertex shader中设置的proj矩阵的缩放比例相同。
vulkan中viewport可以设置深度的范围，通过最大最小来设置，范围不能超过[0,1]，在pipeline执行的时候，如果使用深度测试，那么会剔除不在viewport深度范围内的点。
对于多个viewport,可以在vertx shader中使用内建变量来访问。
scissor
定义一个矩形剪裁区域，在绘制到窗口时，只保留scissor范围内的内容，其他区域不渲染。

## Rasterizer
设置光栅化的方式，设置信息在结构体**VkPipelineRasterizationStateCreateInfo**中，包含如下内容：
- 深度剪裁，在结构体中，成员**depthClampEnable**中可以设置`VK_TRUE/VK_FALSE`来选择是否对超出范围的深度进行剪裁到边界，同时不丢弃该顶点。
- 图元丢弃，通过结构体中的**rasterizerDiscardEnable**来控制，表示是否丢弃全部的图元(primitives)，这样做不会产生任何的fragment,常见使用在一些不需要绘制像素的场景，例如执行一些只与顶点有关的计算，不需要保存结果。
- 设置线条的长度，**lineWidth**。
- 设置背面剔除，**cullMode**可以设置剔除的面的类型，背面/正面，也可以设置正面的类型，是逆时针还是顺时针。
- depthBias，深度偏移，可能在处理阴影时会使用。

## MSAA
多重采样抗锯齿，设置的信息比较少，可以在获取物理设备时，记录设备能支持的最大采样数。

## Depth Stencil
深度测试和模板测试,使用结构体**VkPipelineDepthStencilStateCreateInfo**。
深度测试需要设置是否开启和深度是否写入，以及深度比较方式（定义偏序），一般保留最近的点，也就是深度较小的点，所以设置为**VK_COMPARE_OP_LESS**。
模板测试如果自定义，需要设置的东西较多，后面再补[TODO]。

## Color Blend
设置颜色混合，对于framebuffer中的color attachment,pipeline输出的颜色需要和framebuffer上的颜色进行混合，可以实现透明物体的显示。
在透明物体的显示中，可以使用如下方法：
开启color blend,设置混合方案，将物体分为不透明和透明两部分，两部分使用不同的pipeline渲染，首先渲染不透明物体，得到深度图（写入）。渲染透明物体的pipeline设置为深度图load时不做处理，同时关闭深度写入，这样做的化对于透明物体之间的空间位置远近可能产生一定影响，因为没有透明物体的深度信息记录。

## Dynamic State
设置在pipeline中可以动态修改的状态，常见如viewport,在窗口大小改变时可能需要更新viewport信息，所以常将viewport设置为动态。
可以设置的状态很多，
```cpp
// Provided by VK_VERSION_1_0
typedef enum VkDynamicState {
    VK_DYNAMIC_STATE_VIEWPORT = 0,
    VK_DYNAMIC_STATE_SCISSOR = 1,
    VK_DYNAMIC_STATE_LINE_WIDTH = 2,
    VK_DYNAMIC_STATE_DEPTH_BIAS = 3,
    VK_DYNAMIC_STATE_BLEND_CONSTANTS = 4,
    VK_DYNAMIC_STATE_DEPTH_BOUNDS = 5,
    VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK = 6,
    VK_DYNAMIC_STATE_STENCIL_WRITE_MASK = 7,
    VK_DYNAMIC_STATE_STENCIL_REFERENCE = 8,
  // Provided by VK_VERSION_1_3
    VK_DYNAMIC_STATE_CULL_MODE = 1000267000,
  // Provided by VK_VERSION_1_3
    VK_DYNAMIC_STATE_FRONT_FACE = 1000267001,
  // Provided by VK_VERSION_1_3
    VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY = 1000267002,
  // Provided by VK_VERSION_1_3
    VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT = 1000267003,
  // Provided by VK_VERSION_1_3
    VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT = 1000267004,
...
};

```
一般来说，如果这些将这些状态设置为动态，如果更新一次，后面该状态都会保持，所以不必在每一帧都设置这些状态（如果每必要的话）。

## Pipeline Layout
设置pipeline中的一些数据布局，例如uniform buffer,texture sample位置信息，绑定多个DescriptorSetLayout。
这里只是指定布局，实际中如果需要使用uniform buffer等资源，可以通过**vkCmdBindDescriptorSets**来设置，descriptorSet中才记录资源对象。

## Pipeline
创建Grahics Pipeline时，需要以上信息，按成员填入：
```cpp
VkGraphicsPipelineCreateInfo pipelineInfo{};
pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
pipelineInfo.stageCount = 2;
pipelineInfo.pStages = shaderStages;
pipelineInfo.pVertexInputState = &vertexInputInfo;
pipelineInfo.pInputAssemblyState = &inputAssembly;
pipelineInfo.pViewportState = &viewportState;
pipelineInfo.pRasterizationState = &rasterizer;
pipelineInfo.pMultisampleState = &multisampling;
pipelineInfo.pDepthStencilState = &depthStencil;
pipelineInfo.pColorBlendState = &colorBlending;
pipelineInfo.pDynamicState = &dynamicState;
pipelineInfo.layout = pipelineLayout;
pipelineInfo.renderPass = renderPass;
pipelineInfo.subpass = 0;
pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
```
