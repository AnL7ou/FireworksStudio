#include "FireworkAsset.h"

#include "../template/FireworkTemplate.h"

namespace fireworks {

FireworkAsset::FireworkAsset(uint64_t id,
                             std::string name,
                             std::shared_ptr<::FireworkTemplate> templ,
                             Metadata metadata)
    : m_id(id),
      m_name(std::move(name)),
      m_template(std::move(templ)),
      m_metadata(std::move(metadata)) {}

} // namespace fireworks
