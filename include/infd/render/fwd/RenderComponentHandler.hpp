#pragma once

#include "RenderComponent.hpp"
#include "infd/util/handle_vector.hpp"

namespace infd::render {
    using RenderComponentHandler = util::handle_vector<RenderComponent*>::element_handler;
}
