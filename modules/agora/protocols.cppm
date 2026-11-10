module;
#include <cstddef>
#include <span>
#include <vector>

export module agora.protocols;
export import agora.core;

export namespace agora::protocols {
// A future internal wire format. No claim of OUCH/ITCH compatibility.
[[nodiscard]] std::vector<std::byte> encode(const Command& command);
[[nodiscard]] Command decode(std::span<const std::byte> bytes);
} // namespace agora::protocols
