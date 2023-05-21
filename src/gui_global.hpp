#include <tuple>

inline constexpr float g_xResFactor = 0.8f;
inline constexpr float g_yResFactor = 0.8f;
inline constexpr int g_xMinRes = 300;
inline constexpr int g_yMinRes = 300;
inline constexpr int g_defaultVertexSize = 40;

inline constexpr std::tuple<int, int, int> g_colors[] =
{
	std::make_tuple(0xFF, 0xFF, 0xFF), // active
	std::make_tuple(0, 0, 0),          // inactive
	std::make_tuple(0, 0xFF, 0),       // open
	std::make_tuple(0xFF, 0xFF, 0),    // closed
	std::make_tuple(0, 0xFF, 0xFF)     // path
};
