module;
#include <stdexcept>
#include <cstddef>
#include <span>
#include <vector>

module agora.protocols;

namespace agora::protocols {
std::vector<std::byte> encode(const Command&) {
    throw std::logic_error("TODO: specify a wire format before implementing encode");
}

Command decode(std::span<const std::byte>) {
    throw std::logic_error("TODO: specify a wire format before implementing decode");
}
} // namespace agora::protocols
