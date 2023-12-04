static constexpr uint32_t
WINDOW_WIDTH{ 640 },
WINDOW_HEIGHT{ 480 },
WINDOW_PIXEL_COUNT{ WINDOW_WIDTH * WINDOW_HEIGHT };

static constexpr float ASPECT_RATIO{ static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT };

constexpr char CONTROLS[]
{
	"--------\n"
	"CONTROLS:\n"
	"F3:	 Toggle Bilinear Texture Interpolation\n"
	"F4:	 Toggle Depth Buffer Rendering\n"
	"F5:	 Toggle Rotation\n"
	"F6:	 Toggle Normal Map Use\n"
	"F7:	 Cycle Shading Mode\n"
	"SCROLL:  In-/decrease Field Of View\n"
	"X:	 Take Screenshot\n"
};