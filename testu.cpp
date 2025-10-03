#include "string.cpp"

int main()
{
	auto x = UnicodeString("hello world\n");
	auto y = UnicodeString("wtf is the kilometer\n");
	UnicodeString res;
	res += x;
	res += y;
	res += UnicodeString("💩💩💩");
	res += "hihihi\n";
	res += UnicodeSymbol('<');
	res += '>';
	res += "\n\n-----\n";
	Byte smile[4] = {0xf0, 0x9f, 0x98, 0x8a};
	res[34] = UnicodeSymbol(smile, 4);
	res.WriteTo();
	printf("size: %d, width: %d\n", res.Length(), res.Width());
	// auto t = UnicodeString("💩 💩 💩\n💩");
	// t.WriteTo();
	// printf("| size %d, width %d\n", t.Length(), t.Width());
	return 0;
}
