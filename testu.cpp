#include "string.cpp"

int main()
{
	Byte smile[4] = {0xf0, 0x9f, 0x98, 0x8a};
	auto x = UnicodeString("hello world\n");
	auto y = UnicodeString("wtf is the kilometer\n");
	UnicodeString res;
	res.Append(x);
	res.Append(y);
	res.Insert(8, UnicodeString("💩💩💩"));
	res.Prepend(UnicodeString("💩💩💩"));
	res.Prepend(UnicodeString("💩💩💩"));
	res.Insert(3, UnicodeSymbol(smile, 4));
	res.Append("hihihi\n");
	res.Append(UnicodeSymbol('<'));
	res.Append('>');
	res.Append("\n\n-----\n");
	res[34] = UnicodeSymbol(smile, 4);
	res.WriteTo();
	printf("size: %d, width: %d\n", res.Length(), res.Width());
	// auto t = UnicodeString("💩 💩 💩\n💩");
	// t.WriteTo();
	// printf("| size %d, width %d\n", t.Length(), t.Width());
	return 0;
}
