
#define rThis *this

#define ForEachUInt(x_i, x_Length)\
		for (uint x_i = 0; x_i < x_Length; ++x_i)

#define ForEachIter(x_it, x_stdCollection, x_VariableName)\
			for (x_stdCollection::iterator x_it = x_VariableName.begin ();\
				x_it != x_VariableName.end ();  ++x_it)

#define ForEachIterC(x_it, x_stdCollection, x_VariableName)\
			for (x_stdCollection::const_iterator x_it = x_VariableName.begin ();\
				x_it != x_VariableName.end ();  ++x_it)
