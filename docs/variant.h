#include <string>

enum class VType {
	vstring	=  0,
	vbool	=  1,
	vchar	=  8,
	vshort	= 16,
	vint	= 32,
	vlong	= 64
};

class Variant {
public:
	Variant() : type(VType::vstring), set(false), value(0) {}
	virtual ~Variant() {};

	VType		type;
	bool		set;
	long		value;
	std::string	str;
};

