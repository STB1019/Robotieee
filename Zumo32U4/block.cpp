#include "block.hpp"

namespace robotieee {

block::block(id_t id, const point& start_position) : moveable{start_position}, id{id} {

}

block::block(const block& b) : moveable{b.position}, id{b.id} {

}

block::~block() {

}

}

