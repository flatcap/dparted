#include <cstdint>
#include "vw.h"
int main()
{
	std::string a = "abc";
	double      b = 100;
	bool        c = 101;
	uint8_t     d = 102;
	int8_t      e = 103;
	uint16_t    f = 104;
	int16_t     g = 105;
	uint32_t    h = 106;
	int32_t     i = 107;
	uint64_t    j = 108;
	int64_t     k = 109;

	Variant<std::string> va(a);
	Variant<double>      vb(b);
	Variant<bool>        vc(c);
	Variant<uint8_t>     vd(d);
	Variant<int8_t>      ve(e);
	Variant<uint16_t>    vf(f);
	Variant<int16_t>     vg(g);
	Variant<uint32_t>    vh(h);
	Variant<int32_t>     vi(i);
	Variant<uint64_t>    vj(j);
	Variant<int64_t>     vk(k);

	std::cout << "std::string:" << std::endl;
	try { std::cout << "\tstd::string " << std::flush; std::cout << (std::string) va << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tdouble      " << std::flush; std::cout << (std::string) vb << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tbool        " << std::flush; std::cout << (std::string) vc << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint8_t     " << std::flush; std::cout << (std::string) vd << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint8_t      " << std::flush; std::cout << (std::string) ve << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint16_t    " << std::flush; std::cout << (std::string) vf << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint16_t     " << std::flush; std::cout << (std::string) vg << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint32_t    " << std::flush; std::cout << (std::string) vh << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint32_t     " << std::flush; std::cout << (std::string) vi << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint64_t    " << std::flush; std::cout << (std::string) vj << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint64_t     " << std::flush; std::cout << (std::string) vk << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	std::cout << std::endl;

	std::cout << "double:" << std::endl;
	try { std::cout << "\tstd::string " << std::flush; std::cout << (double) va << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tdouble      " << std::flush; std::cout << (double) vb << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tbool        " << std::flush; std::cout << (double) vc << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint8_t     " << std::flush; std::cout << (double) vd << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint8_t      " << std::flush; std::cout << (double) ve << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint16_t    " << std::flush; std::cout << (double) vf << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint16_t     " << std::flush; std::cout << (double) vg << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint32_t    " << std::flush; std::cout << (double) vh << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint32_t     " << std::flush; std::cout << (double) vi << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint64_t    " << std::flush; std::cout << (double) vj << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint64_t     " << std::flush; std::cout << (double) vk << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	std::cout << std::endl;

	std::cout << "bool:" << std::endl;
	try { std::cout << "\tstd::string " << std::flush; std::cout << (bool) va << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tdouble      " << std::flush; std::cout << (bool) vb << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tbool        " << std::flush; std::cout << (bool) vc << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint8_t     " << std::flush; std::cout << (bool) vd << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint8_t      " << std::flush; std::cout << (bool) ve << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint16_t    " << std::flush; std::cout << (bool) vf << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint16_t     " << std::flush; std::cout << (bool) vg << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint32_t    " << std::flush; std::cout << (bool) vh << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint32_t     " << std::flush; std::cout << (bool) vi << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint64_t    " << std::flush; std::cout << (bool) vj << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint64_t     " << std::flush; std::cout << (bool) vk << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	std::cout << std::endl;

	std::cout << "uint8_t:" << std::endl;
	try { std::cout << "\tstd::string " << std::flush; std::cout << (int) (uint8_t) va << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tdouble      " << std::flush; std::cout << (int) (uint8_t) vb << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tbool        " << std::flush; std::cout << (int) (uint8_t) vc << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint8_t     " << std::flush; std::cout << (int) (uint8_t) vd << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint8_t      " << std::flush; std::cout << (int) (uint8_t) ve << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint16_t    " << std::flush; std::cout << (int) (uint8_t) vf << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint16_t     " << std::flush; std::cout << (int) (uint8_t) vg << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint32_t    " << std::flush; std::cout << (int) (uint8_t) vh << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint32_t     " << std::flush; std::cout << (int) (uint8_t) vi << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint64_t    " << std::flush; std::cout << (int) (uint8_t) vj << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint64_t     " << std::flush; std::cout << (int) (uint8_t) vk << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	std::cout << std::endl;

	std::cout << "int8_t:" << std::endl;
	try { std::cout << "\tstd::string " << std::flush; std::cout << (int) (int8_t) va << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tdouble      " << std::flush; std::cout << (int) (int8_t) vb << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tbool        " << std::flush; std::cout << (int) (int8_t) vc << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint8_t     " << std::flush; std::cout << (int) (int8_t) vd << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint8_t      " << std::flush; std::cout << (int) (int8_t) ve << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint16_t    " << std::flush; std::cout << (int) (int8_t) vf << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint16_t     " << std::flush; std::cout << (int) (int8_t) vg << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint32_t    " << std::flush; std::cout << (int) (int8_t) vh << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint32_t     " << std::flush; std::cout << (int) (int8_t) vi << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint64_t    " << std::flush; std::cout << (int) (int8_t) vj << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint64_t     " << std::flush; std::cout << (int) (int8_t) vk << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	std::cout << std::endl;

	std::cout << "uint16_t:" << std::endl;
	try { std::cout << "\tstd::string " << std::flush; std::cout << (uint16_t) va << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tdouble      " << std::flush; std::cout << (uint16_t) vb << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tbool        " << std::flush; std::cout << (uint16_t) vc << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint8_t     " << std::flush; std::cout << (uint16_t) vd << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint8_t      " << std::flush; std::cout << (uint16_t) ve << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint16_t    " << std::flush; std::cout << (uint16_t) vf << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint16_t     " << std::flush; std::cout << (uint16_t) vg << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint32_t    " << std::flush; std::cout << (uint16_t) vh << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint32_t     " << std::flush; std::cout << (uint16_t) vi << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint64_t    " << std::flush; std::cout << (uint16_t) vj << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint64_t     " << std::flush; std::cout << (uint16_t) vk << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	std::cout << std::endl;

	std::cout << "int16_t:" << std::endl;
	try { std::cout << "\tstd::string " << std::flush; std::cout << (int16_t) va << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tdouble      " << std::flush; std::cout << (int16_t) vb << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tbool        " << std::flush; std::cout << (int16_t) vc << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint8_t     " << std::flush; std::cout << (int16_t) vd << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint8_t      " << std::flush; std::cout << (int16_t) ve << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint16_t    " << std::flush; std::cout << (int16_t) vf << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint16_t     " << std::flush; std::cout << (int16_t) vg << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint32_t    " << std::flush; std::cout << (int16_t) vh << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint32_t     " << std::flush; std::cout << (int16_t) vi << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint64_t    " << std::flush; std::cout << (int16_t) vj << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint64_t     " << std::flush; std::cout << (int16_t) vk << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	std::cout << std::endl;

	std::cout << "uint32_t:" << std::endl;
	try { std::cout << "\tstd::string " << std::flush; std::cout << (uint32_t) va << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tdouble      " << std::flush; std::cout << (uint32_t) vb << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tbool        " << std::flush; std::cout << (uint32_t) vc << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint8_t     " << std::flush; std::cout << (uint32_t) vd << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint8_t      " << std::flush; std::cout << (uint32_t) ve << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint16_t    " << std::flush; std::cout << (uint32_t) vf << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint16_t     " << std::flush; std::cout << (uint32_t) vg << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint32_t    " << std::flush; std::cout << (uint32_t) vh << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint32_t     " << std::flush; std::cout << (uint32_t) vi << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint64_t    " << std::flush; std::cout << (uint32_t) vj << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint64_t     " << std::flush; std::cout << (uint32_t) vk << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	std::cout << std::endl;

	std::cout << "int32_t:" << std::endl;
	try { std::cout << "\tstd::string " << std::flush; std::cout << (int32_t) va << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tdouble      " << std::flush; std::cout << (int32_t) vb << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tbool        " << std::flush; std::cout << (int32_t) vc << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint8_t     " << std::flush; std::cout << (int32_t) vd << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint8_t      " << std::flush; std::cout << (int32_t) ve << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint16_t    " << std::flush; std::cout << (int32_t) vf << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint16_t     " << std::flush; std::cout << (int32_t) vg << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint32_t    " << std::flush; std::cout << (int32_t) vh << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint32_t     " << std::flush; std::cout << (int32_t) vi << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint64_t    " << std::flush; std::cout << (int32_t) vj << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint64_t     " << std::flush; std::cout << (int32_t) vk << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	std::cout << std::endl;

	std::cout << "uint64_t:" << std::endl;
	try { std::cout << "\tstd::string " << std::flush; std::cout << (uint64_t) va << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tdouble      " << std::flush; std::cout << (uint64_t) vb << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tbool        " << std::flush; std::cout << (uint64_t) vc << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint8_t     " << std::flush; std::cout << (uint64_t) vd << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint8_t      " << std::flush; std::cout << (uint64_t) ve << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint16_t    " << std::flush; std::cout << (uint64_t) vf << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint16_t     " << std::flush; std::cout << (uint64_t) vg << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint32_t    " << std::flush; std::cout << (uint64_t) vh << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint32_t     " << std::flush; std::cout << (uint64_t) vi << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint64_t    " << std::flush; std::cout << (uint64_t) vj << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint64_t     " << std::flush; std::cout << (uint64_t) vk << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	std::cout << std::endl;

	std::cout << "int64_t:" << std::endl;
	try { std::cout << "\tstd::string " << std::flush; std::cout << (int64_t) va << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tdouble      " << std::flush; std::cout << (int64_t) vb << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tbool        " << std::flush; std::cout << (int64_t) vc << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint8_t     " << std::flush; std::cout << (int64_t) vd << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint8_t      " << std::flush; std::cout << (int64_t) ve << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint16_t    " << std::flush; std::cout << (int64_t) vf << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint16_t     " << std::flush; std::cout << (int64_t) vg << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint32_t    " << std::flush; std::cout << (int64_t) vh << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint32_t     " << std::flush; std::cout << (int64_t) vi << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tuint64_t    " << std::flush; std::cout << (int64_t) vj << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	try { std::cout << "\tint64_t     " << std::flush; std::cout << (int64_t) vk << std::endl; } catch (std::runtime_error& e) { std::cout << e.what() << std::endl; }
	std::cout << std::endl;
}
