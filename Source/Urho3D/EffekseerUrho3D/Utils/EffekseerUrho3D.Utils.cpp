// #include <GDScript.hpp>
// #include <NativeScript.hpp>
// #include <VisualScript.hpp>
#include "EffekseerUrho3D.Utils.h"
#include <locale>
#include <codecvt>
// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
template <class Facet> struct deletable_facet : Facet
{
    template <class... Args>
    deletable_facet(Args&&... args)
        : Facet(std::forward<Args>(args)...)
    {
    }
    ~deletable_facet() {}
};
static std::string w2u(const std::u16string& source)
{
    std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> conv16;
    return conv16.to_bytes(source);
//    return std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t>().to_bytes(source);
}
static std::u16string u2w(const std::string& source)
{
    std::wstring_convert<deletable_facet<std::codecvt<char16_t, char, std::mbstate_t>>, char16_t> conv16;
    return conv16.from_bytes(source.c_str());
}

namespace EffekseerUrho3D
{

size_t ToEfkString(char16_t* to, const ea::string& from, size_t size) {
    //auto ws = ea::wstring(from);
    auto ustr = u2w(from.c_str());
    size_t len = (size_t)from.length();
    size_t count = std::min(len, size - 1);
    memcpy(to, ustr.data(), count * sizeof(char16_t));
    to[count] = u'\0';
    return count;
#ifdef _MSC_VER
	// Simple copy
// 	const wchar_t* ustr = from.unicode_str();
// 	size_t len = (size_t)from.length();
//     const char16_t* ustr = ws.CString();
//     size_t len = (size_t)ws.Length();
// 	size_t count = std::min(len, size - 1);
// 	memcpy(to, ustr, count * sizeof(char16_t));
// 	to[count] = u'\0';
// 	return count;
#else
// 	// UTF32 -> UTF16
//     const wchar_t* ustr = ws.CString();
//     size_t len = (size_t)ws.Length();
// 	size_t count = 0;
// 	for (size_t i = 0; i < len; i++) {
// 		wchar_t c = ustr[i];
// 		if (c == 0) {
// 			break;
// 		}
// 		if ((uint32_t)c < 0x10000) {
// 			if (count >= size - 1) break;
// 			to[count++] = (char16_t)c;
// 		} else {
// 			if (count >= size - 2) break;
// 			to[count++] = (char16_t)(((uint32_t)c - 0x10000) / 0x400 + 0xD800);
// 			to[count++] = (char16_t)(((uint32_t)c - 0x10000) % 0x400 + 0xDC00);
// 		}
// 	}
// 	to[count] = u'\0';
// 	return count;
#endif
}

ea::string ToGdString(const char16_t* from)
{
    return ea::string(w2u(from).c_str());

//#ifdef _MSC_VER
//	return Urho3D::String((const wchar_t*)from);
// #else
// 	Urho3D::String result;
// 	while (true) {
// 		// FIXME
// 		wchar_t c[2] = {}; 
// 		c[0] = *from++;
// 		if (c[0] == 0) {
// 			break;
// 		}
// 		result += c;
// 	}
// 	return result;
// #endif
}

// godot::Variant ScriptNew(godot::Ref<godot::Script> script)
// {
// 	using namespace Urho3D;
// 
// 	auto className = script->get_class();
// 	if (className == "GDScript") {
// 		return Ref<GDScript>(script)->new_();
// 	} else if (className == "NativeScript") {
// 		return Ref<NativeScript>(script)->new_();
// 	} else if (className == "VisualScript") {
// 		return Variant();
// 	}
// 	return Variant();
// }

} // namespace EffekseerUrho3D
