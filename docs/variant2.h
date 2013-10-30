
class Widget			// Three alternative implementations represented as a union
{
private:
	enum class Tag { point, number, text } type;	// discriminant

	union {		// representation
		point p;	  // point has constructor
		int i;
		string s;	 // string has default constructor, copy operations, and destructor
	};

	// ...

	widget& operator=(const widget& w)	// necessary because of  the string variant
	{
		if (type==Tag::text && w.type==Tag::text) {
			s = w.s;		// usual string assignment
			return *this;
		}

		if (type==Tag::text) s.~string();	// destroy (explicitly!)

		switch (w.type) {
			case Tag::point: p = w.p; break;	// normal copy
			case Tag::number: i = w.i; break;
			case Tag::text: new(&s)(w.s); break;	// placement new
		}
		type = w.type;
		return *this;
	}
};



