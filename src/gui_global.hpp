#pragma once
#include <tuple>

inline constexpr float g_scaleStep = 0.25f;
inline constexpr float g_xResFactor = 0.8f;
inline constexpr float g_yResFactor = 0.8f;
inline constexpr int g_xMinRes = 300;
inline constexpr int g_yMinRes = 300;
inline constexpr int g_fontSize = 24;
inline constexpr int g_windowExtraHeight = 48;
inline constexpr int g_defaultVertexSize = 48;
inline constexpr int g_descriptionXOffset = 8;
inline constexpr int g_descriptionYOffset = 10;

inline const char* const g_iconsPath = "icons.png";
inline const char* const g_fontPath = "RobotoCondensed-Regular.ttf";

inline constexpr SDL_Color g_fontColor{ 0xFF, 0xFF, 0xFF, 0xFF };
inline constexpr SDL_Color g_fontBgColor{ 0x10, 0x10, 0x10, 0xFF };

inline constexpr std::tuple<int, int, int> g_colors[] =
{
	std::make_tuple(0xFF, 0xFF, 0xFF), // active
	std::make_tuple(0x20, 0x20, 0x20), // inactive
	std::make_tuple(0, 0xFF, 0),       // open
	std::make_tuple(0xFF, 0xFF, 0),    // closed
	std::make_tuple(0, 0xFF, 0xFF)     // path
};

inline const char* const g_descriptions[] =
{
	"Uruchom algorytm",
	"Zatrzymaj algorytm",
	"Wyczyść wszystkie węzły",
	"Wybierz: nawigacja",
	"Wybierz: początek",
	"Wybierz: koniec",
	"Wybierz: aktywny węzeł",
	"Wybierz: nieaktywny węzeł",
	"Zmień wymiary",
	"Generuj losowy szum",
	"Wczytaj z pliku",
	"Zezwól na przekątne",
	"Wybierz: algorytm A*",
	"Wybierz: algorytm Dijkstry"
};
