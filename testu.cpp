#include <wchar.h>
#include <cstdint>

int main()
{
	wchar_t c = L'а';
	uint8_t m[4] = { 0, 0, 0xD0, 0xB0 };

	return c == *((wchar_t*) m);
}
