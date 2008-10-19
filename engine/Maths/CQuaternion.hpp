

class Quaternion
{
public:
	float x, y, z, w;

	Quaternion()
	{
		x = y = z = w = 0.0;
	};

	Quaternion(const Quaternion& q)
	{
		x = q.x;
		y = q.y;
		z = q.z;
		w = q.w;
	};

	~Quaternion(){};

	float length()
	{
		return sqrt(x * x + y * y + z * z + w * w);
	};

	void normalize()
	{
		float L = length();

		x /= L;
		y /= L;
		z /= L;
		w /= L;
	};

	void conjugate()
	{
		x = -x;
		y = -y;
		z = -z;
	};

	Quaternion operator*(const Quaternion& B) const
	{
		Quaternion C;

		C.x = w*B.x + x*B.w + y*B.z - z*B.y;
		C.y = w*B.y - x*B.z + y*B.w + z*B.x;
		C.z = w*B.z + x*B.y - y*B.x + z*B.w;
		C.w = w*B.w - x*B.x - y*B.y - z*B.z;

		return C;
	};
};

