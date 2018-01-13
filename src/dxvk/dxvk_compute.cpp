#include "dxvk_compute.h"

namespace dxvk {
  
  DxvkComputePipeline::DxvkComputePipeline(
    const Rc<vk::DeviceFn>&       vkd,
    const Rc<DxvkPipelineCache>&  cache,
    const Rc<DxvkShader>&         cs)
  : m_vkd(vkd), m_cache(cache) {
    DxvkDescriptorSlotMapping slotMapping;
    cs->defineResourceSlots(slotMapping);
    
    m_layout = new DxvkBindingLayout(vkd,
      slotMapping.bindingCount(),
      slotMapping.bindingInfos());
    
    m_cs = cs->createShaderModule(vkd, slotMapping);
    
    this->compilePipeline();
  }
  
  
  DxvkComputePipeline::~DxvkComputePipeline() {
    if (m_pipeline != VK_NULL_HANDLE)
      m_vkd->vkDestroyPipeline(m_vkd->device(), m_pipeline, nullptr);
  }
  
  
  void DxvkComputePipeline::compilePipeline() {
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    
    VkComputePipelineCreateInfo info;
    info.sType                = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    info.pNext                = nullptr;
    info.flags                = 0;
    info.stage                = m_cs->stageInfo(nullptr);
    info.layout               = m_layout->pipelineLayout();
    info.basePipelineHandle   = VK_NULL_HANDLE;
    info.basePipelineIndex    = -1;
    
    if (m_vkd->vkCreateComputePipelines(m_vkd->device(),
          m_cache->handle(), 1, &info, nullptr, &m_pipeline) != VK_SUCCESS)
      throw DxvkError("DxvkComputePipeline::DxvkComputePipeline: Failed to compile pipeline");
  }
  
}